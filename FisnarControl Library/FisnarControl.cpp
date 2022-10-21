#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include "FisnarControl.h"
#include <iostream>

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

using namespace std;

FisnarControl::FisnarControl()
{
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1)
    {
        printf("Error opening /dev/mem\n");
        
    }
    printf("Opened /dev/mem\n");
    printf("%d\n", fd);


    printf("Mapping memory\n");
    base_ptr = mmap(NULL, PIO_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, PIO_BASE);
    perror("mmap");

    led_ptr = base_ptr + LED_OFFSET;
    switch_ptr = base_ptr + SWITCH_OFFSET;
    x_motor_target = base_ptr + X_MOTOR_TARGET_OFFSET;
    y_motor_target = base_ptr + Y_MOTOR_TARGET_OFFSET;
    z_motor_target = base_ptr + Z_MOTOR_TARGET_OFFSET;
    x_motor_location = base_ptr + X_MOTOR_LOCATION_OFFSET;
    y_motor_location = base_ptr + Y_MOTOR_LOCATION_OFFSET;
    z_motor_location = base_ptr + Z_MOTOR_LOCATION_OFFSET;
    x_motor_ctrl = base_ptr + X_MOTOR_CTRL_OFFSET;
    y_motor_ctrl = base_ptr + Y_MOTOR_CTRL_OFFSET;
    z_motor_ctrl = base_ptr + Z_MOTOR_CTRL_OFFSET;
    x_motor_max_speed = base_ptr + X_MOTOR_MAX_SPEED_OFFSET;
    y_motor_max_speed = base_ptr + Y_MOTOR_MAX_SPEED_OFFSET;
    z_motor_max_speed = base_ptr + Z_MOTOR_MAX_SPEED_OFFSET;
    x_motor_min_speed = base_ptr + X_MOTOR_MIN_SPEED_OFFSET;
    y_motor_min_speed = base_ptr + Y_MOTOR_MIN_SPEED_OFFSET;
    z_motor_min_speed = base_ptr + Z_MOTOR_MIN_SPEED_OFFSET;
    x_motor_accelstep = base_ptr + X_MOTOR_ACCELSTEP_OFFSET;
    y_motor_accelstep = base_ptr + Y_MOTOR_ACCELSTEP_OFFSET;
    z_motor_accelstep = base_ptr + Z_MOTOR_ACCELSTEP_OFFSET;
    x_motor_stepincrement = base_ptr + X_MOTOR_STEPINCREMENT_OFFSET;
    y_motor_stepincrement = base_ptr + Y_MOTOR_STEPINCREMENT_OFFSET;
    z_motor_stepincrement = base_ptr + Z_MOTOR_STEPINCREMENT_OFFSET;
    x_motor_upper_limit = base_ptr + X_MOTOR_UPPER_LIMIT_OFFSET;
    y_motor_upper_limit = base_ptr + Y_MOTOR_UPPER_LIMIT_OFFSET;
    z_motor_upper_limit = base_ptr + Z_MOTOR_UPPER_LIMIT_OFFSET;
    x_motor_lower_limit = base_ptr + X_MOTOR_LOWER_LIMIT_OFFSET;
    y_motor_lower_limit = base_ptr + Y_MOTOR_LOWER_LIMIT_OFFSET;
    z_motor_lower_limit = base_ptr + Z_MOTOR_LOWER_LIMIT_OFFSET;
    x_motor_home_loc = base_ptr + X_MOTOR_HOME_LOC_OFFSET;
    y_motor_home_loc = base_ptr + Y_MOTOR_HOME_LOC_OFFSET;
    z_motor_home_loc = base_ptr + Z_MOTOR_HOME_LOC_OFFSET;
    x_motor_home_speed = base_ptr + X_MOTOR_HOME_SPEED_OFFSET;
    y_motor_home_speed = base_ptr + Y_MOTOR_HOME_SPEED_OFFSET;
    z_motor_home_speed = base_ptr + Z_MOTOR_HOME_SPEED_OFFSET;
    x_motor_status = base_ptr + X_MOTOR_STATUS_OFFSET;
    y_motor_status = base_ptr + Y_MOTOR_STATUS_OFFSET;
    z_motor_status = base_ptr + Z_MOTOR_STATUS_OFFSET;
    clk_f_div2 = base_ptr + CLK_F_DIV2_OFFSET;
    joystick = base_ptr + JOYSTICK_OFFSET;
    dispense = base_ptr + DISPENSE_OFFSET;
    outputs = base_ptr + OUTPUTS_OFFSET;
    inputs = base_ptr + INPUTS_OFFSET;
    teach_hps2fpga = base_ptr + TEACH_HPS2FPGA_OFFSET;
    teach_fpga2hps = base_ptr + TEACH_FPGA2HPS_OFFSET;
    printf("Memory mapped at address %p\n", base_ptr);
    
    float x_motor_max_speed_f = 50000;
    float y_motor_max_speed_f = 50000;
    float z_motor_max_speed_f = 30000;

    float x_motor_min_speed_f = 5000;
    float y_motor_min_speed_f = 5000;
    float z_motor_min_speed_f = 2000;

    float x_motor_accelstep_f = 5000;
    float y_motor_accelstep_f = 5000;
    float z_motor_accelstep_f = 10000;

    // Set up motor parameters //
    *(uint32_t *)x_motor_ctrl = 0x0;
    *(uint32_t *)y_motor_ctrl = 0x0 | POLARITY; // Y motor is inverted CR[2] = polarity bit
    *(uint32_t *)z_motor_ctrl = 0x0;

    *(uint32_t *)x_motor_max_speed = (int)x_motor_max_speed_f; 
    *(uint32_t *)y_motor_max_speed = (int)y_motor_max_speed_f; 
    *(uint32_t *)z_motor_max_speed = (int)z_motor_max_speed_f; 

    *(uint32_t *)x_motor_min_speed = (int)x_motor_min_speed_f;
    *(uint32_t *)y_motor_min_speed = (int)y_motor_min_speed_f;
    *(uint32_t *)z_motor_min_speed = (int)z_motor_min_speed_f;

    *(uint32_t *)x_motor_accelstep = (int)x_motor_accelstep_f;
    *(uint32_t *)y_motor_accelstep = (int)y_motor_accelstep_f;
    *(uint32_t *)z_motor_accelstep = (int)z_motor_accelstep_f;


    *(uint32_t *)x_motor_stepincrement = ceil((x_motor_max_speed_f - x_motor_min_speed_f) / x_motor_accelstep_f); // ceil so increment non-zero
    *(uint32_t *)y_motor_stepincrement = ceil((y_motor_max_speed_f - y_motor_min_speed_f) / y_motor_accelstep_f);
    *(uint32_t *)z_motor_stepincrement = ceil((z_motor_max_speed_f - z_motor_min_speed_f) / z_motor_accelstep_f);

    *(uint32_t *)x_motor_home_speed = 10000;
    *(uint32_t *)y_motor_home_speed = 10000;
    *(uint32_t *)z_motor_home_speed = 5000;

    *(uint32_t *)x_motor_upper_limit = 300000;
    *(uint32_t *)y_motor_upper_limit = 300000;
    *(uint32_t *)z_motor_upper_limit = 100000;

    *(uint32_t *)x_motor_lower_limit = 0;
    *(uint32_t *)y_motor_lower_limit = 0;
    *(uint32_t *)z_motor_lower_limit = 0;

    *(uint32_t *)x_motor_home_loc = 0;
    *(uint32_t *)y_motor_home_loc = 300000;
    *(uint32_t *)z_motor_home_loc = 0;

    *(uint32_t *)clk_f_div2 = 25000000; // 50MHz / 2 = 25MHz

    *(uint32_t *)x_motor_target = *(uint32_t *)x_motor_home_loc;
    *(uint32_t *)y_motor_target = *(uint32_t *)y_motor_home_loc;
    *(uint32_t *)z_motor_target = *(uint32_t *)x_motor_home_loc;

    *(uint32_t *)x_motor_ctrl = SOFT_RESET;
    *(uint32_t *)y_motor_ctrl = SOFT_RESET;
    *(uint32_t *)z_motor_ctrl = SOFT_RESET;
    cout << "End of Constructor" << endl;
}


