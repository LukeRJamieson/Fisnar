// Fisnar Manual Control Class
#pragma once

#include <string>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>

using namespace std;

#define PIO_SIZE 0x30000

#define PIO_BASE 0xFF200000

#define LED_OFFSET 0x00003000
#define SWITCH_OFFSET 0x00004000

#define X_MOTOR_TARGET_OFFSET 0x00006000
#define Y_MOTOR_TARGET_OFFSET 0x00006100
#define Z_MOTOR_TARGET_OFFSET 0x00006200

#define X_MOTOR_LOCATION_OFFSET 0x00006300
#define Y_MOTOR_LOCATION_OFFSET 0x00006400
#define Z_MOTOR_LOCATION_OFFSET 0x00006500

#define X_MOTOR_CTRL_OFFSET 0x00006600
#define Y_MOTOR_CTRL_OFFSET 0x00006700
#define Z_MOTOR_CTRL_OFFSET 0x00006800

#define X_MOTOR_MAX_SPEED_OFFSET 0x00006900
#define Y_MOTOR_MAX_SPEED_OFFSET 0x00006A00
#define Z_MOTOR_MAX_SPEED_OFFSET 0x00006B00

#define X_MOTOR_MIN_SPEED_OFFSET 0x00006C00
#define Y_MOTOR_MIN_SPEED_OFFSET 0x00006D00
#define Z_MOTOR_MIN_SPEED_OFFSET 0x00006E00

#define X_MOTOR_ACCELSTEP_OFFSET 0x00006F00
#define Y_MOTOR_ACCELSTEP_OFFSET 0x00007000
#define Z_MOTOR_ACCELSTEP_OFFSET 0x00007100

#define X_MOTOR_STEPINCREMENT_OFFSET 0x00007200
#define Y_MOTOR_STEPINCREMENT_OFFSET 0x00007300
#define Z_MOTOR_STEPINCREMENT_OFFSET 0x00007400

#define X_MOTOR_UPPER_LIMIT_OFFSET 0x00007500
#define Y_MOTOR_UPPER_LIMIT_OFFSET 0x00007600
#define Z_MOTOR_UPPER_LIMIT_OFFSET 0x00007700

#define X_MOTOR_LOWER_LIMIT_OFFSET 0x00007800
#define Y_MOTOR_LOWER_LIMIT_OFFSET 0x00007900
#define Z_MOTOR_LOWER_LIMIT_OFFSET 0x00007A00

#define X_MOTOR_HOME_LOC_OFFSET 0x00007B00
#define Y_MOTOR_HOME_LOC_OFFSET 0x00007C00
#define Z_MOTOR_HOME_LOC_OFFSET 0x00007D00

#define X_MOTOR_HOME_SPEED_OFFSET 0x00007E00
#define Y_MOTOR_HOME_SPEED_OFFSET 0x00007F00
#define Z_MOTOR_HOME_SPEED_OFFSET 0x00008000

#define X_MOTOR_STATUS_OFFSET 0x00008100
#define Y_MOTOR_STATUS_OFFSET 0x00008200
#define Z_MOTOR_STATUS_OFFSET 0x00008300

#define CLK_F_DIV2_OFFSET 0x00008400

#define POLOLU_OFFSET 0x00008500
#define JOYSTICK_OFFSET 0x00008600

#define DISPENSE_OFFSET 0x00008700

#define OUTPUTS_OFFSET 0x00008800
#define INPUTS_OFFSET 0x00008900

#define TEACH_HPS2FPGA_OFFSET 0x00008A00
#define TEACH_FPGA2HPS_OFFSET 0x00008B00

#define HOME 0x01
#define ENABLE 0x02
#define POLARITY 0x04
#define WINDING_OFF 0x08
#define SOFT_RESET 0x80000000

#define STATUS_RESET 0x09
#define STATUS_HOMING1 0x04
#define STATUS_HOMING2 0x02
#define STATUS_HOMED 0x06
#define STATUS_TARGET_UPPER 0x08
#define STATUS_TARGET_LOWER 0x01
#define STATUS_COUNTING_DOWN 0x03
#define STATUS_COUNTING_UP 0x0C
#define STATUS_INPOSITION 0x0F
#define STATUS_OTHER 0x0A

