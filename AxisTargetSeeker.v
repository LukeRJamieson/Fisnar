module AxisTargetSeeker
(
input Clock,
input Reset,
input Enable,
input [18:0] Target,
input Home,
input LS,
input [18:0] Limit,

output reg [18:0] Location,
output CW, // Clockwise increments location
output CCW,// CCW decrements location
output reg Done

);
reg DIR_CW;
reg DIR_CCW;
reg Step;
reg Start_Location;
reg End_Location;


// Acceleration Control (Dynamic Clock)
always@(posedge Clock) //TODO
begin
	if(Enable && (Location <= Limit))
		Step <= ~Step;
	else
		Step <= 0;
end


// Move to Target
always@(posedge Step)
begin
	if(Home && !LS)
		begin
			Location <= 0;
			DIR_CW <= 0;
			DIR_CCW <= 1;
			Done <= 0;
		end
	else if(Home && LS)
			begin
			Location <= 0;
			DIR_CW <= 0;
			DIR_CCW <= 0;
			Done <= 1;
		end
	
	else if(Enable && (Location < Target))
	begin
		Location <= Location + 19'b1;
		DIR_CW <= 1;
		DIR_CCW <= 0;
		Done <= 0;
	end
	else if(Enable && (Location > Target))
	begin
		Location <= Location - 19'b1;
		DIR_CW <= 0;
		DIR_CCW <= 1;
		Done <= 0;
	end
	else if(Enable && (Location == Target))
	begin
		Location <= Location;
		DIR_CW <= 0;
		DIR_CCW <= 0;
		Done <= 1;
	end
	else
	begin
		Location <= Location;
		DIR_CW <= 0;
		DIR_CCW <= 0;
		Done <= 0;
	end

end

assign CW = Step & DIR_CW;
assign CCW = Step & DIR_CCW;


endmodule
