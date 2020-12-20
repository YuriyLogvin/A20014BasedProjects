/*
 * Kernel.h
 *
 *  Created on: 6 вер. 2019 р.
 *      Author: banz
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

void KernelInit();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "IDataTransmitter.h"
#include "InterfaceMetodsDisplay.h"

class Kernel : public IDataTransmitter {
	static void _SendData();
	static void _ResponsePing();
	static void _ProcessDataPacket();
	static void _TransmitData2Display();
	static bool _TransmitValue2Display(const char* valName, int16_t val);
	static bool _TransmitValue2Display(const char* valName, int16_t val, InterfaceMetodsDisplay md);
	static void _TransmitCellValue2Display(const char* valName, uint8_t cellNum, int16_t val);
	static bool _GetDrvValue(int16_t&);
	static bool _GetChgValue(int16_t&);
	static bool _GetDirValue(int16_t&);
	static bool _GetBatHeatValue(int16_t&);
	static bool _GetBatError(int16_t&);
public:
	Kernel() {};

	static void Init();
	static void Tick();
	static void TransmitData2Display();

	virtual void Send(uint8_t* data, uint16_t len);
};

#endif //#ifdef __cplusplus
#endif /* KERNEL_H_ */
