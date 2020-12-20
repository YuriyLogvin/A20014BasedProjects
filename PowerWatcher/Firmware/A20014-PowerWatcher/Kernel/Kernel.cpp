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
#include "cmsis_os.h"
#include "StatusBits.h"

extern "C" void tskTransmit2UartStart(void const * argument);
extern "C" void tskMainStart(void const * argument);

ProtocolHost* _ProtocolHost;
ReceiveMetodHost* _ReceiveMetodHost;
SendMetodHost* _SendMetodHost;

#define VALUE_NULL ((int16_t)0x8000)
#define VALUE8_NULL 0xff


struct CellState
{
	uint8_t CellNum;
	uint8_t CellState;
	int16_t Voltage;
	int16_t Temp;
	int16_t TempBal;
	uint8_t Ver;
};

#define _CellStatesCacheSize 20

CellState _CellStates[_CellStatesCacheSize];
int8_t _CellStatesWPos = 0;
int8_t _CellStatesRPos = 0;

uint8_t _LastBatError = 0;

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
	  osDelay(1);
  }
  /* USER CODE END StartTask02 */
}




void Kernel::Init()
{
	_ProtocolHost = new ProtocolHost(EmkAddr::Display);
	//_ProtocolHost->AddSelfAddr(EmkAddr::CurrentSensor);
	//_ProtocolHost->AddSelfAddr(EmkAddr::VoltageSensor);
	_ProtocolHost->DestAddr(EmkAddr::Host);

	_ReceiveMetodHost = new ReceiveMetodHost();

	_SendMetodHost = new SendMetodHost();


	for (int8_t i = 0; i < _CellStatesCacheSize; i++)
		_CellStates[i].CellNum = VALUE8_NULL;

	_CellStatesWPos = 0;
	_CellStatesRPos = 0;

}

int32_t _KernelTicks = 0;

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

	if (Hal::GetSpendTicks(_KernelTicks) < Hal::GetTicksInMilliSecond() * 10)
		return;

	Hal::LedBlue(!Hal::LedBlue());

	_KernelTicks = Hal::GetTickCount();
}

static int16_t _Voltage = 0;
static int16_t _Current = 0;
static int16_t _Speed = 0;
static int16_t _Distance = 0;
static int16_t _DistanceTotal = 0;
static int16_t _DistanceRest = 0;
static int16_t _Ah = 0;
static int16_t _AhTotal = 0;
static int16_t _Wh = 0;
static int16_t _WhTotal = 0;
static int16_t _Soc = 0;
static Switches1 _Switches = Switches1::None;
static Switches _Switches2 = Switches::None;
static Switches _Switches3 = Switches::None;
static Switches _Switches4 = Switches::None;
static int16_t _ControllerError = 0;

static DriveStates _DriveState;
static int16_t _DriveStateError = 0;
static ChargeStates _ChargeState;
static int16_t _ChargeStateError = 0;
//static int16_t _BmsState = 0;
//static int16_t _CellState = 0;
static int16_t _Temperature1 = 0;
static int16_t _Temperature2 = 0;
static int16_t _Temperature3 = 0;
static int16_t _Temperature4 = 0;

static bool _ReceivedDataStates[(int16_t)InterfaceMetodsDisplay::Size] = {false};