#define INPUT_LIMITSWITCH_X (1<<0)
#define INPUT_LIMITSWITCH_Y (1<<1)
#define INPUT_LIMITSWITCH_Z (1<<2)
#define INPUT_PANEL_EMG (1<<3)
#define INPUT_PANEL_SWITCH (1<<4)
#define INPUT_PANEL_START (1<<5)
#define INPUT_EXT_EMG (1<<6)
#define INPUT_EXT_NO (1<<7)
#define INPUT_EXT_NC (1<<8)
#define INPUT_EXT_START (1<<9)
#define INPUT_PORT (1<<10)
#define INPUT_ONES (1<<18)
#define INPUT_TENS (1<<22)

#define OUTPUT_PANEL_START (1<<19)
#define OUTPUT_DISPENSE (1<<20)


class FisnarControl {
public:
	// Constructor
	FisnarControl();

	// Methods
    void MoveXY(uint32_t, uint32_t);
    void MoveZ(uint32_t);
    void MoveXYZ(uint32_t, uint32_t, uint32_t);

    void DisplaceX(int);
    void DisplaceY(int);
    void DisplaceZ(int);
    void DisplaceXY(int, int);

    void HomeMotors();
    void EnableMotors();
    void DisableMotors();
    void MotorWindings(bool);
    void MotorWindings(char, bool);
    void Dispense(bool);
    void DispenseTimed(uint32_t);
    uint32_t ReadXYZ(char);
    bool ReadLS(char);
    uint16_t ReadJoystick(char);
    uint16_t ReadJoystickX();
    uint16_t ReadJoystickY();
    uint32_t MotorStatus(char);
    int ReadWheel();
    bool ReadSwitch();
    bool ReadStartButton();
    void WriteStartLED(bool);
    void DirectionTeach(uint8_t);
    uint8_t ReadTeach();
    void WriteTeach(uint8_t);
    uint32_t LocationX();
    uint32_t LocationY();
    uint32_t LocationZ();

    void ChangeMotorSpeed(char, float, float, float);
    void ChangeMotorLimits(char,uint32_t, uint32_t);
    void ChangeHomeLocation(char, uint32_t);



    ~FisnarControl();

private:
    int fd;
	void *base_ptr = NULL;
    void *led_ptr = NULL;
    void *switch_ptr = NULL;
    void *x_motor_target = NULL;
    void *y_motor_target = NULL;
    void *z_motor_target = NULL;
    void *x_motor_location = NULL;
    void *y_motor_location = NULL;
    void *z_motor_location = NULL;
    void *x_motor_ctrl = NULL;
    void *y_motor_ctrl = NULL;
    void *z_motor_ctrl = NULL;
    void *x_motor_max_speed = NULL;
    void *y_motor_max_speed = NULL;
    void *z_motor_max_speed = NULL;
    void *x_motor_min_speed = NULL;
    void *y_motor_min_speed = NULL;
    void *z_motor_min_speed = NULL;
    void *x_motor_accelstep = NULL;
    void *y_motor_accelstep = NULL;
    void *z_motor_accelstep = NULL;
    void *x_motor_stepincrement = NULL;
    void *y_motor_stepincrement = NULL;
    void *z_motor_stepincrement = NULL;
    void *x_motor_upper_limit = NULL;
    void *y_motor_upper_limit = NULL;
    void *z_motor_upper_limit = NULL;
    void *x_motor_lower_limit = NULL;
    void *y_motor_lower_limit = NULL;
    void *z_motor_lower_limit = NULL;
    void *x_motor_home_loc = NULL;
    void *y_motor_home_loc = NULL;
    void *z_motor_home_loc = NULL;
    void *x_motor_home_speed = NULL;
    void *y_motor_home_speed = NULL;
    void *z_motor_home_speed = NULL;
    void *x_motor_status = NULL;
    void *y_motor_status = NULL;
    void *z_motor_status = NULL;
    void *clk_f_div2 = NULL;
    void *joystick = NULL;
    void *dispense = NULL;
    void *outputs = NULL;
    void *inputs = NULL;
    void *teach_hps2fpga = NULL;
    void *teach_fpga2hps = NULL;
};
