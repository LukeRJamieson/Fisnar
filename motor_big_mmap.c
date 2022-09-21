#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>

#define PIO_SIZE 0x30000

#define PIO_BASE 0xFF200000

#define LED_OFFSET 0x00003000
#define SWITCH_OFFSET 0x00004000

#define X_MOTOR_TARGET_OFFSET 0x00006000
#define Y_MOTOR_TARGET_OFFSET 0x00007000
#define Z_MOTOR_TARGET_OFFSET 0x00008000

#define X_MOTOR_LOCATION_OFFSET 0x00009000
#define Y_MOTOR_LOCATION_OFFSET 0x0000A000
#define Z_MOTOR_LOCATION_OFFSET 0x0000B000

#define X_MOTOR_CTRL_OFFSET 0x0000C000
#define Y_MOTOR_CTRL_OFFSET 0x0000D000
#define Z_MOTOR_CTRL_OFFSET 0x0000E000

#define X_MOTOR_MAX_SPEED_OFFSET 0x0000F000
#define Y_MOTOR_MAX_SPEED_OFFSET 0x00010000
#define Z_MOTOR_MAX_SPEED_OFFSET 0x00011000

#define X_MOTOR_MIN_SPEED_OFFSET 0x00012000
#define Y_MOTOR_MIN_SPEED_OFFSET 0x00013000
#define Z_MOTOR_MIN_SPEED_OFFSET 0x00014000

#define X_MOTOR_ACCELSTEP_OFFSET 0x00015000
#define Y_MOTOR_ACCELSTEP_OFFSET 0x00016000
#define Z_MOTOR_ACCELSTEP_OFFSET 0x00017000

#define X_MOTOR_STEPINCREMENT_OFFSET 0x00018000
#define Y_MOTOR_STEPINCREMENT_OFFSET 0x00019000
#define Z_MOTOR_STEPINCREMENT_OFFSET 0x0001A000

#define X_MOTOR_UPPER_LIMIT_OFFSET 0x0001B000
#define Y_MOTOR_UPPER_LIMIT_OFFSET 0x0001C000
#define Z_MOTOR_UPPER_LIMIT_OFFSET 0x0001D000

#define X_MOTOR_LOWER_LIMIT_OFFSET 0x0001E000
#define Y_MOTOR_LOWER_LIMIT_OFFSET 0x0001F000
#define Z_MOTOR_LOWER_LIMIT_OFFSET 0x00020000

#define X_MOTOR_HOME_LOC_OFFSET 0x00021000
#define Y_MOTOR_HOME_LOC_OFFSET 0x00022000
#define Z_MOTOR_HOME_LOC_OFFSET 0x00023000

#define X_MOTOR_HOME_SPEED_OFFSET 0x00024000
#define Y_MOTOR_HOME_SPEED_OFFSET 0x00025000
#define Z_MOTOR_HOME_SPEED_OFFSET 0x00026000

#define X_MOTOR_STATUS_OFFSET 0x00027000
#define Y_MOTOR_STATUS_OFFSET 0x00028000
#define Z_MOTOR_STATUS_OFFSET 0x00029000

#define CLK_F_DIV2_OFFSET 0x0002A000

#define HOME 0x01
#define ENABLE 0x02
#define POLARITY 0x04
#define SOFT_RESET 0x80000000


int main()
{
    printf("Motor Control Program\n29 Aug 2022 - 7:56pm\n");
    unsigned char running = 1;
    int fd;
    printf("Opening file\n");
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1)
    {
        printf("Error opening /dev/mem\n");
        return -1;
    }
    printf("Opened /dev/mem\n");
    printf("%d\n", fd);

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
    printf("Memory mapped at address %p\n", base_ptr);


    
    float x_motor_max_speed_f = 50000;
    float y_motor_max_speed_f = 50000;
    float z_motor_max_speed_f = 30000;

    float x_motor_min_speed_f = 2000;
    float y_motor_min_speed_f = 2000;
    float z_motor_min_speed_f = 2000;

    float x_motor_accelstep_f = 10000;
    float y_motor_accelstep_f = 10000;
    float z_motor_accelstep_f = 10000;


    // Set up motor parameters
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

    *(uint32_t *)x_motor_home_speed = 5000;
    *(uint32_t *)y_motor_home_speed = 5000;
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
    usleep(100000);

    printf("Home the motors? Y/n\n");
    char c = getchar();
    if (c == 'y')
    {
        *(uint32_t *)x_motor_ctrl = HOME | ENABLE;
        *(uint32_t *)y_motor_ctrl = HOME | ENABLE | POLARITY;
        *(uint32_t *)z_motor_ctrl = HOME | ENABLE;
        printf("Homing...\n");
        usleep(100000);
        printf("%d\n",*(uint32_t *)x_motor_status);
        while(*(uint32_t *)x_motor_status != 0xF || *(uint32_t *)y_motor_status != 0xF || *(uint32_t *)z_motor_status != 0xF)
        {   
            printf(".\n");
            usleep(250000);
            printf("\n%d : ",*(uint32_t *)x_motor_status);
            printf("%d : ",*(uint32_t *)y_motor_status);
            printf("%d\n",*(uint32_t *)z_motor_status);
        }
        printf("Homed \n");
        *(uint32_t *)x_motor_target = *(uint32_t *)x_motor_home_loc;
        *(uint32_t *)y_motor_target = *(uint32_t *)y_motor_home_loc;
        *(uint32_t *)z_motor_target = *(uint32_t *)z_motor_home_loc;

        *(uint32_t *)x_motor_ctrl = ENABLE;
        *(uint32_t *)y_motor_ctrl = ENABLE | POLARITY;
        *(uint32_t *)z_motor_ctrl = ENABLE;
    }
    else
    {
        *(uint32_t *)x_motor_ctrl = 0x0;
        *(uint32_t *)y_motor_ctrl = 0x0 | POLARITY;
        *(uint32_t *)z_motor_ctrl = 0x0;
    }
    while(getchar() != '\n');

