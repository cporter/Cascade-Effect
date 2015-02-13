#pragma config(Hubs, S1, HTMotor, HTServo, HTMotor, none)
#pragma config(Sensor, S3, IR_l, sensorI2CCustom)
#pragma config(Sensor, S4, IR_r, sensorI2CCustom)
#pragma config(Motor, mtr_S1_C1_1, motorD, tmotorTetrix, openLoop)
#pragma config(Motor, mtr_S1_C1_2, LIFT, tmotorTetrix, openLoop, encoder)
#pragma config(Motor, mtr_S1_C3_1, LEFT_DRIVE, tmotorTetrix, openLoop, encoder)
#pragma config(Motor,                                                          \
               mtr_S1_C3_2,                                                    \
               RIGHT_DRIVE,                                                    \
               tmotorTetrix,                                                   \
               openLoop,                                                       \
               reversed,                                                       \
               encoder)
#pragma config(Servo, srvo_S1_C2_1, TUBE_MAN_B, tServoStandard)
#pragma config(Servo, srvo_S1_C2_2, servo2, tServoNone)
#pragma config(Servo, srvo_S1_C2_3, servo3, tServoNone)
#pragma config(Servo, srvo_S1_C2_4, servo4, tServoNone)
#pragma config(Servo, srvo_S1_C2_5, servo5, tServoNone)
#pragma config(Servo, srvo_S1_C2_6, BALL_DUMP, tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard !!*//

#include "JoystickDriver.c"
#include "autonomous_functions.h"

void initialize_robot () {
    servo[BALL_DUMP] = 0;
    servo[TUBE_MAN_B] = 0;
}

int irDirLeft () {
    int dir, strength;

    if (HTIRS2readEnhanced (IR_l, dir, strength)) {
        return dir;
    } else {
        return 0;
    }
}

int irDirRight () {
    int dir, strength;

    if (HTIRS2readEnhanced (IR_r, dir, strength)) {
        return dir;
    } else {
        return 0;
    }
}

int irPowerLeft () {
    int dir, strength;

    if (HTIRS2readEnhanced (IR_l, dir, strength)) {
        return strength;
    } else {
        return 0;
    }
}

int irPowerRight () {
    int dir, strength;

    if (HTIRS2readEnhanced (IR_r, dir, strength)) {
        return strength;
    } else {
        return 0;
    }
}

void setSpeeds (int left, int right) {
    motor[LEFT_DRIVE] = right;
    motor[RIGHT_DRIVE] = left;
}

void allStop () { setSpeeds (0, 0); }

void sleep (float seconds) { wait10Msec (seconds * 100.0); }

void driveFor (int left, int right, float time) {
    setSpeeds (left, right);
    sleep (time);
}

const int LEFT_SENSOR_CENTER = 5;
const int RIGHT_SENSOR_CENTER = 5;

void dumpBallsInCenterGoal () {
    const float RAISE_LOWER_DURATION = 2.6;
    // Setting this initially low, just in case.
    const float SERVO_DUMP_DURATION = 1.0;
    // raise the dumping arm

    writeDebugStreamLine ("Raising linear slide");
    motor[LIFT] = 100;
    sleep (RAISE_LOWER_DURATION);
    motor[LIFT] = 0;

    writeDebugStreamLine ("Dumpin the game elements");
    servo[BALL_DUMP] = 140;
    sleep (SERVO_DUMP_DURATION);
    writeDebugStreamLine ("Returning servo to start position");
    servo[BALL_DUMP] = 0;

    writeDebugStreamLine ("Lowering linear slide");
    motor[LIFT] = -100;
    sleep (RAISE_LOWER_DURATION);
    writeDebugStreamLine ("Finished with game element dump");
    motor[LIFT] = 0;
}

// Roughly straight-on
void kickstandPositionOne () {
    // Turn left
    driveFor (100, -100, .25);
    // Forward a bit
    driveFor (50, 50, .25);
    // Turn right towards the center element
    driveFor (-100, 100, .5);
    // Drive in, angling right, to knock it down.
    driveFor (60, 100, 1.0);
}

// Somewhat angled
void kickstandPositionTwo () {
    // Angle slightly to the right
    driveFor (30, -60, .25);
    // Go forward a bit
    driveFor (60, 60, .25);
    // Angle right towards the kickstand
    driveFor (60, 100, 1.0);
}

// Roughly straight-on
void kickstandPositionThree () {
    // Turn left
    driveFor (100, -100, .25);
    // Forward a bit
    driveFor (50, 50, .25);
    // Turn right towards the center element
    driveFor (-100, 100, .5);
    // Drive in, angling right, to knock it down.
    driveFor (60, 100, 1.0);
}

void kickstandIRExperimental () {
    // Back up for a second.
    driveFor (-60, -60, 0.5);
    // And then turn a bit to the right
    driveFor (60, -60, .25);

    // Now drive forward unti the IR beacon is on our left.
    setSpeeds (50, 50);

    const int MAX_FWD_ITER = 20;
    for (int i = 0; i < 20; ++i) {
        int left_ir = irDirLeft ();
        if (1 >= left_ir) {
            break;
        }
        sleep (.05);
    }

    // Turn in unti we're angled towards the center element
    setSpeeds (-50, 75);

    const int MAX_ROT_ITER = 20;
    for (int i = 0; i < MAX_ROT_ITER; ++i) {
        int left_ir = irDirLeft ();
        if (3 >= left_ir) {
            break;
        }
        sleep (.05);
    }

    // Ramming speed!
    driveFor (100, 100, .5);
}