void FisnarControl::MoveXY(uint32_t x, uint32_t y)
{
    *(uint32_t *)x_motor_target = x;
    *(uint32_t *)y_motor_target = y;
    *(uint32_t *)x_motor_ctrl = ENABLE;
    *(uint32_t *)y_motor_ctrl = ENABLE | POLARITY;
}

void FisnarControl::MoveXYZ(uint32_t x, uint32_t y, uint32_t z)
{
    *(uint32_t *)x_motor_target = x;
    *(uint32_t *)y_motor_target = y;
    *(uint32_t *)y_motor_target = z;
    *(uint32_t *)x_motor_ctrl = ENABLE;
    *(uint32_t *)y_motor_ctrl = ENABLE | POLARITY;
    *(uint32_t *)z_motor_ctrl = ENABLE;
}

void FisnarControl::MoveZ(uint32_t z)
{
    *(uint32_t *)z_motor_target = z;
    *(uint32_t *)z_motor_ctrl = ENABLE;
}

void FisnarControl::DisplaceX(int s)
{
    int target = *(uint32_t *)x_motor_location + s;
    if(target < *(uint32_t *)x_motor_lower_limit)
        target = *(uint32_t *)x_motor_lower_limit;
    if(target > *(uint32_t *)x_motor_upper_limit)
        target = *(uint32_t *)x_motor_upper_limit;
    
    *(uint32_t *)x_motor_target = (uint32_t)target;
}

