module MotorControl
(
input clk,
input rst, /////////////////////////////////// include a software reset
// Software Control Signals
input [31:0] CR,
input [31:0] Target,
// Hardware Physical Signals
input LS,
input AWOff,

// Acceleration Parameters
input [31:0] max_speed_freq, // This is the counter value for running at maximum speed. eg max speed is 50000 (50mm/s). Using the 50MHz clock this will be (50,000,000/2)/2000 = 500
input [31:0] min_speed_freq, // This is the counter value for running at minimum speed. eg min speed is 2000 (2mm/s). Using the 50MHz clock this will be (50,000,000/2)/2000 = 12500
input [31:0] accel_steps, // This is how many steps the motors should travel between the min and max speed. For this example we let it go from 2mm/s to 50mm/s in 10mm (10000 steps) . 
input [31:0] freq_increment, // (maxspeed-minspeed)/accel_steps (let the HPS work this out)
input [31:0] homing_speed_freq, // Speed at which homing is performed
input [31:0] clk_freq_div_2,

// Movement and Range Parameters
input [31:0] home_location, // after homing / at limit switch, what is the location
input [31:0] lower_limit,
input [31:0] upper_limit,

// Outputs
output CW,
output CCW,
output reg [31:0] Location,
output reg Done, //flag
output reg [3:0] StatusCode,

output [31:0] debug_current_count,
output [31:0] debug_current_freq
);
assign debug_current_count = current_count;
assign debug_current_freq = current_freq;

wire SoftReset = CR[31];
wire Go_Home = CR[0];
wire Enable = CR[1];
wire CW_polarity = CR[2];  // 0: CW = +1, 1: CW = -1 (X and Z = 0; Y = 1)
wire Start_Homing_Sequence = Go_Home & ReadyToGoHome;

// Acceleration Control (Dynamic Clock)
reg Step;
reg [31:0] counter;
reg [31:0] current_count;
reg [31:0] current_freq;
reg DIR_CW;
reg DIR_CCW;
reg Overlimit;
reg ReadyToGoHome;
reg Homed;
reg [2:0]Homing_State;
reg [15:0]Wait_Counter;

// STATUS CODE PARAMETERS //
parameter isReset = 4'b0000;
parameter isHoming1 = 4'b0100;
parameter isHoming2 = 4'b0010;
parameter isHomed = 4'b0110;
parameter targetUpper = 4'b1000;
parameter targetLower = 4'b0001;
parameter countingUp = 4'b1100;
parameter countingDown = 4'b0011;
parameter LocationAtTarget = 4'b1111;
parameter other = 4'b1010;

// HOMING SEQUENCE STATES //
parameter initHome = 3'd0;
parameter leaveLS1 = 3'd1;
parameter wait1 = 3'd2;
parameter goToLS = 3'd3;
parameter wait2 = 3'd4;
parameter leaveLS2 = 3'd5;
parameter wait3 = 3'd6;
parameter finishedHome = 3'd7;

initial // for debug
begin
//	current_count = clk_freq_div_2/min_speed_freq;
	Overlimit = 0;
end


always@(posedge clk) //TODO
begin
	counter <= counter + 32'b1;
	if ((counter >= current_count)) //Enable && 
	begin
		counter <= 0;
		Step <= ~Step;
	end
end

always@(posedge Step or negedge rst or posedge AWOff or posedge SoftReset)
begin

	if(~rst || AWOff || SoftReset)
	begin
			Location <= home_location;
			DIR_CW <= 0;
			DIR_CCW <= 0;
			Done <= 0;
			Overlimit <= 0;
			current_freq <= min_speed_freq;
			StatusCode <= isReset;
			ReadyToGoHome <= 1;
			Homed <= 0;
			Homing_State <= 0;
	end
	
	// HOMING SEQUENCE
	else if(Start_Homing_Sequence && Enable)
	begin
	current_freq <= homing_speed_freq;
	case(Homing_State)
		initHome:
			begin
				Homed <= 0;
				ReadyToGoHome <= 1;
				Wait_Counter <= 16'd5000;
				StatusCode <= isHoming1;
				Location <= home_location;
				if(LS)
					Homing_State <= leaveLS1;
				else
					Homing_State <= goToLS;
			end
			
		leaveLS1:
			begin
				Homed <= 0;
				ReadyToGoHome <= 1;
				DIR_CW <= 1;
				DIR_CCW <= 0;
				Wait_Counter <= 16'd5000;
				StatusCode <= isHoming1;
				Location <= home_location;
				if(!LS)
					Homing_State <= wait1;
				else
					Homing_State <= leaveLS1;
			end
			
		wait1:
			begin
				Homed <= 0;
				ReadyToGoHome <= 1;
				DIR_CW <= 0;
				DIR_CCW <= 0;
				StatusCode <= isHoming2;
				Location <= home_location;
				Wait_Counter <= Wait_Counter - 16'b1;
			if(Wait_Counter == 0)
					Homing_State <= goToLS;
				else
					Homing_State <= wait1;
			end
			
		goToLS:
			begin
				Homed <= 0;
				ReadyToGoHome <= 1;
				DIR_CW <= 0;
				DIR_CCW <= 1;
				Wait_Counter <= 16'd5000;
				StatusCode <= isHoming1;
				Location <= home_location;
				if(LS)
					Homing_State <= wait2;
				else
					Homing_State <= goToLS;
			end
			
		wait2:
			begin
				Homed <= 0;
				ReadyToGoHome <= 1;
				DIR_CW <= 0;
				DIR_CCW <= 0;
				StatusCode <= isHoming2;
				Location <= home_location;
				Wait_Counter <= Wait_Counter - 16'b1;
			if(Wait_Counter == 0)
					Homing_State <= leaveLS2;
				else
					Homing_State <= wait2;
			end
			
		leaveLS2:
			begin
				Homed <= 0;
				ReadyToGoHome <= 1;
				DIR_CW <= 1;
				DIR_CCW <= 0;
				StatusCode <= isHoming1;
				Location <= home_location;
				Wait_Counter <= 16'd200; // Leave the LS by 0.2mm	
				if(!LS)
					Homing_State <= wait3;
				else
					Homing_State <= leaveLS2;
			end
			
		wait3:
			begin
				Homed <= 0;
				ReadyToGoHome <= 1;
				DIR_CW <= 1; // Leave the LS by 0.2mm
				DIR_CCW <= 0;
				StatusCode <= isHoming2;
				Location <= home_location;
				Wait_Counter <= Wait_Counter - 16'b1;
			if(Wait_Counter == 0)
					Homing_State <= finishedHome;
				else
					Homing_State <= wait3;
			end
			
		finishedHome:
			begin
				Homed <= 1;
				ReadyToGoHome <= 0;
				DIR_CW <= 0;
				DIR_CCW <= 0;
				Wait_Counter <= 16'd5000;
				Homing_State <= finishedHome;
				current_freq <= min_speed_freq;
				StatusCode <= isHomed;
				Location <= home_location;
			end
			
		default:
			begin
				Homed <= 0;
				Homing_State <= 0;
				ReadyToGoHome <= 1;
				DIR_CW <= 0;
				DIR_CCW <= 0;
				Wait_Counter <= 16'd5000;
				StatusCode <= isHoming1;
				Location <= home_location;
			end
	endcase
	end

		
