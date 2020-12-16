/*
 * Kernel.cpp
 *
 *  Created on: 6 вер. 2019 р.
 *      Author: banz
 */

#include "Kernel.h"
#include "Hal.h"
#include "ProtocolHost.h"
#include "ReceiveMetodHost.h"
#include "SendMetodHost.h"
#include "EmkStandartMetods.h"
#include "InterfaceMetodsMotorControl.h"
#include "cmsis_os.h"
#include "StatusBits.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern "C" void tskTransmit2UartStart(void const * argument);
extern "C" void tskMainStart(void const * argument);

ProtocolHost* _ProtocolHost;
ReceiveMetodHost* _ReceiveMetodHost;
SendMetodHost* _SendMetodHost;

#define VALUE_NULL ((int16_t)0x8000)
#define VALUE8_NULL 0xff

void KernelInit()
{

	Hal::Init();

	Kernel::Init();

}

void tskMainStart(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		Hal::Tick();
		Kernel::Tick();
		Hal::UpdateWdt();
		osDelay(0);
  }
  /* USER CODE END 5 */
}

void tskTransmit2UartStart(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
	  Kernel::TransmitData2Display();
	  Kernel::ReceiveDataFromDisplay();
	  osDelay(1);
  }
  /* USER CODE END StartTask02 */
}




void Kernel::Init()
{
	_ProtocolHost = new ProtocolHost(EmkAddr::Host);

	_ProtocolHost->DestAddr(EmkAddr::MotorController);

	_ReceiveMetodHost = new ReceiveMetodHost();

	_SendMetodHost = new SendMetodHost();

}

int32_t _KernelTicks = 0;

static int16_t _VoltageNeed = 0;
static int16_t _VoltageHave = 0;
static int16_t _CurrentNeed = 0;
static int16_t _CurrentHave = 0;
static int16_t _SpeedNeed = 0;
static int16_t _SpeedHave = 0;
static int16_t _TurnOnNeed = 0;
static int16_t _TurnOnHave = 0;

void Kernel::Tick()
{

	uint8_t b = 0;
	uint8_t* data = 0;
	uint8_t len = 0;
	//for (;Hal::Rs485->Receive(&b, 1) > 0;)
	for (;Hal::Rs485->Receive(b);)
	{
		data = _ProtocolHost->ReceiveData(b, len);
		if (data)
			break;
	}


	if (data != 0)
	{
		_ReceiveMetodHost->InitNewMetod(data, len);
		_ProcessDataPacket();
		return;
	}

	if (Hal::GetSpendTicks(_KernelTicks) < Hal::GetTicksInMilliSecond() * 100)
		return;

	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsMotorControl::StateGetSet);
	_SendMetodHost->AddArgumentBool(_TurnOnNeed != 0 ? true : false);
	_SendMetodHost->AddArgumentShort(_VoltageNeed);
	_SendMetodHost->AddArgumentShort(_CurrentNeed);
	_SendMetodHost->AddArgumentShort(_SpeedNeed);
	_SendData();

	Hal::LedBlue(!Hal::LedBlue());

	_KernelTicks = Hal::GetTickCount();
}

void Kernel::_ProcessDataPacket()
{

	auto mNum = _ReceiveMetodHost->GetMetodNumber();

	if (_ProtocolHost->PacketAddr() != EmkAddr::Host)
		return;

	if ((EmkMetods)mNum == EmkMetods::Ping)
	{
		return;
	};

	bool bVal = false;
	int16_t sVal = 0;
	switch ((InterfaceMetodsMotorControl)mNum)
	{
	case InterfaceMetodsMotorControl::StateGetSet:
		if (_ReceiveMetodHost->GetArgumentBool(0, bVal))
			_TurnOnHave = bVal ? 1 : 0;
		if (_ReceiveMetodHost->GetArgumentShort(1, sVal))
			_VoltageHave = sVal;
		if (_ReceiveMetodHost->GetArgumentShort(2, sVal))
			_CurrentHave = sVal;
		if (_ReceiveMetodHost->GetArgumentShort(3, sVal))
			_SpeedHave = sVal;
		break;
	default:
		break;
	};
}

