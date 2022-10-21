module ADCcontrol(
input FPGA_CLK1_50, // From FPGA
input wire serial_SDO, // From ADC
input reset_n, // From user key




output output_SCK, //To ADC 40MHz
output reg SDI,	// To ADC
output reg CONVSRT, // To ADC

output [15:0]ADC1,
output [15:0]ADC2


);


reg [11:0]ADC_ADDR;
reg ChannelCounter; //CH select

// State Parameters //
parameter ConvStart = 3'b000;
parameter ConvWait = 3'b001;
parameter Prepare = 3'b011;
parameter SendAddr = 3'b010;
parameter Wait = 3'b110;

reg SCK;

reg sending;
reg [3:0]currentState;
reg [3:0]nextState;
reg [7:0]bitcounter;
reg [5:0]convwaitcounter;
reg [11:0]ADCResult[1:0]; //originally 255 /////////////////////////////////////////////////////////
reg [11:0]Result;


reg [11:0]SDO;

reg [8:0] conversionCounter;
reg convClock;
reg [5:0]SCK_counter;
	
always @ (posedge(FPGA_CLK1_50))
		begin
			SCK_counter <= SCK_counter + 6'b1;
			if(SCK_counter == 0)
			begin
				SCK <= ~SCK;
				conversionCounter <= conversionCounter + 8'b1;
				if (conversionCounter < 60)
				begin
					convClock <= 1;
				end
				else
				begin
					convClock <= 0;
				end
			end
		end

always @ (posedge(SCK))
	begin: nextStateLogic
		currentState <= nextState;
	
	end
	
always @ (currentState,bitcounter,convwaitcounter,reset_n,convClock)
	begin: currentStateLogic
		case(currentState)
			ConvStart:
				begin
					if(!reset_n)
						nextState = ConvWait;
					else
						nextState = ConvWait;
				end
			ConvWait:
				begin
					if(!reset_n)
						nextState = ConvWait;
					else if(convwaitcounter == 0)
						nextState = Prepare;
					else
						nextState = ConvWait;
				end
				
			Prepare:
				begin
					if(!reset_n)
						nextState = ConvWait;
					else
						nextState = SendAddr;
				end			
				
			SendAddr:
				begin
					if(!reset_n)
						nextState = ConvWait;
					else if (bitcounter == 0)
						nextState = Wait;
					else 
						nextState = SendAddr;
				end
			Wait:
				begin
					if(!reset_n)
						nextState = ConvWait;
					else if (convClock == 1)
						nextState = ConvStart;
					else
						nextState = Wait; 
				end
			default: nextState = ConvStart;
		endcase
	
	end

// Update address sending on Negative Egde	
always @ (negedge(SCK))
	begin: outputLogic
		case(currentState)
			ConvStart:
				begin
					sending <= 0;
					CONVSRT <= 1;
					convwaitcounter <= 6'd60;

					ADCResult[ChannelCounter] <= Result;

					ChannelCounter <= ~ChannelCounter;
						
//					ADC_ADDR <= {1'b1,ChannelCounter,10'b00100_00000};
				
				end
			ConvWait:
				begin
					ADC_ADDR <= {1'b1,ChannelCounter,10'b00100_00000};
					sending <= 0;
					CONVSRT <= 0;
					convwaitcounter <= convwaitcounter - 6'd1;
					bitcounter <= 4'd12;
				end
			Prepare:
				begin
					SDI <= ADC_ADDR[bitcounter];
				end
				
			SendAddr:
				begin
					sending <= 1;
					CONVSRT <= 0;
					SDI <= ADC_ADDR[bitcounter-1];
					bitcounter <= bitcounter - 8'd1;
				end
			Wait:
				begin
					sending <= 0;
					CONVSRT <= 0;
					Result <= SDO[11:0];
				end
			default: nextState <= ConvStart;
		endcase
	end

// Read incoming data on positive edge	
always @ (posedge(SCK))
	begin
		if(sending)
			begin	
				SDO[bitcounter] <= serial_SDO;
			end
	end

assign output_SCK = sending ? SCK : 1'b0;

assign ADC1 = {4'b0,ADCResult[0]};
assign ADC2 = {4'b0,ADCResult[1]};


endmodule