//    char c;
    int input;

    *(uint32_t *)x_motor_ctrl = ENABLE;
    *(uint32_t *)y_motor_ctrl = ENABLE | POLARITY;
    *(uint32_t *)z_motor_ctrl = ENABLE;
    usleep(100000);
    printf("X SR:%04x\n", *(uint32_t *)x_motor_status);
    printf("Y SR:%04x\n", *(uint32_t *)y_motor_status);
    printf("Z SR:%04x\n", *(uint32_t *)z_motor_status);

//    printf("Press Any Key...\n");
    while(running)
    {
        while(getchar() != '\n');
        printf("Enter [X], [Y], [Z], [B] for XY, or [Q] to quit: ");
        scanf("%c", &c);
        printf("\n");
        if(c == 'x' || c == 'X' || c == 'y' || c == 'Y' || c == 'z' || c == 'Z' || c == 'b' || c == 'B')
        {
            if (c == 'x' || c == 'X')
            {
                //while(getchar() != '\n');
                printf("Enter target X location: ");
                scanf("%d", &input);
                printf("Target is %d\n", input);
                if(input > *(uint32_t *)x_motor_upper_limit || input < *(uint32_t *)x_motor_lower_limit)
                {
                    printf("Target out of range\n");
                    input = *(uint32_t *)x_motor_target;
                    continue;
                }
                *(uint32_t *)x_motor_target = input;
                *(uint32_t *)x_motor_ctrl = ENABLE;
            }
            else if (c == 'y' || c == 'Y')
            {
                //while(getchar() != '\n');
                printf("Enter target Y location: ");
                scanf("%d", &input);
                if(input > *(uint32_t *)y_motor_upper_limit || input < *(uint32_t *)y_motor_lower_limit)
                {
                    printf("Target out of range\n");
                    input = *(uint32_t *)y_motor_target;
                    continue;
                }
                printf("Target is %d\n", input);
                *(uint32_t *)y_motor_target = input;
                *(uint32_t *)y_motor_ctrl = ENABLE | POLARITY;
            }
            else if (c == 'z' || c == 'Z')
            {
                //while(getchar() != '\n');
                printf("Enter target Z location: ");
                scanf("%d", &input);
                printf("Target is %d\n", input);
                if(input > *(uint32_t *)z_motor_upper_limit || input < *(uint32_t *)z_motor_lower_limit)
                {
                    printf("Target out of range\n");
                    input = *(uint32_t *)z_motor_target;
                    continue;
                }
                *(uint32_t *)z_motor_target = input;
                *(uint32_t *)z_motor_ctrl = ENABLE;
            }
            else if(c == 'b' || c =='B')
            {
                *(uint32_t *)x_motor_ctrl = 0;
                *(uint32_t *)y_motor_ctrl = 0;

                printf("Enter target X location: ");
                scanf("%d", &input);
                if(input > *(uint32_t *)x_motor_upper_limit || input < *(uint32_t *)x_motor_lower_limit)
                {
                    printf("Target out of range\n");
                    input = *(uint32_t *)x_motor_target;
                    continue;
                }
                *(uint32_t *)x_motor_target = input;
                while(getchar() != '\n');
                printf("Enter target Y location: ");
                scanf("%d", &input);
                if(input > *(uint32_t *)y_motor_upper_limit || input < *(uint32_t *)y_motor_lower_limit)
                {
                    printf("Target out of range\n");
                    input = *(uint32_t *)y_motor_target;
                    continue;
                }
                *(uint32_t *)y_motor_target = input;
                *(uint32_t *)x_motor_ctrl = ENABLE;
                *(uint32_t *)y_motor_ctrl = ENABLE | POLARITY;
            }
            printf("\n");
            usleep(100000);
            while(*(uint32_t *)x_motor_status != 0xF || *(uint32_t *)y_motor_status != 0xF || *(uint32_t *)z_motor_status != 0xF)
                {
                    printf(".\n");
                    usleep(500000);
                }
            printf("\n");
            printf("X SR:%04x\n", *(uint32_t *)x_motor_status);
            printf("Y SR:%04x\n", *(uint32_t *)y_motor_status);
            printf("Z SR:%04x\n", *(uint32_t *)z_motor_status);
            printf("\n");
            printf("X: %d\n", *(uint32_t *)x_motor_location);
            printf("Y: %d\n", *(uint32_t *)y_motor_location);
            printf("Z: %d\n", *(uint32_t *)z_motor_location);
            printf("\n");
        }
        else if(c == 'q' || c == 'Q')
        {
            //while(getchar() != '\n');
            printf("Quitting...\n");
            running = 0;
        }
        else if(c == 's' || c == 'S')
        {
            printf("\n");
            printf("X SR:%04x\n", *(uint32_t *)x_motor_status);
            printf("Y SR:%04x\n", *(uint32_t *)y_motor_status);
            printf("Z SR:%04x\n", *(uint32_t *)z_motor_status);

            printf("X: %d\n", *(uint32_t *)x_motor_location);
            printf("Y: %d\n", *(uint32_t *)y_motor_location);
            printf("Z: %d\n", *(uint32_t *)z_motor_location);
        }
        else
        {
            printf("Invalid input\n");
        }
        //while(getchar() != '\n');
    }

    munmap(base_ptr, PIO_SIZE);



    close(fd);
    printf("Done\n");
    return 0;
}
