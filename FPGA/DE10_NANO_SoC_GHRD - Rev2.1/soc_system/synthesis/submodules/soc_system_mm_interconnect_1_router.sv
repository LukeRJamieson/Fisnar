// (C) 2001-2018 Intel Corporation. All rights reserved.
// Your use of Intel Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files from any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Intel Program License Subscription 
// Agreement, Intel FPGA IP License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Intel and sold by 
// Intel or its authorized distributors.  Please refer to the applicable 
// agreement for further details.



// Your use of Altera Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License Subscription 
// Agreement, Altera MegaCore Function License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Altera and sold by 
// Altera or its authorized distributors.  Please refer to the applicable 
// agreement for further details.


// $Id: //acds/rel/18.1std/ip/merlin/altera_merlin_router/altera_merlin_router.sv.terp#1 $
// $Revision: #1 $
// $Date: 2018/07/18 $
// $Author: psgswbuild $

// -------------------------------------------------------
// Merlin Router
//
// Asserts the appropriate one-hot encoded channel based on 
// either (a) the address or (b) the dest id. The DECODER_TYPE
// parameter controls this behaviour. 0 means address decoder,
// 1 means dest id decoder.
//
// In the case of (a), it also sets the destination id.
// -------------------------------------------------------

`timescale 1 ns / 1 ns

module soc_system_mm_interconnect_1_router_default_decode
  #(
     parameter DEFAULT_CHANNEL = 1,
               DEFAULT_WR_CHANNEL = -1,
               DEFAULT_RD_CHANNEL = -1,
               DEFAULT_DESTID = 0 
   )
  (output [98 - 93 : 0] default_destination_id,
   output [50-1 : 0] default_wr_channel,
   output [50-1 : 0] default_rd_channel,
   output [50-1 : 0] default_src_channel
  );

  assign default_destination_id = 
    DEFAULT_DESTID[98 - 93 : 0];

  generate
    if (DEFAULT_CHANNEL == -1) begin : no_default_channel_assignment
      assign default_src_channel = '0;
    end
    else begin : default_channel_assignment
      assign default_src_channel = 50'b1 << DEFAULT_CHANNEL;
    end
  endgenerate

  generate
    if (DEFAULT_RD_CHANNEL == -1) begin : no_default_rw_channel_assignment
      assign default_wr_channel = '0;
      assign default_rd_channel = '0;
    end
    else begin : default_rw_channel_assignment
      assign default_wr_channel = 50'b1 << DEFAULT_WR_CHANNEL;
      assign default_rd_channel = 50'b1 << DEFAULT_RD_CHANNEL;
    end
  endgenerate

endmodule


module soc_system_mm_interconnect_1_router
(
    // -------------------
    // Clock & Reset
    // -------------------
    input clk,
    input reset,

    // -------------------
    // Command Sink (Input)
    // -------------------
    input                       sink_valid,
    input  [112-1 : 0]    sink_data,
    input                       sink_startofpacket,
    input                       sink_endofpacket,
    output                      sink_ready,

    // -------------------
    // Command Source (Output)
    // -------------------
    output                          src_valid,
    output reg [112-1    : 0] src_data,
    output reg [50-1 : 0] src_channel,
    output                          src_startofpacket,
    output                          src_endofpacket,
    input                           src_ready
);

    // -------------------------------------------------------
    // Local parameters and variables
    // -------------------------------------------------------
    localparam PKT_ADDR_H = 67;
    localparam PKT_ADDR_L = 36;
    localparam PKT_DEST_ID_H = 98;
    localparam PKT_DEST_ID_L = 93;
    localparam PKT_PROTECTION_H = 102;
    localparam PKT_PROTECTION_L = 100;
    localparam ST_DATA_W = 112;
    localparam ST_CHANNEL_W = 50;
    localparam DECODER_TYPE = 0;

    localparam PKT_TRANS_WRITE = 70;
    localparam PKT_TRANS_READ  = 71;

    localparam PKT_ADDR_W = PKT_ADDR_H-PKT_ADDR_L + 1;
    localparam PKT_DEST_ID_W = PKT_DEST_ID_H-PKT_DEST_ID_L + 1;



    // -------------------------------------------------------
    // Figure out the number of bits to mask off for each slave span
    // during address decoding
    // -------------------------------------------------------
    localparam PAD0 = log2ceil(64'h1008 - 64'h1000); 
    localparam PAD1 = log2ceil(64'h2008 - 64'h2000); 
    localparam PAD2 = log2ceil(64'h3010 - 64'h3000); 
    localparam PAD3 = log2ceil(64'h4010 - 64'h4000); 
    localparam PAD4 = log2ceil(64'h5010 - 64'h5000); 
    localparam PAD5 = log2ceil(64'h6010 - 64'h6000); 
    localparam PAD6 = log2ceil(64'h6110 - 64'h6100); 
    localparam PAD7 = log2ceil(64'h6210 - 64'h6200); 
    localparam PAD8 = log2ceil(64'h6310 - 64'h6300); 
    localparam PAD9 = log2ceil(64'h6410 - 64'h6400); 
    localparam PAD10 = log2ceil(64'h6510 - 64'h6500); 
    localparam PAD11 = log2ceil(64'h6610 - 64'h6600); 
    localparam PAD12 = log2ceil(64'h6710 - 64'h6700); 
    localparam PAD13 = log2ceil(64'h6810 - 64'h6800); 
    localparam PAD14 = log2ceil(64'h6910 - 64'h6900); 
    localparam PAD15 = log2ceil(64'h6a10 - 64'h6a00); 
    localparam PAD16 = log2ceil(64'h6b10 - 64'h6b00); 
    localparam PAD17 = log2ceil(64'h6c10 - 64'h6c00); 
    localparam PAD18 = log2ceil(64'h6d10 - 64'h6d00); 
    localparam PAD19 = log2ceil(64'h6e10 - 64'h6e00); 
    localparam PAD20 = log2ceil(64'h6f10 - 64'h6f00); 
    localparam PAD21 = log2ceil(64'h7010 - 64'h7000); 
    localparam PAD22 = log2ceil(64'h7110 - 64'h7100); 
    localparam PAD23 = log2ceil(64'h7210 - 64'h7200); 
    localparam PAD24 = log2ceil(64'h7310 - 64'h7300); 
    localparam PAD25 = log2ceil(64'h7410 - 64'h7400); 
    localparam PAD26 = log2ceil(64'h7510 - 64'h7500); 
    localparam PAD27 = log2ceil(64'h7610 - 64'h7600); 
    localparam PAD28 = log2ceil(64'h7710 - 64'h7700); 
    localparam PAD29 = log2ceil(64'h7810 - 64'h7800); 
    localparam PAD30 = log2ceil(64'h7910 - 64'h7900); 
    localparam PAD31 = log2ceil(64'h7a10 - 64'h7a00); 
    localparam PAD32 = log2ceil(64'h7b10 - 64'h7b00); 
    localparam PAD33 = log2ceil(64'h7c10 - 64'h7c00); 
    localparam PAD34 = log2ceil(64'h7d10 - 64'h7d00); 
    localparam PAD35 = log2ceil(64'h7e10 - 64'h7e00); 
    localparam PAD36 = log2ceil(64'h7f10 - 64'h7f00); 
    localparam PAD37 = log2ceil(64'h8010 - 64'h8000); 
    localparam PAD38 = log2ceil(64'h8110 - 64'h8100); 
    localparam PAD39 = log2ceil(64'h8210 - 64'h8200); 
    localparam PAD40 = log2ceil(64'h8310 - 64'h8300); 
    localparam PAD41 = log2ceil(64'h8410 - 64'h8400); 
    localparam PAD42 = log2ceil(64'h8510 - 64'h8500); 
    localparam PAD43 = log2ceil(64'h8610 - 64'h8600); 
    localparam PAD44 = log2ceil(64'h8710 - 64'h8700); 
    localparam PAD45 = log2ceil(64'h8810 - 64'h8800); 
    localparam PAD46 = log2ceil(64'h8910 - 64'h8900); 
    localparam PAD47 = log2ceil(64'h8a10 - 64'h8a00); 
    localparam PAD48 = log2ceil(64'h8b10 - 64'h8b00); 
    localparam PAD49 = log2ceil(64'h30100 - 64'h30000); 
    // -------------------------------------------------------
    // Work out which address bits are significant based on the
    // address range of the slaves. If the required width is too
    // large or too small, we use the address field width instead.
    // -------------------------------------------------------
    localparam ADDR_RANGE = 64'h30100;
    localparam RANGE_ADDR_WIDTH = log2ceil(ADDR_RANGE);
    localparam OPTIMIZED_ADDR_H = (RANGE_ADDR_WIDTH > PKT_ADDR_W) ||
                                  (RANGE_ADDR_WIDTH == 0) ?
                                        PKT_ADDR_H :
                                        PKT_ADDR_L + RANGE_ADDR_WIDTH - 1;

    localparam RG = RANGE_ADDR_WIDTH-1;
    localparam REAL_ADDRESS_RANGE = OPTIMIZED_ADDR_H - PKT_ADDR_L;

      reg [PKT_ADDR_W-1 : 0] address;
      always @* begin
        address = {PKT_ADDR_W{1'b0}};
        address [REAL_ADDRESS_RANGE:0] = sink_data[OPTIMIZED_ADDR_H : PKT_ADDR_L];
      end   

    // -------------------------------------------------------
    // Pass almost everything through, untouched
    // -------------------------------------------------------
    assign sink_ready        = src_ready;
    assign src_valid         = sink_valid;
    assign src_startofpacket = sink_startofpacket;
    assign src_endofpacket   = sink_endofpacket;
    wire [PKT_DEST_ID_W-1:0] default_destid;
    wire [50-1 : 0] default_src_channel;




    // -------------------------------------------------------
    // Write and read transaction signals
    // -------------------------------------------------------
    wire read_transaction;
    assign read_transaction  = sink_data[PKT_TRANS_READ];


    soc_system_mm_interconnect_1_router_default_decode the_default_decode(
      .default_destination_id (default_destid),
      .default_wr_channel   (),
      .default_rd_channel   (),
      .default_src_channel  (default_src_channel)
    );

    always @* begin
        src_data    = sink_data;
        src_channel = default_src_channel;
        src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = default_destid;

        // --------------------------------------------------
        // Address Decoder
        // Sets the channel and destination ID based on the address
        // --------------------------------------------------

    // ( 0x1000 .. 0x1008 )
    if ( {address[RG:PAD0],{PAD0{1'b0}}} == 18'h1000  && read_transaction  ) begin
            src_channel = 50'b00000000000000000000000000000000000000000000000100;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 11;
    end

    // ( 0x2000 .. 0x2008 )
    if ( {address[RG:PAD1],{PAD1{1'b0}}} == 18'h2000   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000000000001;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 8;
    end

    // ( 0x3000 .. 0x3010 )
    if ( {address[RG:PAD2],{PAD2{1'b0}}} == 18'h3000   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000000001000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 9;
    end

    // ( 0x4000 .. 0x4010 )
    if ( {address[RG:PAD3],{PAD3{1'b0}}} == 18'h4000   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000000010000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 3;
    end

    // ( 0x5000 .. 0x5010 )
    if ( {address[RG:PAD4],{PAD4{1'b0}}} == 18'h5000   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000000100000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 1;
    end

    // ( 0x6000 .. 0x6010 )
    if ( {address[RG:PAD5],{PAD5{1'b0}}} == 18'h6000   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000001000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 24;
    end

    // ( 0x6100 .. 0x6110 )
    if ( {address[RG:PAD6],{PAD6{1'b0}}} == 18'h6100   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000010000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 36;
    end

    // ( 0x6200 .. 0x6210 )
    if ( {address[RG:PAD7],{PAD7{1'b0}}} == 18'h6200   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000100000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 48;
    end

    // ( 0x6300 .. 0x6310 )
    if ( {address[RG:PAD8],{PAD8{1'b0}}} == 18'h6300  && read_transaction  ) begin
            src_channel = 50'b00000000000000000000000000000000000000001000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 18;
    end

    // ( 0x6400 .. 0x6410 )
    if ( {address[RG:PAD9],{PAD9{1'b0}}} == 18'h6400  && read_transaction  ) begin
            src_channel = 50'b00000000000000000000000000000000000000010000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 30;
    end

    // ( 0x6500 .. 0x6510 )
    if ( {address[RG:PAD10],{PAD10{1'b0}}} == 18'h6500  && read_transaction  ) begin
            src_channel = 50'b00000000000000000000000000000000000000100000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 42;
    end

    // ( 0x6600 .. 0x6610 )
    if ( {address[RG:PAD11],{PAD11{1'b0}}} == 18'h6600   ) begin
            src_channel = 50'b00000000000000000000000000000000000001000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 15;
    end

    // ( 0x6700 .. 0x6710 )
    if ( {address[RG:PAD12],{PAD12{1'b0}}} == 18'h6700   ) begin
            src_channel = 50'b00000000000000000000000000000000000010000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 27;
    end

    // ( 0x6800 .. 0x6810 )
    if ( {address[RG:PAD13],{PAD13{1'b0}}} == 18'h6800   ) begin
            src_channel = 50'b00000000000000000000000000000000000100000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 39;
    end

    // ( 0x6900 .. 0x6910 )
    if ( {address[RG:PAD14],{PAD14{1'b0}}} == 18'h6900   ) begin
            src_channel = 50'b00000000000000000000000000000000001000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 20;
    end

    // ( 0x6a00 .. 0x6a10 )
    if ( {address[RG:PAD15],{PAD15{1'b0}}} == 18'h6a00   ) begin
            src_channel = 50'b00000000000000000000000000000000010000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 32;
    end

    // ( 0x6b00 .. 0x6b10 )
    if ( {address[RG:PAD16],{PAD16{1'b0}}} == 18'h6b00   ) begin
            src_channel = 50'b00000000000000000000000000000000100000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 44;
    end

    // ( 0x6c00 .. 0x6c10 )
    if ( {address[RG:PAD17],{PAD17{1'b0}}} == 18'h6c00   ) begin
            src_channel = 50'b00000000000000000000000000000001000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 21;
    end

    // ( 0x6d00 .. 0x6d10 )
    if ( {address[RG:PAD18],{PAD18{1'b0}}} == 18'h6d00   ) begin
            src_channel = 50'b00000000000000000000000000000010000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 33;
    end

    // ( 0x6e00 .. 0x6e10 )
    if ( {address[RG:PAD19],{PAD19{1'b0}}} == 18'h6e00   ) begin
            src_channel = 50'b00000000000000000000000000000100000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 45;
    end

    // ( 0x6f00 .. 0x6f10 )
    if ( {address[RG:PAD20],{PAD20{1'b0}}} == 18'h6f00   ) begin
            src_channel = 50'b00000000000000000000000000001000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 14;
    end

    // ( 0x7000 .. 0x7010 )
    if ( {address[RG:PAD21],{PAD21{1'b0}}} == 18'h7000   ) begin
            src_channel = 50'b00000000000000000000000000010000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 26;
    end

    // ( 0x7100 .. 0x7110 )
    if ( {address[RG:PAD22],{PAD22{1'b0}}} == 18'h7100   ) begin
            src_channel = 50'b00000000000000000000000000100000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 38;
    end

    // ( 0x7200 .. 0x7210 )
    if ( {address[RG:PAD23],{PAD23{1'b0}}} == 18'h7200   ) begin
            src_channel = 50'b00000000000000000000000001000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 23;
    end

    // ( 0x7300 .. 0x7310 )
    if ( {address[RG:PAD24],{PAD24{1'b0}}} == 18'h7300   ) begin
            src_channel = 50'b00000000000000000000000010000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 35;
    end

    // ( 0x7400 .. 0x7410 )
    if ( {address[RG:PAD25],{PAD25{1'b0}}} == 18'h7400   ) begin
            src_channel = 50'b00000000000000000000000100000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 47;
    end

    // ( 0x7500 .. 0x7510 )
    if ( {address[RG:PAD26],{PAD26{1'b0}}} == 18'h7500   ) begin
            src_channel = 50'b00000000000000000000001000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 25;
    end

    // ( 0x7600 .. 0x7610 )
    if ( {address[RG:PAD27],{PAD27{1'b0}}} == 18'h7600   ) begin
            src_channel = 50'b00000000000000000000100000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 37;
    end

    // ( 0x7700 .. 0x7710 )
    if ( {address[RG:PAD28],{PAD28{1'b0}}} == 18'h7700   ) begin
            src_channel = 50'b00000000000000000000010000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 49;
    end

    // ( 0x7800 .. 0x7810 )
    if ( {address[RG:PAD29],{PAD29{1'b0}}} == 18'h7800   ) begin
            src_channel = 50'b00000000000000000001000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 19;
    end

    // ( 0x7900 .. 0x7910 )
    if ( {address[RG:PAD30],{PAD30{1'b0}}} == 18'h7900   ) begin
            src_channel = 50'b00000000000000000010000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 31;
    end

    // ( 0x7a00 .. 0x7a10 )
    if ( {address[RG:PAD31],{PAD31{1'b0}}} == 18'h7a00   ) begin
            src_channel = 50'b00000000000000000100000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 43;
    end

    // ( 0x7b00 .. 0x7b10 )
    if ( {address[RG:PAD32],{PAD32{1'b0}}} == 18'h7b00   ) begin
            src_channel = 50'b00000000000000001000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 16;
    end

    // ( 0x7c00 .. 0x7c10 )
    if ( {address[RG:PAD33],{PAD33{1'b0}}} == 18'h7c00   ) begin
            src_channel = 50'b00000000000000010000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 28;
    end

    // ( 0x7d00 .. 0x7d10 )
    if ( {address[RG:PAD34],{PAD34{1'b0}}} == 18'h7d00   ) begin
            src_channel = 50'b00000000000000100000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 40;
    end

    // ( 0x7e00 .. 0x7e10 )
    if ( {address[RG:PAD35],{PAD35{1'b0}}} == 18'h7e00   ) begin
            src_channel = 50'b00000000000001000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 17;
    end

    // ( 0x7f00 .. 0x7f10 )
    if ( {address[RG:PAD36],{PAD36{1'b0}}} == 18'h7f00   ) begin
            src_channel = 50'b00000000000010000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 29;
    end

    // ( 0x8000 .. 0x8010 )
    if ( {address[RG:PAD37],{PAD37{1'b0}}} == 18'h8000   ) begin
            src_channel = 50'b00000000000100000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 41;
    end

    // ( 0x8100 .. 0x8110 )
    if ( {address[RG:PAD38],{PAD38{1'b0}}} == 18'h8100  && read_transaction  ) begin
            src_channel = 50'b00000000001000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 22;
    end

    // ( 0x8200 .. 0x8210 )
    if ( {address[RG:PAD39],{PAD39{1'b0}}} == 18'h8200  && read_transaction  ) begin
            src_channel = 50'b00000000010000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 34;
    end

    // ( 0x8300 .. 0x8310 )
    if ( {address[RG:PAD40],{PAD40{1'b0}}} == 18'h8300  && read_transaction  ) begin
            src_channel = 50'b00000000100000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 46;
    end

    // ( 0x8400 .. 0x8410 )
    if ( {address[RG:PAD41],{PAD41{1'b0}}} == 18'h8400   ) begin
            src_channel = 50'b00000001000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 2;
    end

    // ( 0x8500 .. 0x8510 )
    if ( {address[RG:PAD42],{PAD42{1'b0}}} == 18'h8500   ) begin
            src_channel = 50'b00000010000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 10;
    end

    // ( 0x8600 .. 0x8610 )
    if ( {address[RG:PAD43],{PAD43{1'b0}}} == 18'h8600  && read_transaction  ) begin
            src_channel = 50'b00000100000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 7;
    end

    // ( 0x8700 .. 0x8710 )
    if ( {address[RG:PAD44],{PAD44{1'b0}}} == 18'h8700   ) begin
            src_channel = 50'b00001000000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 4;
    end

    // ( 0x8800 .. 0x8810 )
    if ( {address[RG:PAD45],{PAD45{1'b0}}} == 18'h8800   ) begin
            src_channel = 50'b00010000000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 6;
    end

    // ( 0x8900 .. 0x8910 )
    if ( {address[RG:PAD46],{PAD46{1'b0}}} == 18'h8900  && read_transaction  ) begin
            src_channel = 50'b00100000000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 5;
    end

    // ( 0x8a00 .. 0x8a10 )
    if ( {address[RG:PAD47],{PAD47{1'b0}}} == 18'h8a00   ) begin
            src_channel = 50'b01000000000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 13;
    end

    // ( 0x8b00 .. 0x8b10 )
    if ( {address[RG:PAD48],{PAD48{1'b0}}} == 18'h8b00  && read_transaction  ) begin
            src_channel = 50'b10000000000000000000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 12;
    end

    // ( 0x30000 .. 0x30100 )
    if ( {address[RG:PAD49],{PAD49{1'b0}}} == 18'h30000   ) begin
            src_channel = 50'b00000000000000000000000000000000000000000000000010;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 0;
    end

end


    // --------------------------------------------------
    // Ceil(log2()) function
    // --------------------------------------------------
    function integer log2ceil;
        input reg[65:0] val;
        reg [65:0] i;

        begin
            i = 1;
            log2ceil = 0;

            while (i < val) begin
                log2ceil = log2ceil + 1;
                i = i << 1;
            end
        end
    endfunction

endmodule


