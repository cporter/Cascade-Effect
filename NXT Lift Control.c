#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     LIFT,          tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     LEFT_DRIVE,    tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     RIGHT_DRIVE,   tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    TUBE_MAN,             tServoStandard)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)


task main() {
	while (true) {
		if (nNxtButtonPressed == 2) {
			motor[LIFT] = 100;
			} else if (nNxtButtonPressed == 1) {
			motor[LIFT] = -100;
			} else {
			motor[LIFT] = 0;
		}
	}
}
