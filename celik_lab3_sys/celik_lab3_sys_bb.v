
module celik_lab3_sys (
	clk_clk,
	key_external_connection_export,
	opencores_i2c_0_export_0_scl_pad_io,
	opencores_i2c_0_export_0_sda_pad_io,
	reset_reset_n,
	seg1_external_connection_export,
	seg2_external_connection_export,
	seg3_external_connection_export,
	seg4_external_connection_export,
	seg5_external_connection_export,
	seg6_external_connection_export);	

	input		clk_clk;
	input		key_external_connection_export;
	inout		opencores_i2c_0_export_0_scl_pad_io;
	inout		opencores_i2c_0_export_0_sda_pad_io;
	input		reset_reset_n;
	output	[3:0]	seg1_external_connection_export;
	output	[3:0]	seg2_external_connection_export;
	output	[3:0]	seg3_external_connection_export;
	output	[3:0]	seg4_external_connection_export;
	output	[3:0]	seg5_external_connection_export;
	output	[3:0]	seg6_external_connection_export;
endmodule
