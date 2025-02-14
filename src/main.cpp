#include "main.h"
#include "pros/misc.h"
#include "pros/rtos.hpp"

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// motor groups
pros::MotorGroup
    leftMotors({1, 2, 3},
               pros::MotorGearset::blue); // left motor group - ports 3
                                          // (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors(
    {-8, -9, -10},
    pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)

// Inertial Sensor on port 10
pros::Imu imu = pros::Imu::get_imu();

// tracking wheels
// horizontal tracking wheel encoder. Rotation sensor, port 20, not reversed
pros::Rotation horizontalEnc(4);
// vertical tracking wheel encoder. Rotation sensor, port 11, reversed
// -pros::Rotation verticalEnc(-11);
// horizontal tracking wheel. 2.75" diameter, 5.75" offset, back of the robot
// (negative)
lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_2, 2.5);
// vertical tracking wheel. 2.75" diameter, 2.5" offset, left of the robot
// (negative) -lemlib::TrackingWheel vertical(&verticalEnc,
// lemlib::Omniwheel::NEW_2, -2.5);

// drivetrain settings
lemlib::Drivetrain drivetrain(
    &leftMotors,                // left motor group
    &rightMotors,               // right motor group
    12.5,                       // 10 inch track width
    lemlib::Omniwheel::NEW_275, // using new 4" omnis
    450,                        // drivetrain rpm is 360
    2 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
lemlib::ControllerSettings
    linearController(0.5,  // proportional gain (kP)
                     0.0001,   // integral gain (kI)
                     0.01,   // derivative gain (kD)
                     0,   // anti windup
                     0,   // small error range, in inches
                     0, // small error range timeout, in milliseconds
                     0,   // large error range, in inches
                     0, // large error range timeout, in milliseconds
                     0   // maximum acceleration (slew)
    );

// angular motion controller
lemlib::ControllerSettings
    angularController(0.01,   // proportional gain (kP)
                      0.0001,   // integral gain (kI)
                      1,  // derivative gain (kD)
                      0,   // anti windup
                      0,   // small error range, in degrees
                      0, // small error range timeout, in milliseconds
                      0,   // large error range, in degrees
                      0, // large error range timeout, in milliseconds
                      0    // maximum acceleration (slew)
    );

// sensors for odometry
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel
                            nullptr, // vertical tracking wheel 2, set to
                                     // nullptr as we don't have a second one
                            &horizontal, // horizontal tracking wheel
                            nullptr,     // horizontal tracking wheel 2, set to
                                     // nullptr as we don't have a second one
                            &imu // inertial sensor
);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve
    throttleCurve(3,    // joystick deadband out of 127
                  10,   // minimum output where drivetrain will move out of 127
                  1.019 // expo curve gain
    );

// input curve for steer input during driver control
lemlib::ExpoDriveCurve
    steerCurve(3,    // joystick deadband out of 127
               10,   // minimum output where drivetrain will move out of 127
               1.019 // expo curve gain
    );

// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController,
                        sensors, &throttleCurve, &steerCurve);

pros::Motor intake(5, pros::MotorGearset::green);

pros::Motor stakeMotor(-7, pros::MotorGearset::red);

pros::adi::DigitalOut clamp('A');
pros::adi::Encoder stakeEncoder('C', 'D', true);

bool testing = true;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  pros::lcd::initialize(); // initialize brain screen
  chassis.calibrate();     // calibrate sensors

  // the default rate is 50. however, if you need to change the rate, you
  // can do the following.
  // lemlib::bufferedStdout().setRate(...);
  // If you use bluetooth or a wired connection, you will want to have a rate of
  // 10ms

  // for more information on how the formatting for the loggers
  // works, refer to the fmtlib docs

  // thread to for brain screen and position logging
  pros::Task screenTask([&]() {
    while (true) {
      // print robot location to the brain screen
      pros::lcd::print(0, "X: %f", chassis.getPose().x);         // x
      pros::lcd::print(1, "Y: %f", chassis.getPose().y);         // y
      pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
      // log position telemetry
      lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());

      std::cout << "X: " << chassis.getPose().x << std::endl;         // x
      std::cout << "Y: " << chassis.getPose().y << std::endl;         // y
      std::cout << "Theta: " << chassis.getPose().theta << std::endl; // heading
      std::cout << "\n\n\n\n\n" << std::endl;
      // delay to save resources
      pros::delay(50);
    }
  });
}

/**
 * Runs while the robot is disabled
 */
void disabled() {}

/**
 * runs after initialize if the robot is connected to field control
 */
void competition_initialize() {}

// get a path used for pure pursuit
// this needs to be put outside a function
ASSET(example_txt); // '.' replaced with "_" to make c++ happy
ASSET(my_lemlib_tarball_file_txt);

/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the
 * features LemLib has to offer
 */