void Kernel::_ProcessDataPacket()
{

	auto mNum = _ReceiveMetodHost->GetMetodNumber();

	if (_ProtocolHost->PacketAddr() != EmkAddr::Display)
		return;

	if ((EmkMetods)mNum == EmkMetods::Ping)
	{
		_ResponsePing();
		return;
	};

	int16_t sVal = 0;
	uint16_t usVal = 0;
	if (mNum < (uint8_t)InterfaceMetodsDisplay::Size)
		_ReceivedDataStates[mNum] = true;

	switch ((InterfaceMetodsDisplay)mNum)
	{
	case InterfaceMetodsDisplay::Voltage:
		_ReceiveMetodHost->GetArgumentShort(0, _Voltage);
		break;
	case InterfaceMetodsDisplay::Current:
		_ReceiveMetodHost->GetArgumentShort(0, _Current);
		break;
	case InterfaceMetodsDisplay::Speed:
		_ReceiveMetodHost->GetArgumentShort(0, _Speed);
		break;
	case InterfaceMetodsDisplay::Distance:
		_ReceiveMetodHost->GetArgumentShort(0, _Distance);
		break;
	case InterfaceMetodsDisplay::DistanceTotal:
		_ReceiveMetodHost->GetArgumentShort(0, _DistanceTotal);
		break;
	case InterfaceMetodsDisplay::DistanceRest:
		_ReceiveMetodHost->GetArgumentShort(0, _DistanceRest);
		break;
	case InterfaceMetodsDisplay::Ah:
		_ReceiveMetodHost->GetArgumentShort(0, _Ah);
		break;
	case InterfaceMetodsDisplay::AhTotal:
		_ReceiveMetodHost->GetArgumentShort(0, _AhTotal);
		break;
	case InterfaceMetodsDisplay::Wh:
		_ReceiveMetodHost->GetArgumentShort(0, _Wh);
		break;
	case InterfaceMetodsDisplay::WhTotal:
		_ReceiveMetodHost->GetArgumentShort(0, _WhTotal);
		break;
	case InterfaceMetodsDisplay::Soc:
		_ReceiveMetodHost->GetArgumentShort(0, _Soc);
		break;
	case InterfaceMetodsDisplay::Temperature1:
		_ReceiveMetodHost->GetArgumentShort(0, _Temperature1); _Temperature1 /= 10;
		break;
	case InterfaceMetodsDisplay::Temperature2:
		_ReceiveMetodHost->GetArgumentShort(0, _Temperature2); _Temperature2 /= 10;
		break;
	case InterfaceMetodsDisplay::Temperature3:
		_ReceiveMetodHost->GetArgumentShort(0, _Temperature3); _Temperature3 /= 10;
		break;
	case InterfaceMetodsDisplay::Temperature4:
		_ReceiveMetodHost->GetArgumentShort(0, _Temperature4); _Temperature4 /= 10;
		break;
	case InterfaceMetodsDisplay::ChargeState:
		if (_ReceiveMetodHost->GetArgumentShort(0, sVal))
		{
			_ChargeState = (ChargeStates)sVal;
			if (_ReceiveMetodHost->GetArgumentShort(1, sVal))
				_ChargeStateError = sVal;
		}
		break;
	case InterfaceMetodsDisplay::DriveState:
		if (_ReceiveMetodHost->GetArgumentShort(0, sVal))
		{
			_DriveState = (DriveStates)sVal;
			if (_ReceiveMetodHost->GetArgumentShort(1, sVal))
				_DriveStateError = sVal;
		}
		break;
	case InterfaceMetodsDisplay::Switches:
		if (_ReceiveMetodHost->GetArgumentUshort(0, usVal))
			_Switches = (Switches1)usVal;
		break;
	case InterfaceMetodsDisplay::Switches2:
		if (_ReceiveMetodHost->GetArgumentUshort(0, usVal))
			_Switches2 = (Switches)usVal;
		break;
	case InterfaceMetodsDisplay::Switches3:
		if (_ReceiveMetodHost->GetArgumentUshort(0, usVal))
			_Switches3 = (Switches)usVal;
		break;
	case InterfaceMetodsDisplay::Switches4:
		if (_ReceiveMetodHost->GetArgumentUshort(0, usVal))
			_Switches4 = (Switches)usVal;
		break;
	case InterfaceMetodsDisplay::ControllerErr:
		_ReceiveMetodHost->GetArgumentShort(0, _ControllerError);
		break;
	case InterfaceMetodsDisplay::CellState:
		_ReceiveMetodHost->GetArgumentByte(0, _CellStates[_CellStatesWPos].CellNum);
		_ReceiveMetodHost->GetArgumentByte(1, _CellStates[_CellStatesWPos].CellState);
		_ReceiveMetodHost->GetArgumentShort(2, _CellStates[_CellStatesWPos].Voltage);
		_ReceiveMetodHost->GetArgumentShort(3, _CellStates[_CellStatesWPos].Temp);
		_ReceiveMetodHost->GetArgumentShort(4, _CellStates[_CellStatesWPos].TempBal);
		_ReceiveMetodHost->GetArgumentByte(5, _CellStates[_CellStatesWPos].Ver);
		if (++_CellStatesWPos >= _CellStatesCacheSize)
			_CellStatesWPos = 0;
		break;
	default:
		break;
	};

}

bool Kernel::_TransmitValue2Display(const char* valName, int16_t val)
{
	while (!Hal::Usart->Redy4Send())
		 osDelay(1);
	Hal::Usart->Send("%s=%i%c%c%c",valName,val,0xff,0xff,0xff);
	//Hal::Usart->Send("%s=%i\n\r",valName,val);
	return true;
}

bool Kernel::_TransmitValue2Display(const char* valName, int16_t val, InterfaceMetodsDisplay md)
{
	if (!_ReceivedDataStates[(uint8_t)md])
		return false;

	_TransmitValue2Display(valName, val);

	_ReceivedDataStates[(uint8_t)md] = false;
	return true;
}

void Kernel::_TransmitCellValue2Display(const char* valName, uint8_t cellNum, int16_t val)
{
	while (!Hal::Usart->Redy4Send())
		 osDelay(1);
	Hal::Usart->Send("%s%i.val=%i%c%c%c",valName,cellNum,val,0xff,0xff,0xff);
	//Hal::Usart->Send("%s%i.val=%i\n\r",valName,cellNum,val);
}

int32_t _TransmitData2DisplayTicks = 0;

