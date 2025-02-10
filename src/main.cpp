#include "main.h"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

lemlib_tarball::Decoder decoder(my_lemlib_tarball_file_txt);

void autonomous() {
	chassis.setPose(40, 0, 0);

	// Parameters: path, lookahead distance, timeout
	//chassis.follow(decoder["Path 1"], 10, 2000);
  	//chassis.follow(decoder["Path 2"], 10, 2000);
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

bool isClamped = false;
bool isIntaking = false;
bool intakeReversed = false;

void buttonControls(void* param) {
   while(true) {
		controller.clear_line(1);
		controller.print(1, 1, "%d", isIntaking);
		if(controller.get_digital(DIGITAL_A) && isClamped) {
			clamp.set_value(false);
			isClamped = false;
			while(controller.get_digital(DIGITAL_A)){
				pros::delay(50);
			}
		}
		else if(controller.get_digital(DIGITAL_A) && !isClamped) {
			clamp.set_value(true);
			isClamped = true;
			while(controller.get_digital(DIGITAL_A)){
				pros::delay(50);
			}
		}
   	if (controller.get_digital(DIGITAL_R2) && (!isIntaking || intakeReversed)) {
			intake.move(127);
			isIntaking = true;
			intakeReversed = false;
			while(controller.get_digital(DIGITAL_R2)){
				pros::delay(50);
			}
		}
		else if (controller.get_digital(DIGITAL_R2) && isIntaking && !intakeReversed) {
			intake.brake();
			isIntaking = false;
			intakeReversed = false;
			while(controller.get_digital(DIGITAL_R2)){
				pros::delay(50);
			}
		}
		else if(controller.get_digital(DIGITAL_R1) && (!isIntaking || !intakeReversed)) {
			intake.move(-127);
			isIntaking = true;
			intakeReversed = true;
			while(controller.get_digital(DIGITAL_R1)){
				pros::delay(50);
			}
		}
		else if(controller.get_digital(DIGITAL_R1) && isIntaking && intakeReversed) {
			intake.brake();
			isIntaking = false;
			intakeReversed = false;
			while(controller.get_digital(DIGITAL_R1)){
				pros::delay(50);
			}
		}
	}
}


void opcontrol() {

	pros::Task BUTTON_CONTROLLS(buttonControls);
	leftMotors.set_brake_mode_all(pros::MotorBrake::brake);
	rightMotors.set_brake_mode_all(pros::MotorBrake::brake);

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		chassis.arcade(controller.get_analog(ANALOG_LEFT_Y), controller.get_analog(ANALOG_RIGHT_X));
		pros::delay(20);                                   // Run for 20 ms then update
	}
}