void FisnarControl::DisplaceY(int s)
{
    int target = *(uint32_t *)y_motor_location + s;
    if(target < *(uint32_t *)y_motor_lower_limit)
        target = *(uint32_t *)y_motor_lower_limit;
    if(target > *(uint32_t *)y_motor_upper_limit)
        target = *(uint32_t *)y_motor_upper_limit;
    
    *(uint32_t *)y_motor_target = (uint32_t)target;
}

void FisnarControl::DisplaceZ(int s)
{
    int target = *(uint32_t *)z_motor_location + s;
    if(target < *(uint32_t *)z_motor_lower_limit)
        target = *(uint32_t *)z_motor_lower_limit;
    if(target > *(uint32_t *)z_motor_upper_limit)
        target = *(uint32_t *)z_motor_upper_limit;
    
    *(uint32_t *)z_motor_target = (uint32_t)target;
}

void FisnarControl::DisplaceXY(int s_x, int s_y)
{
    int target_x = *(uint32_t *)x_motor_location + s_x;
    if(target_x < *(uint32_t *)x_motor_lower_limit)
        target_x = *(uint32_t *)x_motor_lower_limit;
    if(target_x > *(uint32_t *)x_motor_upper_limit)
        target_x = *(uint32_t *)x_motor_upper_limit;
    
    *(uint32_t *)x_motor_target = (uint32_t)target_x;

    int target_y = *(uint32_t *)y_motor_location + s_y;
    if(target_y < *(uint32_t *)y_motor_lower_limit)
        target_y = *(uint32_t *)y_motor_lower_limit;
    if(target_y > *(uint32_t *)y_motor_upper_limit)
        target_y = *(uint32_t *)y_motor_upper_limit;
    
    *(uint32_t *)y_motor_target = (uint32_t)target_y;
}

void FisnarControl::HomeMotors()
{
    *(uint32_t *)x_motor_ctrl = ENABLE | HOME;
    *(uint32_t *)y_motor_ctrl = ENABLE | HOME | POLARITY;
    *(uint32_t *)z_motor_ctrl = ENABLE | HOME;
    // Wait for motors to home
    while(!((*(uint32_t *)x_motor_status == STATUS_INPOSITION | *(uint32_t *)x_motor_status == STATUS_HOMED) && (*(uint32_t *)y_motor_status == STATUS_INPOSITION | *(uint32_t *)y_motor_status == STATUS_HOMED) && (*(uint32_t *)z_motor_status == STATUS_INPOSITION | *(uint32_t *)z_motor_status == STATUS_HOMED)));
    // Set motor target to home location, so they don't move when enabled !!!!!!!! VERY IMPORTANT !!!!!!!!!
    *(uint32_t *)x_motor_target = *(uint32_t *)x_motor_home_loc;
    *(uint32_t *)y_motor_target = *(uint32_t *)y_motor_home_loc;
    *(uint32_t *)z_motor_target = *(uint32_t *)z_motor_home_loc;
    // Remove the home flag
    *(uint32_t *)x_motor_ctrl = ENABLE;
    *(uint32_t *)y_motor_ctrl = ENABLE| POLARITY;
    *(uint32_t *)z_motor_ctrl = ENABLE;
}


void FisnarControl::EnableMotors()
{
    *(uint32_t *)x_motor_ctrl = ENABLE ;
    *(uint32_t *)y_motor_ctrl = ENABLE | POLARITY;
    *(uint32_t *)z_motor_ctrl = ENABLE ;
}

void FisnarControl::DisableMotors()
{
    *(uint32_t *)x_motor_ctrl = 0 ;
    *(uint32_t *)y_motor_ctrl = POLARITY;
    *(uint32_t *)z_motor_ctrl = 0 ;
}

