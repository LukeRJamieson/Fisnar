#include "FisnarControl.h"
#include <iostream>
#include <fstream>

using namespace std;

// Struct for Position and Rotation Correction
struct coord {
    int x;
    int y;
};

// Function for Position and Rotation Correction
struct coord FindLocation(struct coord real_coord_1, struct coord file_coord_1, struct coord file_coord_2, double *theta)
{
    file_coord_2.x -= file_coord_1.x;
    file_coord_2.y -= file_coord_1.y;

    int x = file_coord_2.x;
    int y = file_coord_2.y;
    
    file_coord_2.x = x*cos(*theta) - y*sin(*theta);
    file_coord_2.y = x*sin(*theta) + y*cos(*theta);
    
    file_coord_2.x = file_coord_2.x + real_coord_1.x;
    file_coord_2.y = file_coord_2.y + real_coord_1.y;

    return file_coord_2;
}

// Struct for Gerber Coordinates
struct coordinate {
	int32_t X;
	int32_t Y;
	int32_t P;
};

int main () {

	double stepsize = 50000;
	double direction = 3;
	FisnarControl fc;
	fc.Dispense(false);
	coordinate co[500] = {};
	ifstream infile;
	infile.open("/home/root/workspace/FileDestination/Gerber_Interpreted.txt", ios::in);
	//infile.open("/home/root/uploads/Gerber_Interpreted.txt", ios::in);
	if(!infile) {
		cout << "Error opening file" << endl;
		return 1;
	}
	char buffer[80] = {};
	int XFlag = 0;
	int YFlag = 0;
	int PFlag = 0;
	int index = 0;
	string num_string = "";
	while (!infile.eof())
	{
		infile.getline(buffer, sizeof(buffer));
		for (int i = 0; buffer[i] != '\0'; i++) // iterate through each character in the current line
		{
			if (buffer[i] == 'X')
			{
				XFlag = 1;
				YFlag = 0;
				PFlag = 0;
				num_string = "";
			}
			else if (buffer[i] == 'Y')
			{
				XFlag = 0;
				YFlag = 1;
				PFlag = 0;
				num_string = "";
			}
			else if (buffer[i] == 'P')
			{
				XFlag = 0;
				YFlag = 0;
				PFlag = 1;
				num_string = "";
			}
			else if (buffer[i] == ' ')
			{
				if (XFlag)
				{
					co[index].X = atoi(num_string.c_str());
				}
				else if (YFlag)
				{
					co[index].Y = atoi(num_string.c_str());
				}
				else if (PFlag)
				{
					co[index].P = atoi(num_string.c_str());
				}
			}
			else if ((buffer[i] >= '0' && buffer[i] <= '9') || (buffer[i] == '-') || (buffer[i] == '.'))
			{
				//cout << "Handle numerical character\n";
				num_string = num_string + buffer[i]; // Add current character to the end of the number string if current character is numerical
			}
		}
		cout << "X: " << co[index].X << " Y: " << co[index].Y << " P: " << co[index].P << endl;
		index++;
	}
	index = index - 2;
	system("rm /home/root/workspace/FileDestination/Gerber_Interpreted.txt");
	cout << co[index].X << " " << co[index].Y << " " << co[index].P << endl;

	cout << "Thumb Wheel = " << fc.ReadWheel() << endl; 


	usleep(1000000);
	cout << fc.MotorStatus('x') << endl;
	usleep(1000000);
	fc.HomeMotors();
	cout << fc.MotorStatus('x') << endl;
	cout << "Homing" << endl;

	cout << "Homed" << endl;
	cout << "X = " << fc.LocationX() << endl;
	cout << "Y = " << fc.LocationY() << endl;
	cout << "Z = " << fc.LocationZ() << endl;
	usleep(1000000);
	fc.EnableMotors(); // Enable the motors (remove Home signal)
	fc.DirectionTeach(0x00); // Set all teach pendant IO to inputs




// Point Registration // These are the physical points the Fisnar captured when probing the PCB
uint32_t real_X, real_Y, real_Z, real_Angle;

bool point_registered = false; //Boolean to exist while loop for joystick control. Value is set when button is pressed.

// Variable to read ADC value
uint32_t joystick_reading_X;
uint32_t joystick_reading_Y;
uint32_t joystick_reading_Z;

int joystick_target_X = fc.LocationX();
int joystick_target_Y = fc.LocationY();
int joystick_target_Z = fc.LocationZ();


int teach = 0;
while(!point_registered)
{
///*********************************************************//	
//     START: Joystick Control for Finding Coordinate [0]   //
//**********************************************************//
	teach = fc.ReadTeach();


	
	if((teach & 0x02) == 0x02) // XY Movement
	{
		joystick_reading_X = fc.ReadJoystickX();
		joystick_reading_Y = fc.ReadJoystickY();

		//----------------------- X -----------------------//
		if(joystick_reading_X < 300 && (fc.MotorStatus('x') == STATUS_INPOSITION || fc.MotorStatus('x') == STATUS_COUNTING_DOWN)) // 0-750
			joystick_target_X = fc.LocationX() - 100000; // Fast
		else if(joystick_reading_X < 1500 && (fc.MotorStatus('x') == STATUS_INPOSITION || fc.MotorStatus('x') == STATUS_COUNTING_DOWN)) // 750-1500
			joystick_target_X = fc.LocationX() - 200; // Crawl

		else if(joystick_reading_X < 1800) // between 1500 and 1800 dead zone
		{
			if(joystick_target_X > fc.LocationX() + 500)
			{
				joystick_target_X = fc.LocationX() + 500;
			}
			else if(joystick_target_X < fc.LocationX() - 500)
			{
				joystick_target_X = fc.LocationX() - 500;
			}
			else
				joystick_target_X = joystick_target_X;
		}
		else if(joystick_reading_X < 3000 && (fc.MotorStatus('x') == STATUS_INPOSITION || fc.MotorStatus('x') == STATUS_COUNTING_UP)) // 1800-2250
			joystick_target_X = fc.LocationX() + 200;	
		else if (joystick_reading_X >= 3000 && fc.MotorStatus('x') == STATUS_COUNTING_DOWN) 
			joystick_target_X = fc.LocationX() - 1000;
		else if (joystick_reading_X >= 3000 && (fc.MotorStatus('x') == STATUS_INPOSITION || fc.MotorStatus('x') == STATUS_COUNTING_UP))
			joystick_target_X = fc.LocationX() + 100000;

		// Set limits for Joystick Targets
		if(joystick_target_X < 0)
			joystick_target_X = 0;
		else if(joystick_target_X > 300000)
			joystick_target_X = 300000;

		//----------------------- Y -----------------------//
		if(joystick_reading_Y < 300 && (fc.MotorStatus('y') == STATUS_INPOSITION || fc.MotorStatus('y') == STATUS_COUNTING_UP)) // 0-750
			joystick_target_Y = fc.LocationY() + 100000; // Fast
		else if(joystick_reading_Y < 1500 && (fc.MotorStatus('y') == STATUS_INPOSITION || fc.MotorStatus('y') == STATUS_COUNTING_UP)) // 750-1500
			joystick_target_Y = fc.LocationY() + 200; // Crawl

		else if(joystick_reading_Y < 1800) // between 1500 and 1800 dead zone
		{
			if(joystick_target_Y > fc.LocationY() + 500)
			{
				joystick_target_Y = fc.LocationY() + 500;
			}
			else if(joystick_target_Y < fc.LocationY() - 500)
			{
				joystick_target_Y = fc.LocationY() - 500;
			}
			else
				joystick_target_Y = joystick_target_Y;
		}
		else if(joystick_reading_Y < 3000 && (fc.MotorStatus('y') == STATUS_INPOSITION || fc.MotorStatus('y') == STATUS_COUNTING_DOWN)) // 1800-2250
			joystick_target_Y = fc.LocationY() - 200;	
		else if (joystick_reading_Y >= 3000 && fc.MotorStatus('y') == STATUS_COUNTING_UP) 
			joystick_target_Y = fc.LocationY() + 1000;
		else if (joystick_reading_Y >= 3000 && (fc.MotorStatus('y') == STATUS_INPOSITION || fc.MotorStatus('y') == STATUS_COUNTING_DOWN))
			joystick_target_Y = fc.LocationY() - 100000;

		// Set limits for Joystick Targets
		if(joystick_target_Y < 0)
			joystick_target_Y = 0;
		else if(joystick_target_Y > 300000)
			joystick_target_Y = 300000;


		// Write to FPGA Memory //
		fc.MoveXY((uint32_t)joystick_target_X,(uint32_t)joystick_target_Y);

		cout << "X: A = " << joystick_reading_X << " T = " << joystick_target_X << " L = " << fc.LocationX() << endl;
		cout << "Y: A = " << joystick_reading_Y << " T = " << joystick_target_Y << " L = " << fc.LocationY() << endl;

		// Stop Z from moving when XY is moving
		joystick_target_Z = fc.LocationZ();
		fc.MoveZ((uint32_t)joystick_target_Z);
		
		usleep(100000);
	}

	else // Z Movement
	{
		joystick_reading_Z = fc.ReadJoystickY();

		if(joystick_reading_Z < 300 && (fc.MotorStatus('z') == STATUS_INPOSITION || fc.MotorStatus('z') == STATUS_COUNTING_DOWN)) // 0-750
			joystick_target_Z = fc.LocationZ() - 10000; // Fast
		else if(joystick_reading_Z < 1500 && (fc.MotorStatus('z') == STATUS_INPOSITION || fc.MotorStatus('z') == STATUS_COUNTING_DOWN)) // 750-1500
			joystick_target_Z = fc.LocationZ() - 200; // Crawl

		else if(joystick_reading_Z < 1800) // between 1500 and 1800 dead zone
		{
			if(joystick_target_Z > fc.LocationZ() + 100)
			{
				joystick_target_Z = fc.LocationZ() + 100;
			}
			else if(joystick_target_Z < fc.LocationZ() - 100)
			{
				joystick_target_Z = fc.LocationZ() - 100;
			}
			else
				joystick_target_Z = joystick_target_Z;
		}
		else if(joystick_reading_Z < 3000 && (fc.MotorStatus('z') == STATUS_INPOSITION || fc.MotorStatus('z') == STATUS_COUNTING_UP)) // 1800-2250
			joystick_target_Z = fc.LocationZ() + 200;	
		else if (joystick_reading_Z >= 3000 && fc.MotorStatus('z') == STATUS_COUNTING_DOWN) 
			joystick_target_Z = fc.LocationZ() - 1000;
		else if (joystick_reading_Z >= 3000 && (fc.MotorStatus('z') == STATUS_INPOSITION || fc.MotorStatus('z') == STATUS_COUNTING_UP))
			joystick_target_Z = fc.LocationZ() + 10000;

		// Set limits for Joystick Targets
		if(joystick_target_Z < 0)
			joystick_target_Z = 0;
		else if(joystick_target_Z > 80000)
			joystick_target_Z = 80000;

		fc.MoveZ((uint32_t)joystick_target_Z);

		cout << "Z: A = " << joystick_reading_Z << " T = " << joystick_target_Z << " L = " << fc.LocationZ() << endl;
		usleep(100000);
	}
// ************************************ End of Joystick Control *************************************** //

	if ((teach & 0x01) != 0x01) // <-------------------------------------- Add webserver 'found point' condition here to exist the while loop and save the location.
	{
		point_registered = true;
		real_X = fc.LocationX();
		real_Y = fc.LocationY();
		real_Z = fc.LocationZ();
		cout << "(" << real_X << "," << real_Y << "," << real_Z << ")" << endl; 
	}
}
point_registered = 0;
cout << "Point Registered - Stop Pressing Button." << endl;
///*******************************************//	
// 	Point [0] Found if while loop exits here  //
//********************************************//

usleep(1000000); // <-------------------------------------- I have a 1 second delay here to make sure the button is released before the next point is found.

int Z_travel = real_Z - 20000;
if (Z_travel < 0)
	Z_travel = 0;

fc.MoveZ((uint32_t)Z_travel); // Move to safe height

// Teach 1 to rotate or Z (bit 0)
// Teach 2 to register rotation (bit 1)
struct coord real_coord_1, real_coord_2 ,file_coord_1, file_coord_2, job_coord;

int relativeX2, relativeY2;
int real_X2, real_Y2;
double theta = 0.0;

int translationX = real_X - co[0].X; // X translation
int translationY = real_Y - co[0].Y; // Y translation


cout << co[0].X << " " << co[0].Y << endl;
cout << real_X << " " << real_Y << endl;


//fc.MoveZ((uint32_t)real_Z - 1000); // Move to Z - 1mm
usleep(1000);
while(fc.MotorStatus('x') != STATUS_INPOSITION || fc.MotorStatus('y') != STATUS_INPOSITION || fc.MotorStatus('z') != STATUS_INPOSITION);

///*******************************************************//	
// 	Find Point [last] with Joystick by finding rotation   //
//********************************************************//

real_coord_1 = {(int)real_X, (int)real_Y};
file_coord_1 = {co[0].X, co[0].Y};
file_coord_2 = {co[index].X, co[index].Y};

while(!point_registered)
{
	teach = fc.ReadTeach();

	real_coord_2 = FindLocation(real_coord_1,file_coord_1,file_coord_2,&theta);

	cout << "Theta: " << theta << endl;
	cout << co[index].X << " " << co[index].Y << endl;
	cout << real_coord_2.x << " " << real_coord_2.y << endl;
	usleep(100000);

	fc.MoveXY((uint32_t)real_coord_2.x,(uint32_t)real_coord_2.y);

	joystick_reading_Y = fc.ReadJoystickY();


	if(joystick_reading_Y < 300) // 0-750
			theta += 0.01; // Fast
		else if(joystick_reading_Y < 1500) // 750-1500
			theta += 0.001; // Crawl
		else if(joystick_reading_Y < 1800) // between 1500 and 1800 dead zone
			joystick_target_Y = joystick_target_Y;
		else if(joystick_reading_Y < 3000) // 1800-2250
			theta -= 0.001;	
		else if (joystick_reading_Y >= 3000)
			theta -= 0.01;

	if ((teach & 0x02) != 0x02) // Button 2 is pressed. Drop Z to real Z + 2mm
	{
		fc.MoveZ((uint32_t)real_Z - 500); // Move to Z - 0.5mm
	}
	else
	{
		fc.MoveZ((uint32_t)real_Z - 20000); // Move to Z - 2cm
	}

	// Button 1 is pressed. Angle Found
	if ((teach & 0x01) != 0x01) // <-------------------------------------- Add webserver 'found point' condition here to exist the while loop and save the angle.
	{
		point_registered = true;
	}
}
cout << "Point Registered" << endl;
cout << "Translation: " << translationX << " " << translationY << endl;
cout << "Theta: " << theta << endl;
///*********************************************************//	
// 	Point [last] (rotation) Found if while loop exits here  //
//**********************************************************//
usleep(1000);
fc.MoveZ((uint32_t)Z_travel);
usleep(1000);
while(fc.MotorStatus('x') != STATUS_INPOSITION || fc.MotorStatus('y') != STATUS_INPOSITION || fc.MotorStatus('z') != STATUS_INPOSITION);

fc.WriteStartLED(true);
while(fc.ReadStartButton()==true);
fc.WriteStartLED(false);

for(int i = 0; i<index+1;i++)
{
	cout << co[i].X << " " << co[i].Y << endl;
}

///********************************************************************************//	
// 	Work through the for loop for length of gerber file, dispensing at each point  //
//*********************************************************************************//

for(int i = 0; i<index+1;i++)
{
	cout << "Point: " << i << endl;
	job_coord = {co[i].X, co[i].Y};
	job_coord = FindLocation(real_coord_1,file_coord_1,job_coord,&theta);
	fc.MoveXY(job_coord.x,job_coord.y);
	cout << "Moving to : " << job_coord.x << " " << job_coord.y << endl;
	usleep(1000); // <---------------------- Short 1ms delay so the motor status is updated before being checked.
	while(fc.MotorStatus('x') != STATUS_INPOSITION || fc.MotorStatus('y') != STATUS_INPOSITION || fc.MotorStatus('z') != STATUS_INPOSITION); // Wait for motors to stop moving
	fc.MoveZ(real_Z);
	usleep(1000);
	while(fc.MotorStatus('x') != STATUS_INPOSITION || fc.MotorStatus('y') != STATUS_INPOSITION || fc.MotorStatus('z') != STATUS_INPOSITION);
	fc.DispenseTimed(10000000);
	usleep(250000);
	fc.MoveZ(real_Z - 3000);
	usleep(1000);
	while(fc.MotorStatus('x') != STATUS_INPOSITION || fc.MotorStatus('y') != STATUS_INPOSITION || fc.MotorStatus('z') != STATUS_INPOSITION);
}
	cout << "Done Loop" << endl;
	fc.MoveZ(0);
	usleep(1000);
	while(fc.MotorStatus('x') != STATUS_INPOSITION || fc.MotorStatus('y') != STATUS_INPOSITION || fc.MotorStatus('z') != STATUS_INPOSITION);
	fc.MoveXY(0,300000);
	usleep(1000);
	while(fc.MotorStatus('x') != STATUS_INPOSITION || fc.MotorStatus('y') != STATUS_INPOSITION || fc.MotorStatus('z') != STATUS_INPOSITION);


	cout << "DONE" << endl;
	cout << "X Status = " << fc.MotorStatus('x') << endl;
	cout << "Y Status = " << fc.MotorStatus('y') << endl;
	cout << "Z Status = " << fc.MotorStatus('z') << endl;
	return 0;
}


