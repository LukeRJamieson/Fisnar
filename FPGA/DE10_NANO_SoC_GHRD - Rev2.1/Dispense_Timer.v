module Dispense_Timer
(
input FPGA_CLK1_50,
input [30:0] timer,
input start_timer,
output dispense_sig
);

reg [30:0] counter; //26-bit counter = 67M
reg count_done;
reg run_timer;
reg nStart_signal;
reg reg_dispense;

assign dispense_sig = reg_dispense;

always @(posedge FPGA_CLK1_50)
begin
	nStart_signal <= ~start_timer;

	if(start_timer && ~reg_dispense && nStart_signal)
		run_timer <= 1;
	
	if(run_timer)
	begin
		if(counter >= timer)
		begin
			run_timer <= 0;
			reg_dispense <= 0;
			counter <= 0;
		end
		else if (counter < timer)
		begin
			reg_dispense <= 1;
			counter <= counter + 31'b1;
		end
	end

end


endmodule