void FisnarControl::MotorWindings(bool AWoff)
{
    if (AWoff)
    {
        *(uint32_t *)x_motor_ctrl = *(uint32_t *)x_motor_ctrl | (WINDING_OFF);
        *(uint32_t *)y_motor_ctrl = *(uint32_t *)y_motor_ctrl | (WINDING_OFF);
        *(uint32_t *)z_motor_ctrl = *(uint32_t *)z_motor_ctrl | (WINDING_OFF);
    }
    else
    {
        *(uint32_t *)x_motor_ctrl = *(uint32_t *)x_motor_ctrl & ~(WINDING_OFF);
        *(uint32_t *)y_motor_ctrl = *(uint32_t *)y_motor_ctrl & ~(WINDING_OFF);
        *(uint32_t *)z_motor_ctrl = *(uint32_t *)z_motor_ctrl & ~(WINDING_OFF);
    }
}

void FisnarControl::MotorWindings(char motor, bool AWoff)
{
    if (motor == 'x' || 'X')
    {
        if (AWoff)
            *(uint32_t *)x_motor_ctrl = *(uint32_t *)x_motor_ctrl | (WINDING_OFF);
        else
            *(uint32_t *)x_motor_ctrl = *(uint32_t *)x_motor_ctrl & ~(WINDING_OFF);
    }
    else if (motor == 'y' || 'Y')
    {
        if (AWoff)
            *(uint32_t *)y_motor_ctrl = *(uint32_t *)y_motor_ctrl | (WINDING_OFF);
        else
            *(uint32_t *)y_motor_ctrl = *(uint32_t *)y_motor_ctrl & ~(WINDING_OFF);
    }
    else if (motor == 'z' || 'Z')
    {
        if (AWoff)
            *(uint32_t *)z_motor_ctrl = *(uint32_t *)z_motor_ctrl | (WINDING_OFF);
        else
            *(uint32_t *)z_motor_ctrl = *(uint32_t *)z_motor_ctrl & ~(WINDING_OFF);
    }
}

void FisnarControl::Dispense(bool dispense)
{
    if (dispense)
    {
        *(uint32_t *)outputs = *(uint32_t *)outputs | OUTPUT_DISPENSE;
    }
    else
    {
        *(uint32_t *)x_motor_ctrl = *(uint32_t *)outputs & ~OUTPUT_DISPENSE;
    }
}

void FisnarControl::DispenseTimed(uint32_t dispenseTime)
{
    *(uint32_t *)dispense = dispenseTime | 1<<31;
    *(uint32_t *)dispense = dispenseTime & ~(1<<31);
}


uint32_t FisnarControl::ReadXYZ(char motor)
{
    if (motor == 'x' || 'X')
        return *(uint32_t *)x_motor_location;
    else if (motor == 'y' || 'Y')
        return *(uint32_t *)y_motor_location;
    else if (motor == 'z' || 'Z')
        return *(uint32_t *)z_motor_location;
    else
        return 999999;
}

uint32_t FisnarControl::LocationX()
    { return *(uint32_t *)x_motor_location; }

uint32_t FisnarControl::LocationY()
    { return *(uint32_t *)y_motor_location; }

uint32_t FisnarControl::LocationZ()
     { return *(uint32_t *)z_motor_location; }



bool FisnarControl::ReadLS(char axis)
{
    if(axis == 'x' || 'X')
        return (*(uint32_t *)inputs & INPUT_LIMITSWITCH_X) && 1;
    else if (axis == 'y' || 'Y')
        return (*(uint32_t *)inputs & INPUT_LIMITSWITCH_Y) && 1;
    else if (axis == 'z' || 'Z')
        return (*(uint32_t *)inputs & INPUT_LIMITSWITCH_Z) && 1;
}

uint16_t FisnarControl::ReadJoystick(char axis)
{
    if (axis == 'x' || axis == 'X')
        return *(uint32_t *)joystick & 0x0000FFFF;
    else if (axis == 'y' || axis == 'Y')
        return (*(uint32_t *)joystick & 0xFFFF0000) >> 16;
}

uint16_t FisnarControl::ReadJoystickX()
{
        return *(uint32_t *)joystick & 0x0000FFFF;
}

uint16_t FisnarControl::ReadJoystickY()
{
        return (*(uint32_t *)joystick & 0xFFFF0000) >> 16;
}

uint32_t FisnarControl::MotorStatus(char motor)
{
    volatile uint32_t status;
    if (motor == 'x')
        status =  *(uint32_t *)x_motor_status;
    else if (motor == 'y')
        status =  *(uint32_t *)y_motor_status;
    else if (motor == 'z')
        status =  *(uint32_t *)z_motor_status;
    else
        status = -1;
    return status;
}