void autonomous() {
    if(testing) {
        initialize();
        pros::delay(1000);
    }
  // Move to x: 20 and y: 15, and face heading 90. Timeout set to 4000 ms
  chassis.setPose(0, 0, 0);
  chassis.turnToHeading(90, 9999999);
  // chassis.moveToPose(20, 15, 90, 4000);
  // Move to x: 0 and y: 0 and face heading 270, going backwards. Timeout set to
  // 4000ms
//   chassis.moveToPose(0, 0, 270, 4000, {.forwards = false});
//   // cancel the movement after it has traveled 10 inches
//   chassis.waitUntil(10);
//   chassis.cancelMotion();
//   // Turn to face the point x:45, y:-45. Timeout set to 1000
//   // dont turn faster than 60 (out of a maximum of 127)
//   chassis.turnToPoint(45, -45, 1000, {.maxSpeed = 60});
//   // Turn to face a direction of 90º. Timeout set to 1000
//   // will always be faster than 100 (out of a maximum of 127)
//   // also force it to turn clockwise, the long way around
//   chassis.turnToHeading(
//       90, 1000, {.direction = AngularDirection::CW_CLOCKWISE, .minSpeed = 100});
//   // Follow the path in path.txt. Lookahead at 15, Timeout set to 4000
//   // following the path with the back of the robot (forwards = false)
//   // see line 116 to see how to define a path
//   chassis.follow(example_txt, 15, 4000, false);
//   // wait until the chassis has traveled 10 inches. Otherwise the code directly
//   // after the movement will run immediately Unless its another movement, in
//   // which case it will wait
//   chassis.waitUntil(10);
//   pros::lcd::print(4, "Traveled 10 inches during pure pursuit!");
  // wait until the movement is done
  chassis.waitUntilDone();
  pros::lcd::print(4, "pure pursuit finished!");
}

/**
 * Runs in driver control
 */

bool isClamped = false;
bool isIntaking = false;
bool intakeReversed = false;

bool stakeIsActive = false;
int stakeLowerLimit = 22;
int stakeUpperLimit = 100;

void buttonControls(void *param) {
    stakeMotor.set_brake_mode(pros::MotorBrake::hold);
    stakeMotor.set_zero_position(0);
  while (true) {
    // clamp
    if (controller.get_digital(DIGITAL_A) && isClamped) {
      clamp.set_value(false);
      isClamped = false;
      while (controller.get_digital(DIGITAL_A)) {
        pros::delay(50);
      }
    } else if (controller.get_digital(DIGITAL_A) && !isClamped) {
      clamp.set_value(true);
      isClamped = true;
      while (controller.get_digital(DIGITAL_A)) {
        pros::delay(50);
      }
    }

    // intake
    if (controller.get_digital(DIGITAL_R2) && (!isIntaking || intakeReversed)) {
      intake.move(127);
      isIntaking = true;
      intakeReversed = false;
      while (controller.get_digital(DIGITAL_R2)) {
        pros::delay(50);
      }
    } else if (controller.get_digital(DIGITAL_R2) && isIntaking &&
               !intakeReversed) {
      intake.brake();
      isIntaking = false;
      intakeReversed = false;
      while (controller.get_digital(DIGITAL_R2)) {
        pros::delay(50);
      }
    } else if (controller.get_digital(DIGITAL_R1) &&
               (!isIntaking || !intakeReversed)) {
      intake.move(-127);
      isIntaking = true;
      intakeReversed = true;
      while (controller.get_digital(DIGITAL_R1)) {
        pros::delay(50);
      }
    } else if (controller.get_digital(DIGITAL_R1) && isIntaking &&
               intakeReversed) {
      intake.brake();
      isIntaking = false;
      intakeReversed = false;
      while (controller.get_digital(DIGITAL_R1)) {
        pros::delay(50);
      }
    }

    // wall stake arm
    if (controller.get_digital(DIGITAL_B)) {
      stakeIsActive = !stakeIsActive;
      if (stakeIsActive) {
        stakeMotor.move_absolute( 180, 100);
      } else {
        stakeMotor.move_absolute(0, 100);
      }
      while (controller.get_digital(DIGITAL_B)) {
        pros::delay(50);
      }
    }
    if (stakeIsActive) {
      if (controller.get_digital(DIGITAL_L1)) {
        stakeMotor.move(40);
      } else if (controller.get_digital(DIGITAL_L2)) {
        stakeMotor.move(-40);
      } else {
        stakeMotor.brake();
      }
    }
  }
}
void opcontrol() {
  pros::Task BUTTON_CONTROLLS(buttonControls);
  
  leftMotors.set_brake_mode_all(pros::MotorBrake::brake);
  rightMotors.set_brake_mode_all(pros::MotorBrake::brake);
  // controller
  // loop to continuously update motors
  while (true) {
    // get joystick positions
    int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    // move the chassis with curvature drive
    chassis.arcade(leftY, -rightX, false, 0.35);
    // delay to save resources
    pros::delay(10);
  }
}