bool Kernel::_TransmitEvent2Display(const char* evName, int16_t& val)
{
	if (val == VALUE_NULL)
		return false;

	while (!Hal::Usart->Redy4Send())
		 osDelay(1);
	Hal::Usart->Send("click %s,%i%c%c%c",evName,val,0xff,0xff,0xff);
	val = VALUE_NULL;
	return true;
}

bool Kernel::_TransmitValue2Display(const char* valName, int16_t& val)
{
	if (val == VALUE_NULL)
		return false;

	while (!Hal::Usart->Redy4Send())
		 osDelay(1);
	Hal::Usart->Send("%s=%i%c%c%c",valName,val,0xff,0xff,0xff);
	val = VALUE_NULL;
	return true;
}

int32_t _TransmitData2DisplayTicks = 0;

void Kernel::TransmitData2Display()
{
	if (Hal::GetSpendTicks(_TransmitData2DisplayTicks) < Hal::GetTicksInMilliSecond() * 100)
		return;

	_TransmitValue2Display("volt.val",_VoltageHave);

	_TransmitValue2Display("curr.val",_CurrentHave);

	_TransmitValue2Display("spd.val",_SpeedHave);

	_TransmitEvent2Display("turn",_TurnOnHave);

	_TransmitData2DisplayTicks = Hal::GetTickCount();
}

uint8_t _ReceiveValueBuffPos = 0;
uint8_t _ReceiveValueBuff[48] = {0};

const char* Kernel::_ReceiveValueFromDisplay(int16_t& val)
{
	uint8_t b;
	bool getPacket = false;

	while (Hal::Usart->Receive(b))
	{
		if (b == 0xff)
		{
			if (_ReceiveValueBuffPos == 0)
				continue;
			getPacket = true;
			_ReceiveValueBuff[_ReceiveValueBuffPos] = 0;
			break;
		};
		_ReceiveValueBuff[_ReceiveValueBuffPos++] = b;
		if (_ReceiveValueBuffPos>=sizeof(_ReceiveValueBuff))
			_ReceiveValueBuffPos = 0;
	};

	if (!getPacket)
		return NULL;

	char* pos = strchr((char *)_ReceiveValueBuff, '=');
	if (pos == NULL)
		return NULL;

	*pos = 0;
	pos++;

	val = atoi(pos);

	_ReceiveValueBuffPos = 0;

	return (const char *)_ReceiveValueBuff;
}

void Kernel::ReceiveDataFromDisplay()
{
	int16_t value = 0;
	const char* valName = _ReceiveValueFromDisplay(value);
	if (valName == NULL)
		return;
	if (strcmp(valName, "volt.val") == 0 )
		_VoltageNeed = value;
	else if (strcmp(valName, "curr.val") == 0 )
		_CurrentNeed = value;
	else if (strcmp(valName, "spd.val") == 0 )
		_SpeedNeed = value;
	else if (strcmp(valName, "turn.val") == 0 )
		_TurnOnNeed = value;
}

volatile int _ReceivedPings = 0;

void Kernel::_ResponsePing()
{
	_SendMetodHost->InitNewMetod((uint8_t)EmkMetods::Ping);

	_SendMetodHost->AddArgumentBool(true);

	//version
	_SendMetodHost->AddArgumentByte(2);

	_ReceivedPings++;

	_SendData();
}

/*void Kernel::_ResponseRpm()
{
	if (_MotorControllerInterface == 0)
		return;

	_SendMetodHost->InitNewMetod((uint8_t)InterfaceMetodsSpeedSens::RpmGet);

	_SendMetodHost->AddArgumentUshort(_MotorControllerInterface->Rpm());

	_SendData();
}*/

void Kernel::_SendData()
{

	_ProtocolHost->InitSendData(*_SendMetodHost);

	uint8_t buff[32];

	uint8_t pos = 0;
	for (; pos < sizeof(buff); pos++)
	{
		uint8_t b;
		if (!_ProtocolHost->SendData(b))
			break;
		buff[pos] = b;
	}

	if (pos)
	{
		Hal::Rs485->Send(buff, pos);
	}

}


void Kernel::Send(uint8_t* data, uint16_t len)
{
	Hal::Rs485->Send(data, len);
}