int FisnarControl::ReadWheel()
{
    uint32_t BCD1 = ((~(*(uint32_t *)inputs) & 0x003C0000) >> 18);
    uint32_t BCD10 = ((~(*(uint32_t *)inputs) & 0x03C00000) >> 22);
    return BCD1 + (BCD10 * 10);
}

bool FisnarControl::ReadSwitch()
{
    return *(uint32_t *)inputs & INPUT_PANEL_SWITCH;
}

bool FisnarControl::ReadStartButton()
{
    return *(uint32_t *)inputs & INPUT_PANEL_START;
}

void FisnarControl::WriteStartLED(bool LED)
{
    if (LED)
    {
        *(uint32_t *)outputs = *(uint32_t *)outputs | OUTPUT_PANEL_START;
    }
    else
    {
        *(uint32_t *)outputs = *(uint32_t *)outputs & ~OUTPUT_PANEL_START;
    }
}

void FisnarControl::DirectionTeach(uint8_t direction)
{
    *(uint32_t *)teach_hps2fpga = (uint32_t)direction<<7;
}

uint8_t FisnarControl::ReadTeach()
{
    return (char)*(uint32_t *)teach_fpga2hps & 0x0000007F;
}

void FisnarControl::WriteTeach(uint8_t teach)
{
    *(uint32_t *)teach_hps2fpga = (uint32_t)teach;
}


// void FisnarControl::WriteOpenCollector(char output_val)
// {
//     *(uint32_t *)outputs = *(uint32_t *)outputs | output_val;
//     *(uint32_t *)outputs = *(uint32_t *)outputs & output_val;
// }

// void FisnarControl::WriteBreakout(int output_val)
// {
//     *(uint32_t *)outputs = (uint32_t)output_val;
// }


void FisnarControl::ChangeMotorSpeed(char motor, float max_speed, float min_speed, float step_distance)
{
    if (motor == 'x' || 'X')
    {
        *(uint32_t *)x_motor_max_speed = (int)max_speed;
        *(uint32_t *)x_motor_min_speed = (int)min_speed;
        *(uint32_t *)x_motor_accelstep = (int)step_distance;
        *(uint32_t *)x_motor_stepincrement = ceil((max_speed - min_speed) / step_distance);

    }
    else if (motor == 'y' || 'Y')
    {
        *(uint32_t *)y_motor_max_speed = (int)max_speed;
        *(uint32_t *)y_motor_min_speed = (int)min_speed;
        *(uint32_t *)y_motor_accelstep = (int)step_distance;
        *(uint32_t *)y_motor_stepincrement = ceil((max_speed - min_speed) / step_distance);   
    }
    else if (motor == 'z' || 'Z')
    {
        *(uint32_t *)z_motor_max_speed = (int)max_speed;
        *(uint32_t *)z_motor_min_speed = (int)min_speed;
        *(uint32_t *)z_motor_accelstep = (int)step_distance;
        *(uint32_t *)z_motor_stepincrement = ceil((max_speed - min_speed) / step_distance);
    }
}

void FisnarControl::ChangeMotorLimits(char motor,uint32_t upper_limit, uint32_t lower_limit)
{
    if (motor == 'x' || 'X')
    {
        *(uint32_t *)x_motor_upper_limit = upper_limit;
        *(uint32_t *)x_motor_lower_limit = lower_limit;
    }
    else if (motor == 'y' || 'Y')
    {
        *(uint32_t *)y_motor_upper_limit = upper_limit;
        *(uint32_t *)y_motor_lower_limit = lower_limit;
    }
    else if (motor == 'z' || 'Z')
    {
        *(uint32_t *)z_motor_upper_limit = upper_limit;
        *(uint32_t *)z_motor_lower_limit = lower_limit;
    }
}

void FisnarControl::ChangeHomeLocation(char motor, uint32_t home_location)
{
    if (motor == 'x' || 'X')
    {
        *(uint32_t *)x_motor_home_loc = home_location;
    }
    else if (motor == 'y' || 'Y')
    {
        *(uint32_t *)y_motor_home_loc = home_location;
    }
    else if (motor == 'z' || 'Z')
    {
        *(uint32_t *)z_motor_home_loc = home_location;
    }
}


FisnarControl::~FisnarControl()
{
    munmap(base_ptr, PIO_SIZE);
	close(fd);
    cout << "Ran Deconstructor" << endl;
}