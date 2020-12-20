/*
 * InterfaceMetodsDisplay.h
 *
 *  Created on: 14 бер. 2020 р.
 *      Author: banz
 */

#ifndef INTERFACEMETODSDISPLAY_H_
#define INTERFACEMETODSDISPLAY_H_


enum class InterfaceMetodsDisplay
{
	Voltage = 3,
	Current = 4,
	Speed = 5,
	Distance = 6,
	DistanceTotal = 7,
	DistanceRest = 8,
	Ah = 9,
	AhTotal = 10,
	Wh = 11,
	WhTotal = 12,
	Soc = 13,
	DriveState = 14,
	ChargeState = 15,
	VoltageExt = 16,
	CurrentExt = 17,
	//BmsState = 18, deprecated
	CellState = 19,
	//StatusBits = 20, deprecated
	Temperature1 = 21,
	Temperature2 = 22,
	Temperature3 = 23,
	Temperature4 = 24,
	Switches = 25,
	Switches2 = 26,
	Switches3 = 27,
	Switches4 = 28,
	ControllerErr = 29,
	Size = 30
};

enum class DriveStates
{
	Stopped = 0,
	Drive = 1,
	Error = 2
};

enum class ChargeStates
{
	Stopped = 0,
	Charging = 1,
	Charged = 2,
	Error = 3
};

enum class Switches1
{
	None = 0,
	Ignition = (0x1 << 1),
	PluggedIn = (0x1 << 2),
	Forward = (0x1 << 3),
	Backward = (0x1 << 4),
	Sw5 = (0x1 << 5),
	Sw6 = (0x1 << 6),
	Sw7 = (0x1 << 7),
	Sw8 = (0x1 << 8),
	Sw9 = (0x1 << 9),
	Sw10 = (0x1 << 10),
	Sw11 = (0x1 << 11),
	Sw12 = (0x1 << 12),
	Sw13 = (0x1 << 13),
	Sw14 = (0x1 << 14),
	Sw15 = (0x1 << 15)
};

bool operator &(const Switches1& v1, const Switches1& v2)
		{
			return ((uint8_t)v1 &(uint8_t)v1) != 0;
		};

enum class Switches
{
	None = 0,
	Sw1 = (0x1 << 1),
	Sw2 = (0x1 << 2),
	Sw3 = (0x1 << 3),
	Sw4 = (0x1 << 4),
	Sw5 = (0x1 << 5),
	Sw6 = (0x1 << 6),
	Sw7 = (0x1 << 7),
	Sw8 = (0x1 << 8),
	Sw9 = (0x1 << 9),
	Sw10 = (0x1 << 10),
	Sw11 = (0x1 << 11),
	Sw12 = (0x1 << 12),
	Sw13 = (0x1 << 13),
	Sw14 = (0x1 << 14),
	Sw15 = (0x1 << 15)
};

#endif /* INTERFACEMETODSDISPLAY_H_ */
