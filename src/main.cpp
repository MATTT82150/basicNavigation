/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       matth                                                     */
/*    Created:      7/26/2023, 10:46:51 PM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

using namespace vex;

const float TRACKWIDTH = 11.515;
const float PI = 3.1415;
const float WHEEL_WIDTH = 4;
const float DRIVE_WHEEL_GEAR_RATIO = 5;

// A global instance of vex::brain used for printing to the V5 brain screen
vex::brain       Brain;
motor motorLeft = motor(PORT1, ratio18_1, false);
motor motorRight = motor(PORT10, ratio18_1, true);
motor motorArm = motor(PORT8, ratio18_1, false);
motor motorElevation = motor(PORT11, ratio18_1, false);
motor motorPusher = motor(PORT12, ratio18_1, false);


// define your global instances of motors and other devices here

void driveForwardDistance(float distance, bool wait) {
    float motorAngle = DRIVE_WHEEL_GEAR_RATIO * distance / (WHEEL_WIDTH * PI);
    motorLeft.spinFor(forward, motorAngle, rev, wait);
    motorRight.spinFor(forward, motorAngle, rev, wait);
}

void driveForwardSpeed(float speed) {
    motorLeft.spin(forward, speed, pct);
    motorRight.spin(forward, speed, pct);
}

void driveTurn(float angle, bool turnOnCenter, bool wait) {
    float rotations = angle/360;
    float wheelTravel = TRACKWIDTH * PI * rotations;
    float motorAngle = DRIVE_WHEEL_GEAR_RATIO * wheelTravel / (WHEEL_WIDTH * PI);
    if (turnOnCenter) {
        motorLeft.spinFor(forward, motorAngle, rev, wait);
        motorRight.spinFor(reverse, motorAngle, rev, wait);
    } else {
        if (angle > 0) {
            motorLeft.spinFor(forward, motorAngle*2, rev, wait);
        } else {
            motorRight.spinFor(reverse, motorAngle*2, rev, wait);
        }
    }
}

void driveCommand(float forwardPercent, float sidePercent) {
    motorLeft.spin(forward, forwardPercent + sidePercent, pct);
    motorRight.spin(forward, forwardPercent - sidePercent, pct);
}

int main() {

    Brain.Screen.printAt( 10, 50, "Hello V5" );
   
    while(1) {
        
        // Allow other tasks to run
        this_thread::sleep_for(10);
    }
}
