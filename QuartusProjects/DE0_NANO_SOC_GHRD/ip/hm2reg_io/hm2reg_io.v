module hm2reg_io (
        // signals to connect to an Avalon clock source interface
        input clk,
	input reset,
	// signals to connect to an Avalon-MM slave interface
	input [ADDRESS_WIDTH-1:0] slave_address,
	input slave_read,
	input slave_write,
	output [DATA_WIDTH-1:0] slave_readdata,
	input [DATA_WIDTH-1:0] slave_writedata,
	input slave_chipselect,
//	input slave_waitreq,
        // signals to connect to custom user logic
//	output waitrequest,
	output [DATA_WIDTH-1:0] user_dataout,
	input [DATA_WIDTH-1:0] user_datain,
	output [ADDRESS_WIDTH-1:0] user_adrout,
//	output reg env_sel,
//	output reg  osc_sel,
//	output reg  m1_sel,
//	output reg  m2_sel,
//	output reg  com_sel,
	output user_write_out,
	output user_read_out,
	output user_chip_sel
);

parameter ADDRESS_WIDTH = 16;          // address size width
parameter DATA_WIDTH = 32;          // word size of each input and output register


reg [DATA_WIDTH-1:0] indata;
reg [DATA_WIDTH-1:0] outdata;
//reg write_delay;
//reg reg_w_act;

//wire w_act = (write | write_delay);
//wire write_active = (write | reg_w_act); 

assign user_chip_sel = slave_chipselect;
//assign dataout = (!read && write_active && !waitreq) ? outdata : 32'bz;
assign user_dataout = outdata;
assign slave_readdata = indata;
assign user_adrout[ADDRESS_WIDTH-1:0] = slave_address[ADDRESS_WIDTH-1:0]; 
assign user_read_out = slave_read;
assign user_write_out = slave_write;

/*
always @(posedge clk) begin
	write_delay <= write;
	reg_w_act <= w_act;
end
*/

always @(posedge clk) begin
	if (reset) begin
		indata[DATA_WIDTH-1:0] <= 32'b0;
		outdata[DATA_WIDTH-1:0] <= 32'b0;
        end
	else if (slave_read)
		indata[DATA_WIDTH-1:0] <= user_datain[DATA_WIDTH-1:0];
	else if	(slave_write)
		outdata <= slave_writedata;
end
/*
always @(address or reset) begin
	if (reset)begin
		env_sel <= 1'b0;
		osc_sel <= 1'b0;
		m1_sel <= 1'b0;
		m2_sel <= 1'b0;
		com_sel <= 1'b0;
	end
	else begin
		case (address[9:7])
			3'd0: begin env_sel <= 1'b1;osc_sel <= 1'b0;m1_sel <= 1'b0;m2_sel <= 1'b0;com_sel <= 1'b0;end
			3'd1: begin env_sel <= 1'b0;osc_sel <= 1'b1;m1_sel <= 1'b0;m2_sel <= 1'b0;com_sel <= 1'b0;end
			3'd2: begin env_sel <= 1'b0;osc_sel <= 1'b0;m1_sel <= 1'b1;m2_sel <= 1'b0;com_sel <= 1'b0;end
			3'd3: begin env_sel <= 1'b0;osc_sel <= 1'b0;m1_sel <= 1'b0;m2_sel <= 1'b1;com_sel <= 1'b0;end
			3'd5: begin env_sel <= 1'b0;osc_sel <= 1'b0;m1_sel <= 1'b0;m2_sel <= 1'b0;com_sel <= 1'b1;end
			default: begin env_sel = 0; osc_sel = 0; m1_sel <= 0; m2_sel <= 0; com_sel <= 0; end
		endcase
	end
end
*/

endmodule
