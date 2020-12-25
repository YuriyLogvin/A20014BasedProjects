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
	CellState = 18,
	Temperature1 = 19,
	Temperature2 = 20,
	Temperature3 = 21,
	Temperature4 = 22,
	TemperatureController = 23,
	TemperatureMotor = 24,
	SwitchesInp = 25,
	SwitchesOutp = 26,
	Switches3 = 27,
	Switches4 = 28,
	ControllerErr = 29,
	ChargerErr = 30,
	Size = 31
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

enum class SwitchesInp
{
	None = 0,
	Ignition = (0x1 << 0),
	PluggedIn = (0x1 << 1),
	Forward = (0x1 << 2),
	Backward = (0x1 << 3),
	Sw5 = (0x1 << 4),
	Sw6 = (0x1 << 5),
	Sw7 = (0x1 << 6),
	Sw8 = (0x1 << 7),
	Sw9 = (0x1 << 8),
	Sw10 = (0x1 << 9),
	Sw11 = (0x1 << 10),
	Sw12 = (0x1 << 11),
	Sw13 = (0x1 << 12),
	Sw14 = (0x1 << 13),
	Sw15 = (0x1 << 14),
	Sw16 = (0x1 << 15)
};

enum class SwitchesOutp
{
	None = 0,
	PowerOn = (0x1 << 0),
	ChargeOn = (0x1 << 1),
	Forward = (0x1 << 2),
	Backward = (0x1 << 3),
	DcDcOn = (0x1 << 4),
	Sw6 = (0x1 << 5),
	Sw7 = (0x1 << 6),
	Thermostat1 = (0x1 << 7), //BattCooller, CarCooller, CarHeater, MotorFan, ControllerFan, BattHeater
	Thermostat2 = (0x1 << 8),
	Thermostat3 = (0x1 << 9),
	Thermostat4 = (0x1 << 10),
	VoltageRelay1 = (0x1 << 11), //SteerPump, VacuumPump
	VoltageRelay2 = (0x1 << 12),
	VoltageRelay3 = (0x1 << 13),
	VoltageRelay4 = (0x1 << 14),
	Sw16 = (0x1 << 15)
};

enum class Switches
{
	None = 0,
	Sw1 = (0x1 << 0),
	Sw2 = (0x1 << 1),
	Sw3 = (0x1 << 2),
	Sw4 = (0x1 << 3),
	Sw5 = (0x1 << 4),
	Sw6 = (0x1 << 5),
	Sw7 = (0x1 << 6),
	Sw8 = (0x1 << 7),
	Sw9 = (0x1 << 8),
	Sw10 = (0x1 << 9),
	Sw11 = (0x1 << 10),
	Sw12 = (0x1 << 11),
	Sw13 = (0x1 << 12),
	Sw14 = (0x1 << 13),
	Sw15 = (0x1 << 14),
	Sw16 = (0x1 << 15)
};

template <typename T>
bool operator &(const T& v1, const T& v2)
		{
			return ((uint32_t)v1 &(uint32_t)v1) != 0;
		};

#endif /* INTERFACEMETODSDISPLAY_H_ */
