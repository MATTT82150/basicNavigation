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

// A global instance of competition
competition Competition;

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

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // User control code here, inside the loop
  while (1) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo
    // values based on feedback from the joysticks.

    // ........................................................................
    // Insert user code here. This is where you use the joystick values to
    // update your motors, etc.
    // ........................................................................

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
