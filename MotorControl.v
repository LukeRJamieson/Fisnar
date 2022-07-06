module MotorControl
(
input clk,
input rst,
// Software Control Signals
input Enable,
input Home,
input [31:0] Target,
// Hardware Physical Signals
input LS,
input AWOff,

// Acceleration Parameters
input [31:0] max_speed_count, // This is the counter value for running at maximum speed. eg max speed is 50000 (50mm/s). Using the 50MHz clock this will be (50,000,000/2)/2000 = 500
input [31:0] min_speed_count, // This is the counter value for running at minimum speed. eg min speed is 2000 (2mm/s). Using the 50MHz clock this will be (50,000,000/2)/2000 = 12500
input [31:0] accel_steps, // This is how many steps the motors should travel between the min and max speed. For this example we let it go from 2mm/s to 50mm/s in 10mm (10000 steps) . 
input [31:0] speed_increment, // (maxspeed-minspeed)/accel_steps (let the HPS work this out)

// Movement and Range Parameters
input CW_polarity, // 0: CW = +1, 1: CW = -1 (X and Z = 0; Y = 1)
input [31:0] home_location, // after homing / at limit switch, what is the location
input [31:0] lower_limit,
input [31:0] upper_limit,

// Outputs
output CW,
output CCW,
output reg [31:0] Location,
output reg Done //flag

);

// Acceleration Control (Dynamic Clock)
reg Step;
reg [31:0] counter;
reg [31:0] speed_count;
reg DIR_CW;
reg DIR_CCW;

initial
begin
	speed_count = min_speed_count;
end
 

always@(posedge clk) //TODO
begin
	counter = counter + 32'b1;
	if (Enable && (counter >= speed_count))
	begin
		counter <= 0;
		Step <= ~Step;
	end
end

always@(posedge Step)
begin
	if(Home && !LS)
		begin
			Location <= 0;
			DIR_CW <= 0;
			DIR_CCW <= 1;
			Done <= 0;
			speed_count <= min_speed_count;
		end

	else if(Home && LS)
		begin
			Location <= home_location;
			DIR_CW <= 0;
			DIR_CCW <= 0;
			Done <= 1;
			speed_count <= min_speed_count;
		end
	
///////////////// Counting up to target///////////////////////////
	else if(Enable && (Location < Target))
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
		if(((Location + accel_steps) > Target) && (speed_count < min_speed_count)) //((Location - Target) < accel_steps) && (speed_count < min_speed_count)
		begin
			speed_count <= speed_count + speed_increment;
		end
		else if((Location + accel_steps) > Target  && speed_count >= min_speed_count) //(Location - Target) < accel_steps
		begin
			speed_count <= min_speed_count;
		end
		else if(speed_count <= max_speed_count)
		begin
			speed_count <= max_speed_count;
		end
		else if(speed_count > max_speed_count)
		begin
			speed_count <= speed_count - speed_increment;
		end
		else
		begin
			speed_count <= min_speed_count;
		end
///////////////////////////////////////////////////////////////////			
	end
///////////////// Counting down to target///////////////////////////
	else if(Enable && (Location > Target))
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
		if(((Target + accel_steps) > Location) && (speed_count < min_speed_count))
		begin
			speed_count <= speed_count + speed_increment; // Slow Down
		end
		else if((Target + accel_steps) > Location && speed_count >= min_speed_count)
		begin
			speed_count <= min_speed_count;
		end
		else if(speed_count <= max_speed_count)
		begin
			speed_count <= max_speed_count;			// Maintain Speed
		end
		else if(speed_count > max_speed_count)
		begin
			speed_count <= speed_count - speed_increment; // Speed up
		end
		else
		begin
			speed_count <= min_speed_count;				// Run at Min
		end
///////////////////////////////////////////////////////////////////	
	end

	else if(Enable && (Location == Target))
	begin
		Location <= Location;
		DIR_CW <= 0;
		DIR_CCW <= 0;
		Done <= 1;
		speed_count <= min_speed_count;
	end

	else
	begin
		Location <= Location;
		DIR_CW <= 0;
		DIR_CCW <= 0;
		Done <= 0;
		speed_count <= min_speed_count;
	end
end

assign CW = Step & DIR_CW;
assign CCW = Step & DIR_CCW;


endmodule
