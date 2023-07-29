/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       matth                                                     */
/*    Created:      7/26/2023, 10:46:51 PM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#include "math.h"
#include "iostream"

using namespace vex;

// A global instance of competition
competition Competition;

// constants used for physics and motion
const float TRACKWIDTH = 11.515;
const float PI = 3.1415;
const float WHEEL_WIDTH = 4;
const float DRIVE_WHEEL_GEAR_RATIO = 5;

// A global instance of vex::brain used for printing to the V5 brain screen
vex::brain       Brain;

// setting motors
motor motorLeft = motor(PORT1, ratio18_1, true);
motor motorRight = motor(PORT10, ratio18_1, false);
motor motorShooter = motor(PORT8, ratio18_1, false);
motor motorArm = motor(PORT11, ratio18_1, false);
motor motorPusher = motor(PORT12, ratio18_1, false);

// setting inertial
inertial IMU = inertial(PORT3);

// setting camera color signatures and making a constant to be able to refer to color signatures by the object they detect rather than their signature number
const int RED_BALL = 0;
const int BLUE_BALL = 1;
const int YELLOW_BALL = 2;
const int RED_HOOP = 3;
const int BLUE_HOOP = 4;
const int GREEN_INDICATOR = 5;
const int YELLOW_INDICATOR = 6;
vex::vision::signature RED_BALL_SIG = vex::vision::signature(1, 0, 0, 0, 0, 0, 0, 0, 0);
vex::vision::signature BLUE_BALL_SIG = vex::vision::signature(1, 0, 0, 0, 0, 0, 0, 0, 0);
vex::vision::signature YELLOW_BALL_SIG = vex::vision::signature(1, 0, 0, 0, 0, 0, 0, 0, 0);
vex::vision::signature RED_HOOP_SIG = vex::vision::signature(1, 0, 0, 0, 0, 0, 0, 0, 0);
vex::vision::signature BLUE_HOOP_SIG = vex::vision::signature(1, 0, 0, 0, 0, 0, 0, 0, 0);
vex::vision::signature GREEN_INDICATOR_SIG = vex::vision::signature(1, 0, 0, 0, 0, 0, 0, 0, 0);
vex::vision::signature YELLOW_INDICATOR_SIG = vex::vision::signature(1, 0, 0, 0, 0, 0, 0, 0, 0);
vision camera = vision(PORT2, 50, RED_BALL_SIG, BLUE_BALL_SIG, YELLOW_BALL_SIG, RED_HOOP_SIG, BLUE_HOOP_SIG, GREEN_INDICATOR_SIG, YELLOW_INDICATOR_SIG);
int CAMERA_PIXEL_WIDTH = 316;
int CAMERA_PIXEL_HEIGHT = 212;
int CAMERA_HORIZONTAL_FOV = 61;
int CAMERA_VERTICAL_FOV = CAMERA_HORIZONTAL_FOV * (CAMERA_PIXEL_HEIGHT / CAMERA_PIXEL_WIDTH);
float RADIANS_TO_DEGREES = 180/PI;
float CAMERA_DEGREES_PER_PIXEL = CAMERA_PIXEL_WIDTH/CAMERA_HORIZONTAL_FOV;
float CAMERA_RADIANS_PER_PIXEL = CAMERA_DEGREES_PER_PIXEL / RADIANS_TO_DEGREES;
float WIDTH_BALL = 2.1;
float WIDTH_HOOP = 0;
float WIDTH_INDICATOR = 0;

// functions and stuff below

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

void driveGlobal(float xPos, float yPos, float angle) {

}

void resetArmAngle() {
  while (motorArm.current() < 2 || motorArm.velocity(rpm) > 5) {
    motorArm.spin(forward, 10, pct);
  }
  motorArm.stop();
  motorArm.resetPosition();
}

void reverseToWall() {
  while (motorLeft.current() < 2 || motorLeft.velocity(rpm) > 5) {
    motorLeft.spin(forward, 10, pct);
    motorRight.spin(forward, 10, pct);
  }
  motorLeft.stop();
  motorRight.stop();
}

void resetIMU() {
  IMU.calibrate();
  while(IMU.isCalibrating()) {
    wait(1, msec);
  }
}

void setArmAngle(float angle) {
  motorArm.spinTo(angle, deg, true);
}

void setShooterDirection(vex::directionType direction) {
  motorShooter.spin(direction, 100, pct);
}

vex::vision::object getLargestObject(vex::vision::object* objects) {
  int largestXSize = 0;
  vex::vision::object largestObject;
  for (int i = 0; i < sizeof(objects); i++) {
    if (objects[i].width > largestXSize) {
      largestXSize = objects[i].width;
      largestObject = objects[i];
    }
  }
  return largestObject;
}

int getLargestObjectPosX_SingleScan() {
  return camera.largestObject.centerX - CAMERA_PIXEL_WIDTH*0.5; // X is normally based on 0-CAMERA_PIXEL_WIDTH, but this adjusts it so centered = 0
}

int getLargestObjectPosX_MultipleScans(vex::vision::object* objects) {
  return getLargestObject(objects).centerX - CAMERA_PIXEL_WIDTH*0.5; // X is normally based on 0-CAMERA_PIXEL_WIDTH, but this adjusts it so centered = 0
}

float getObjectDistance(int objectPixelWidth, float objectWidth) {
  float objectFullArcAngle = objectPixelWidth * CAMERA_RADIANS_PER_PIXEL;
  float objectDistance = objectWidth / tan(objectFullArcAngle);
  return objectDistance;
}

float getLargestObjectDistance_SingleScan(float objectWidth) {
  return getObjectDistance(camera.largestObject.width, objectWidth);
}

float getLargestObjectDistance_MultipleScans(float objectWidth, vex::vision::object* objects) {
  return getObjectDistance(getLargestObject(objects).width, objectWidth);
}

vex::vision::object* snapshotMultipleColors(int* colors) {
  int size;
  int i;
  for (int c = 0; c < sizeof(colors); c++) {
    camera.takeSnapshot(colors[c]);
    size += camera.objectCount;
  }
  vex::vision::object* combinedData = new vex::vision::object[size];
  for (int c = 0; c < sizeof(colors); c++) {
    camera.takeSnapshot(colors[c]);
    for (int j = 0; j < camera.objectCount; j++) {
      combinedData[i] = camera.objects[j];
      i++;
    }
  }
  return combinedData;
}

void yellowBallRun() {
  float YELLOW_BALL_RUN_TURNING_GAIN = 0.01;
  // Shooter should already be set to intake mode from the setup phase.
  int ballDistance = 99999;
  // Angle turret to yellow-ball-grabbing height
  setArmAngle(15);
  // Align camera with largest yellow target and slam the throttle. Get to the yellow ball as fast as possible for the easy 10 points.
  while (ballDistance > 5) {
    camera.takeSnapshot(YELLOW_BALL);
    ballDistance = getLargestObjectDistance_SingleScan(WIDTH_BALL);
    float error = getLargestObjectPosX_SingleScan() * YELLOW_BALL_RUN_TURNING_GAIN;
    driveCommand(30, error);
  }
  // Reverse to around the starting position.
  driveForwardDistance(5, true);
  driveForwardDistance(-20, true);
}

void searchForBalls() {
  // Rev shooter to full/partial speed reverse
  setShooterDirection(forward);
  // threshold = number of balls we need to have loaded before we start shooting them.
  vex::vision::object* balls = snapshotMultipleColors(new int[RED_BALL, BLUE_BALL, YELLOW_BALL]);
  

  // While >0 balls seen, or ball count is below the threshold:
  //   a. Check FOV for balls. If ball count is below the threshold, reverse/look around until one is found.
  //   b. Align to ball, move towards ball until at edge of camera's visual range
  //   c. Preset ball-grab sequence since now blind
  //   d. Keep track of held balls with a list
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
  //
  // uses a physical-contact position resetting technique
  resetArmAngle();
  // shooter should be on intake mode at the beginning
  setShooterDirection(reverse);
  // calibrate the position + rotation by reversing back to the wall and resetting the IMU's position
  reverseToWall();
  resetIMU();

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

  // ..........................................................................
  // What I'm thinking for the auto loop:
  //
  //
  // 
  // PART 1: YELLOW BALL RUN
  yellowBallRun();
  
  // PART 2: SEARCH FOR BALLS
  searchForBalls();
  // PART 3: SHOOT BALLS
  // Angle turret to some positive value like +30deg
  // Rev shooter to full speed forwards
  // While >0 balls in clip:
  //   a. Reverse until a hoop of desired color is seen / maybe recall some preset hoop position
  //   b. Align to hoop
  //   c. Calculate hoop distance based off of vertical+horizontal size in camera's FOV and known vertical distance
  //   d. Calculate necessary turret angle + shooter spinrate based off of math
  //   e. Adjust bot angle + turret angle + shooter spinrate until within a margin of error
  //   f. Push one ball out with the piston system (the piston positions are preset/known, no calculations or readings needed to see if the ball is shot out)
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

    // ..........................................................................
    // What I'm thinking for the manual loop:
    //
    // I'm pretty sure all we're allowed to do here is move motors according to
    // controller movements, so all I can write here are the button binds I want
    //
    // LEFT JOYSTICK
    //   X: Large-scale forwards movement
    //   Y: Large-scale turning
    //
    // RIGHT JOYSTICK
    //   X: Fine-control turning
    //   Y: Turret elevation control
    //
    // LEFT BUMPERS
    //   Top: Set shooter to expulsion.
    //   Bottom: Set shooter to intake.
    //
    // RIGHT BUMPERS
    //   Top: Push piston outward. Probably set to decay, so you just hold RB
    //        until a ball comes out and it will slowly reset.
    //   Bottom: Push piston inward. Pretty much just a way to make the natural
    //           decay faster.
    // ..........................................................................

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  /* ---------- REAL CODE FOR THE COMPETITION ---------- */
  /*// Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }*/

  /* ---------- CODE THAT TESTS FUNCTIONS ---------- */

  /*Brain.Screen.setCursor(1,1);
  Brain.Screen.print("resetArmAngle(); "); try {resetArmAngle(); Brain.Screen.print("succeeded\n");} catch (const std::exception& e) {Brain.Screen.print("failed\n")} wait(1000, msec);
  Brain.Screen.print("setArmAngle(15); "); try {setArmAngle(15); Brain.Screen.print("succeeded\n");} catch (const std::exception& e) {Brain.Screen.print("failed\n")} wait(1000, msec);
  Brain.Screen.print("setShooterDirection(forward); "); try {setShooterDirection(forward); Brain.Screen.print("succeeded\n");} catch (const std::exception& e) {Brain.Screen.print("failed\n")} wait(1000, msec);
  Brain.Screen.print("camera.takeSnapshot(RED_BALL); "); try {camera.takeSnapshot(RED_BALL); Brain.Screen.print("succeeded\n");} catch (const std::exception& e) {Brain.Screen.print("failed\n")} wait(1000, msec);
  Brain.Screen.print("getLargestObjectPosX(); "); try {getLargestObjectPosX(); Brain.Screen.print("succeeded\n");} catch (const std::exception& e) {Brain.Screen.print("failed\n")} wait(1000, msec);
  Brain.Screen.print("getLargestObjectDistance_SingleScan(WIDTH_BALL); "); try {getLargestObjectDistance_SingleScan(WIDTH_BALL); Brain.Screen.print("succeeded\n");} catch (const std::exception& e) {Brain.Screen.print("failed\n")} wait(1000, msec);
  Brain.Screen.print("yellowBallRun(); "); try {yellowBallRun(); Brain.Screen.print("succeeded\n");} catch (const std::exception& e) {Brain.Screen.print("failed\n")} wait(1000, msec);
  */

 searchForBalls();
}