///////////////// Limit Check ////////////////////////////////////
	else if(Target > upper_limit)
		begin
			StatusCode <= targetUpper;
			Overlimit <= 1;
		end
	else if(Target < lower_limit)
		begin
			StatusCode <= targetLower;
			Overlimit <= 1;
		end
	
///////////////// At the Target //////////////////////////////////	
	else if(Enable && (Location == Target)  && Homed)
	begin
		Location <= Location;
		DIR_CW <= 0;
		DIR_CCW <= 0;
		Done <= 1;
		current_freq <= min_speed_freq;
		StatusCode <= LocationAtTarget;
	end
	
	
///////////////// Counting up to target//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(!Overlimit && Enable && (Location < Target) && Homed)
	begin
		Location <= Location + 32'b1;
		if(CW_polarity)
		begin
			DIR_CW <= 0;
			DIR_CCW <= 1;
		end
		else
		begin
			DIR_CW <= 1;
			DIR_CCW <= 0;
		end
		Done <= 0;

/////////////////////// Speed Control //////////////////////////////
		// if within step distance to target && current frequency is greater than min = SLOW DOWN
		if(((Location + accel_steps) > Target) && (current_freq > min_speed_freq)) //((Location - Target) < accel_steps) && (speed_count < min_speed_freq)
		begin
			if((current_freq - freq_increment) < min_speed_freq) // make sure it doesn't go below 0
				current_freq <= min_speed_freq;
			else
				current_freq <= current_freq - freq_increment;
		end
		// if within step distance to target && current freq is less than min = set freq to min
		else if((Location + accel_steps) > Target  && current_freq <= min_speed_freq) //(Location - Target) < accel_steps
		begin
			current_freq <= min_speed_freq;
		end
		// if current freq is great than max = limit freq to max
		else if(current_freq >= max_speed_freq)
		begin
			current_freq <= max_speed_freq;
		end
		// if its not within slow down distance, SPEED UP
		else if(current_freq < max_speed_freq)
		begin
			current_freq <= current_freq + freq_increment;
		end
		// run at min freq
		else
		begin
			current_freq <= min_speed_freq;
		end
		
		if(current_freq < min_speed_freq)
			current_freq <= min_speed_freq;
		
		// Update StatusCode	
		StatusCode <= countingUp;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
	end
///////////////// Counting down to target///////////////////////////
	else if(!Overlimit && Enable && (Location > Target)  && Homed)
	begin
		Location <= Location - 32'b1;
		if(CW_polarity)
		begin
			DIR_CW <= 1;
			DIR_CCW <= 0;
		end
		else
		begin
			DIR_CW <= 0;
			DIR_CCW <= 1;
		end
		Done <= 0;

/////////////////////// Speed Control //////////////////////////////
		
		if(((Target + accel_steps) > Location) && (current_freq > min_speed_freq)) // SLOW DOWN
		begin
			if((current_freq - freq_increment) < min_speed_freq) // make sure it doesn't go below 0
				current_freq <= min_speed_freq;
			else
				current_freq <= current_freq - freq_increment;
		end
		else if((Target + accel_steps) > Location && current_freq <= min_speed_freq) // STAY AT MIN
		begin
			current_freq <= min_speed_freq;
		end
		else if(current_freq >= max_speed_freq)
		begin
			current_freq <= max_speed_freq;			// Maintain Speed
		end
		else if(current_freq < max_speed_freq)
		begin
			current_freq <= current_freq + freq_increment; // Speed up
		end
		else
		begin
			current_freq <= min_speed_freq;				// Run at Min
		end
		
		if(current_freq < min_speed_freq)
			current_freq <= min_speed_freq;
		
		StatusCode <= countingDown;
///////////////////////////////////////////////////////////////////	
	end

	else // Unexpected Condition or not Homed
	begin
		Location <= Location;
		DIR_CW <= 0;
		DIR_CCW <= 0;
		Done <= 0;
		Overlimit <= 0;
		current_freq <= min_speed_freq;
		StatusCode <= other;
	end
	
	current_count <= (clk_freq_div_2/current_freq);
	
end

assign CW = Step & DIR_CW;
assign CCW = Step & DIR_CCW;



endmodule