// Assumes that we've just dumped in the center goal. We're a few inches
// away from the game element. So we'll nedd to back up a bit, turn right
// 45 degrees, drive forward a bit, turn left 45 degrees and then drive
// forward in a spirited way.
void knockOverKickstand (int position) {
#ifdef USE_CHOSEN_POSITION
    if (1 == position) {
        kickstandPositionOne ();
    } else if (2 == position) {
        kickstandPositionTwo ();
    } else if (3 == position) {
        kickstandPositionThree ();
    } else {
        writeDebugStreamLine ("Unknown kickstand position: %d", position);
    }
#else
    kickstandIRExperimental ();
#endif

    allStop ();
}

// Drive forward to the IR beacon. Return the number of times it senses
// "too far left" minus the number of times it senses "too far right."
int guidedIRForward () {
    const int TOO_CLOSE = 40;
    const int POWER_DIFFERENCE_THRESHOLD = 10;
    int zero_count = 0;

    driveFor (40, 40, .1);

    int balance = 0;
    bool quit = false;
    while (!quit) {
        int l_dir = irDirLeft ();
        int r_dir = irDirRight ();

        writeDebugStreamLine ("Guided IR: Direction: %d, %d", l_dir, r_dir);

        if (LEFT_SENSOR_CENTER == l_dir && RIGHT_SENSOR_CENTER == r_dir) {
            int left = irPowerLeft ();
            int right = irPowerRight ();
            writeDebugStreamLine ("Guided IR: Power: %d, %d", left, right);

            if (TOO_CLOSE > (left + right)) {
                break;
            }

            int diff = left - right;
            if (POWER_DIFFERENCE_THRESHOLD > abs (diff)) {
                writeDebugStreamLine ("Guided IR: Simple Forward");
                setSpeeds (20, 20);
            } else if (left > right) {
                writeDebugStreamLine ("Guided IR: Turn Left");
                setSpeeds (0, 70);
                ++balance;
            } else { // left < right
                writeDebugStreamLine ("Guided IR: Turn Right");
                setSpeeds (70, 0);
                --balance;
            }
        } else {
            quit = true;
        }
        sleep (0.01);
    }

    writeDebugStreamLine ("Guided IR: Done!");
    allStop ();

    return balance;
}

bool doneDrivingForwardTowardsPositionOne () { return 9 == irDirRight (); }

void driveToPositionOne () {
    const float INITIAL_FORWARD_DURATION = 0.1;
    const float INITIAL_LEFT_TURN_DURATION = .60;

    // Get away from the wall
    writeDebugStreamLine ("Position One: Initial Forward");
    driveFor (40, 40, INITIAL_FORWARD_DURATION);

    // Turn left a bit
    writeDebugStreamLine ("Position One: Initial Left");
    driveFor (-40, 40, INITIAL_LEFT_TURN_DURATION);

    // Drive forward until the IR beacon is at position 9 for the right
    // IR sensor

    writeDebugStreamLine ("Position One: Forward Until IR");
    int iterations = 0;
    const int MAX_FORWARD_ITERATIONS = 15;
    setSpeeds (40, 40);
    while (MAX_FORWARD_ITERATIONS > iterations++) {
        if (doneDrivingForwardTowardsPositionOne ()) {
            break;
        }
        sleep (0.1);
    }
    writeDebugStreamLine (
        "Position One: Finished going forward after %d iterations", iterations);
    writeDebugStreamLine ("Position One: Turn towards IR beacon");

    // Turn right until the IR beacon is at position 5 for both sensors
    // and the power difference between the two is minimal.
    setSpeeds (40, -40);
    iterations = 0;
    const int MAX_ROTATE_ITERATIONS = 20;
    const int POWER_SAME_THRESHOLD = 20;
    while (MAX_ROTATE_ITERATIONS > iterations++) {
        int right_dir = irDirRight ();
        int left_dir = irDirLeft ();
        if (LEFT_SENSOR_CENTER == left_dir &&
            RIGHT_SENSOR_CENTER == right_dir) {
            int left_power = irPowerLeft ();
            int right_power = irPowerRight ();
            if (POWER_SAME_THRESHOLD > abs (left_power - right_power)) {
                break;
            } else {
                writeDebugStreamLine ("Powers: %d %d", left_power, right_power);
            }
        } else {
            writeDebugStreamLine ("Directions: %d %d", left_dir, right_dir);
        }
        sleep (0.1);
    }

    // Stop!
    allStop ();
    writeDebugStreamLine (
        "Position One: Finished turning right after %d iterations", iterations);
}

// Drive to the center goal. Return the position number that you believe the
// center goal is in.
int driveToCenterGoal () {
    int left = irDirLeft ();
    int right = irDirRight ();

    bool position_one = false;
    // The center goal shows up at 5/5
    if (5 != left || 5 != right) {
        position_one = true;
        driveToPositionOne ();
    }

    int balance = guidedIRForward ();
    // setSpeeds(-50, -50);
    // sleep(0.4);
    allStop ();

    int position = 0;
    if (position_one) {
        position = 1;
    } else {
        if (balance > 0) { // if we turned right more than left
            position = 2;
        } else {
            position = 3;
        }
    }

    writeDebugStreamLine ("The center element is in position %d", position);

    return position;
}

task main () {
    // waitForStart();
    initialize_robot ();

    int position = driveToCenterGoal ();
    dumpBallsInCenterGoal ();
    knockOverKickstand (position);

    writeDebugStreamLine ("Done!!");
}
