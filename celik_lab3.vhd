library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity celik_lab3 is 
		port (
			CLK : in std_logic := '0';
			RESET : in std_logic :='0';
			SCL : inout std_logic := 'X';
			SDA : inout std_logic := 'X';
			CS_n : out std_logic := '1';
			KEY : in std_logic := 'X';
			SEG1 : out std_logic_vector(7 downto 0);
			SEG2 : out std_logic_vector(7 downto 0);
			SEG3 : out std_logic_vector(7 downto 0);
			SEG4 : out std_logic_vector(7 downto 0);
			SEG5 : out std_logic_vector(7 downto 0);
			SEG6 : out std_logic_vector(7 downto 0)
		);
end entity;


architecture arch of celik_lab3 is 

		component celik_lab3_sys is
				port (
					clk_clk                             : in    std_logic                    := 'X';             -- clk
					key_external_connection_export      : in    std_logic                    := 'X'; 				-- export
					opencores_i2c_0_export_0_scl_pad_io : inout std_logic                    := 'X';             -- scl_pad_io
					opencores_i2c_0_export_0_sda_pad_io : inout std_logic                    := 'X';             -- sda_pad_io
					reset_reset_n                       : in    std_logic                    := 'X';             -- reset_n
					seg1_external_connection_export     : out   std_logic_vector(3 downto 0);        				-- export
					seg2_external_connection_export     : out   std_logic_vector(3 downto 0);        				-- export
					seg3_external_connection_export     : out   std_logic_vector(3 downto 0);        				-- export
					seg4_external_connection_export     : out   std_logic_vector(3 downto 0);        				-- export
					seg5_external_connection_export     : out   std_logic_vector(3 downto 0);        				-- export
					seg6_external_connection_export     : out   std_logic_vector(3 downto 0)         				-- export                
				);
		end component celik_lab3_sys;
		
		component BCD_7_SEG is 																											-- BCD_7_SEG.vhd
				port (
						e : in std_logic_vector(3 downto 0) := (others => 'X'); 
						s : out std_logic_vector(7 downto 0)
				);
		end component BCD_7_SEG;

	
signal seg_1 : std_logic_vector(3 downto 0);
signal seg_2 : std_logic_vector(3 downto 0);
signal seg_3 : std_logic_vector(3 downto 0);
signal seg_4 : std_logic_vector(3 downto 0);
signal seg_5 : std_logic_vector(3 downto 0);
signal seg_6 : std_logic_vector(3 downto 0);


begin
		u0 : component celik_lab3_sys
					port map (
						clk_clk                             => CLK,                             						-- clk.clk
						key_external_connection_export      => KEY,      														-- key_external_connection.export
						opencores_i2c_0_export_0_scl_pad_io => SCL, 																-- opencores_i2c_0_export_0.scl_pad_io
						opencores_i2c_0_export_0_sda_pad_io => SDA, 																-- .sda_pad_io
						reset_reset_n                       => RESET,                       								-- reset.reset_n
						seg1_external_connection_export     => seg_1,     														-- seg1_external_connection.export
						seg2_external_connection_export     => seg_2,     														-- seg2_external_connection.export
						seg3_external_connection_export     => seg_3,      													-- seg3_external_connection.export
						seg4_external_connection_export     => seg_4, 															-- seg4_external_connection.export
						seg5_external_connection_export     => seg_5, 															-- seg5_external_connection.export
						seg6_external_connection_export     => seg_6 															-- seg6_external_connection.export
					);
				
		u1 : component BCD_7_SEG
					port map (
						e => seg_1,
						s => SEG1
					);
			
		u2 : component BCD_7_SEG
					port map (
						e => seg_2,
						s => SEG2
					);

		u3 : component BCD_7_SEG
					port map (
						e => seg_3,
						s => SEG3
					);
					
		u4 : component BCD_7_SEG
					port map (
						e => seg_4,
						s => SEG4
					);
			
		u5 : component BCD_7_SEG
					port map (
						e => seg_5,
						s => SEG5
					);

		u6 : component BCD_7_SEG
					port map (
						e => seg_6,
						s => SEG6
					);
			
end architecture;








    
   


