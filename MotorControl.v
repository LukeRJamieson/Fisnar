module MotorControl
(
input clk,
input rst,
// Software Control Signals
input EN,
input init_home,
input [18:0] target,
// Hardware Physical Signals
input LS,
input AWOff,
// Acceleration Parameters
input [25:0] clock_speed,
input [25:0] max_speed, //800,000 for XY, 320,000 of Z
input [25:0] min_speed,
input [25:0] accel_steps, // how many steps between min speed and max speed
// Movement and Range Parameters
input CW_polarity, // 0: CW = +1, 1: CW = -1
input [18:0] home_location, // after homing / at limit switch, what is the location
input [18:0] lower_limit,
input [18:0] upper_limit,

// Outputs
output CW,
output CCW,
output reg [18:0] location,
output targetAtLocation,

output [25:0] DEBUG_currentspeed,
output [25:0] DEBUG_speedincrement
);

// Acceleration Control (Dynamic Clock)
reg step;
reg [18:0] start_location;
reg [18:0] end_location; // Also the target
reg [19:0] counter;
reg [19:0] speed_divider;
reg [25:0] current_speed;
wire [25:0] speed_increment;
assign speed_increment = (max_speed - min_speed) / accel_steps;

initial
begin
	start_location = 0;
end
 

always@(posedge clk) //TODO
begin
	if(EN && !AWOff)
		begin
		end
		
	// state machine

	speed_divider <= clock_speed/current_speed; // divide by zero bug here if min_speed = 0
	counter = counter + 20'b1;
	if (counter >= speed_divider)
	begin
		
		counter <= 0;
		step <= ~step;
	end
	
	
end

reg homed;
always@(posedge step)
begin
	if(start_location == location)
		current_speed <= min_speed;
	else if (current_speed < max_speed)
		current_speed <= current_speed + speed_increment;
	else
		current_speed <= max_speed;


	
	location <= location + 1;
end

// DEBUG
assign CW = step;
assign DEBUG_currentspeed = current_speed;
assign DEBUG_speedincrement = speed_increment;

endmodule