void Kernel::TransmitData2Display()
{
	if (Hal::GetSpendTicks(_TransmitData2DisplayTicks) < Hal::GetTicksInMilliSecond() * 100)
		return;

	_TransmitValue2Display("bv.val",_Voltage, InterfaceMetodsDisplay::Voltage);

	_TransmitValue2Display("cur.val",_Current, InterfaceMetodsDisplay::Current);

	_TransmitValue2Display("spd.val",_Speed, InterfaceMetodsDisplay::Speed);

	_TransmitValue2Display("dst.val",_Distance, InterfaceMetodsDisplay::Distance);

	_TransmitValue2Display("dstt.val",_DistanceTotal, InterfaceMetodsDisplay::DistanceTotal);

	_TransmitValue2Display("dstr.val",_DistanceRest, InterfaceMetodsDisplay::DistanceRest);

	_TransmitValue2Display("ah.val", _Ah, InterfaceMetodsDisplay::Ah);

	_TransmitValue2Display("aht.val",_AhTotal, InterfaceMetodsDisplay::AhTotal);

	_TransmitValue2Display("wh.val",_Wh, InterfaceMetodsDisplay::Wh);

	_TransmitValue2Display("wht.val",_WhTotal, InterfaceMetodsDisplay::WhTotal);

	_TransmitValue2Display("soc.val",_Soc, InterfaceMetodsDisplay::Soc);

	_TransmitValue2Display("tc.val",_Temperature1, InterfaceMetodsDisplay::Temperature1);

	_TransmitValue2Display("tm.val",_Temperature2, InterfaceMetodsDisplay::Temperature2);

	_TransmitValue2Display("cerr.val", _ControllerError, InterfaceMetodsDisplay::ControllerErr);

	int16_t iVal = 0;
	if (_ReceivedDataStates[(int16_t)InterfaceMetodsDisplay::Switches])
	{
		if (_GetDrvValue(iVal))
			_TransmitValue2Display("drv.val", iVal);

		if (_GetChgValue(iVal))
			_TransmitValue2Display("chg.val", iVal);

		if (_GetDirValue(iVal))
			_TransmitValue2Display("dir.val", iVal);
	}

	if (_ReceivedDataStates[(int16_t)InterfaceMetodsDisplay::Switches2])
	{
		if (_GetBatHeatValue(iVal))
			_TransmitValue2Display("bht.val", iVal);
	}

	if (_GetBatError(iVal))
		_TransmitValue2Display("berr.val", iVal);

	while (_CellStatesRPos != _CellStatesWPos)
	{
		if (_CellStates[_CellStatesRPos].CellNum != VALUE8_NULL)
		{
			_TransmitCellValue2Display("cv",_CellStates[_CellStatesRPos].CellNum+1, _CellStates[_CellStatesRPos].Voltage);
			_TransmitCellValue2Display("ct",_CellStates[_CellStatesRPos].CellNum+1, _CellStates[_CellStatesRPos].Temp);
			_TransmitCellValue2Display("cs",_CellStates[_CellStatesRPos].CellNum+1, _CellStates[_CellStatesRPos].CellState);
			_CellStates[_CellStatesRPos].CellNum = VALUE8_NULL;
		}

		if (++_CellStatesRPos >= _CellStatesCacheSize)
			_CellStatesRPos = 0;
	}

	_TransmitData2DisplayTicks = Hal::GetTickCount();


}

bool Kernel::_GetBatError(int16_t& val)
{
	return false;
}

bool Kernel::_GetDrvValue(int16_t& val)
{
	/* 0 - None
	 * 1 - red
	 * 2 - Green Not Blinked
	 * 3 - Green Blinked
	 */
	val = 0;
	switch (_DriveState)
	{
	case DriveStates::Drive:
		val = 2;
		break;
	case DriveStates::Error:
		val = 1;
		break;
	case DriveStates::Stopped:
		val = 3;
		break;
	}
	return true;
}

bool Kernel::_GetBatHeatValue(int16_t& val)
{
	/* 0 - None
	 * 1 - Yellow
	 */
	return false;
}

bool Kernel::_GetChgValue(int16_t& val)
{
	/* 0 - None
	 * 1 - Yellow
	 * 2 - Yellow Blinked
	 * 3 - Green
	 * 4 - Red
	 */
	val = 0;
	switch (_ChargeState)
	{
	case ChargeStates::Charging:
		val = 1;
		break;
	case ChargeStates::Stopped:
		if (_Switches & Switches1::PluggedIn)
			val = 2;
		break;
	case ChargeStates::Charged:
		val = 3;
		break;
	case ChargeStates::Error:
		val = 4;
		break;
	}
	return true;
}

bool Kernel::_GetDirValue(int16_t& val)
{
	/* 0 - None
	 * 1 - forward
	 * 2 - backward
	 */
	val = 0;

	if (_Switches & Switches1::Forward)
		val = 1;
	else if (_Switches & Switches1::Backward)
		val = 2;

	return true;
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
		HAL_Delay(1); //ћежду приемом и отправкой должно быть не менее 100мк—ек
		Hal::Rs485->Send(buff, pos);
	}

}


void Kernel::Send(uint8_t* data, uint16_t len)
{
	Hal::Rs485->Send(data, len);
}

