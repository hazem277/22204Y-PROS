#include "devices.h"

pros::Controller controller(CONTROLLER_MASTER);

pros::MotorGroup rightMotors({1, 2, 3}, pros::MotorGearset::blue);
pros::MotorGroup leftMotors({-8, -9, -10}, pros::MotorGearset::blue);

pros::Imu inertialSensor = pros::Imu::get_imu();
pros::Rotation horizontalEncoder(1);
lemlib::TrackingWheel horizontalTrackingWheel(&horizontalEncoder, lemlib::Omniwheel::NEW_275, -5.75);

// drivetrain settings
lemlib::Drivetrain drivetrain(
    &leftMotors,                // left motor group
    &rightMotors,               // right motor group
    12.5,                       // 10 inch track width
    lemlib::Omniwheel::NEW_275, // using new 2.75" omnis
    450,                        // drivetrain rpm is 450
    2                           // horizontal drift is 2 (for now)
);

lemlib::OdomSensors sensors(
    nullptr,                    // vertical tracking wheel 1, set to null
    nullptr,                    // vertical tracking wheel 2, set to nullptr as we are using IMEs
    &horizontalTrackingWheel,   // horizontal tracking wheel 1
    nullptr,                    // horizontal tracking wheel 2, set to nullptr as we don't have a second one
    &inertialSensor             // inertial sensor
);

// lateral PID controller
lemlib::ControllerSettings lateralController(
    10,     // proportional gain (kP)
    0,      // integral gain (kI)
    3,      // derivative gain (kD)
    3,      // anti windup
    1,      // small error range, in inches
    100,    // small error range timeout, in milliseconds
    3,      // large error range, in inches
    500,    // large error range timeout, in milliseconds
    20      // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angularController(
    2,      // proportional gain (kP)
    0,      // integral gain (kI)
    10,     // derivative gain (kD)
    3,      // anti windup
    1,      // small error range, in degrees
    100,    // small error range timeout, in milliseconds
    3,      // large error range, in degrees
    500,    // large error range timeout, in milliseconds
    0       // maximum acceleration (slew)
);

lemlib::Chassis chassis(
    drivetrain,         // drivetrain settings
    lateralController,  // lateral PID settings
    angularController,  // angular PID settings
    sensors             // odometry sensors
);

pros::Motor intake(5, pros::MotorGearset::green);

pros::Motor wallStakeArm(7, pros::MotorGearset::red);

pros::ADIDigitalOut clamp ('A');