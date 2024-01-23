#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "system.h"
#include <sys/alt_irq.h>
#include <io.h>
#include <alt_types.h>
#include <sys/alt_stdio.h>
#include "sys/alt_sys_init.h"
#include "opencores_i2c.h"
#include "opencores_i2c_regs.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include "altera_avalon_pio_regs.h"

uint8_t X0, X1;
uint8_t Y0, Y1;
uint8_t Z0, Z1;
int16_t X, Y, Z;

#define ADXL_ADDR 0x1D
#define I2C_SPEED 100000 // Normal speed

#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

void write_I2C(int16_t addr, int16_t cpt)
{
	I2C_start(OPENCORES_I2C_0_BASE, ADXL_ADDR, 0);	
	I2C_write(OPENCORES_I2C_0_BASE, addr, 0);
	I2C_write(OPENCORES_I2C_0_BASE, cpt, 1);
}

uint8_t read_I2C(int16_t addr)
{
	uint8_t data;	
	
	I2C_start(OPENCORES_I2C_0_BASE, ADXL_ADDR, 0);	
	I2C_write(OPENCORES_I2C_0_BASE, addr, 0);
	I2C_start(OPENCORES_I2C_0_BASE, ADXL_ADDR, 1);
	
	data = I2C_read(OPENCORES_I2C_0_BASE, 1);
	
	return data;
}

void BCD_TO_SEG(int16_t cpt)
{
	uint8_t data[5] = {0,0,0,0,0};
	
	if (cpt < 0)
	{
		data[0] = -cpt % 10 ;
		data[1] = (-cpt/10) % 10 ;
		data[2] = (-cpt/100) % 10 ;
		data[3] = -cpt / 1000 ;
		data[4] = 15;			
	}
	else
	{
		data[0] = cpt % 10 ;
		data[1] = (cpt/10) % 10 ;
		data[2] = (cpt/100) % 10 ;
		data[3] = cpt / 1000 ;
		data[4] = 0;		
	}

	// Write on BCD 7 SEGMENTS
	IOWR_ALTERA_AVALON_PIO_DATA(SEG1_BASE,data[0]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG2_BASE,data[1]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG3_BASE,data[2]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG4_BASE,data[3]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG5_BASE,data[4]);
}

static void timer_interrupt(void *Context)
{
	X0 = read_I2C(DATAX0);
	X1 = read_I2C(DATAX1);
	Y0 = read_I2C(DATAY0);
	Y1 = read_I2C(DATAY1);
	Z0 = read_I2C(DATAZ0);
	Z1 = read_I2C(DATAZ1);
	
	X = (X1 << 8) | X0;
	Y = (Y1 << 8) | Y0;
	Z = (Z1 << 8) | Z0;
		
	// Sends data over the UART link
	alt_printf("X : %x || Y : %x || Z : %x\n", X, Y, Z);
	
	BCD_TO_SEG(X);
	
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0b1);
}

int main(void)
{
	I2C_init(OPENCORES_I2C_0_BASE, ALT_CPU_CPU_FREQ, I2C_SPEED);
	
	// Register IRQ
	alt_irq_register(TIMER_0_IRQ, NULL, timer_interrupt);
	
	while(1)
	{}
	
	return 0;	
}
