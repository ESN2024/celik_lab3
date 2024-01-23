library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity BCD_7_SEG is
		port (
			e : in std_logic_vector(3 downto 0); -- input 
			s : out std_logic_vector(7 downto 0) -- output 
		);
end entity;


architecture arch of BCD_7_SEG is

begin
	s <= 	"11000000" when e = "0000" else 
			"11111001" when e = "0001" else 
			"10100100" when e = "0010" else 
			"10110000" when e = "0011" else 
			"10011001" when e = "0100" else 
			"10010010" when e = "0101" else 
			"10000010" when e = "0110" else 
			"11111000" when e = "0111" else 
			"10000000" when e = "1000" else 
			"10010000" when e = "1001" else	
			"10111111" when e = "1111" else
			"11111111";

end architecture;