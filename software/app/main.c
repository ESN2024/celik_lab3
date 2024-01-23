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

/* Globales declarations defines */
#define ADXL_ADDR 0x1D

/* Normal speed */
#define I2C_SPEED 100000 

/* Register map */
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

/* Offset */
#define OFSX 0x1E
#define OFSY 0x1F
#define OFSZ 0x20

uint8_t X0, X1;
uint8_t Y0, Y1;
uint8_t Z0, Z1;
int16_t X, Y, Z;
uint8_t cpt_push = 0;

/* Write function */
void write_I2C(int16_t addr, int16_t cpt)
{
	I2C_start(OPENCORES_I2C_0_BASE, ADXL_ADDR, 0);	
	I2C_write(OPENCORES_I2C_0_BASE, addr, 0);
	I2C_write(OPENCORES_I2C_0_BASE, cpt, 1);
}

/* Read function */
uint8_t read_I2C(int16_t addr)
{
	uint8_t data;	
	
	I2C_start(OPENCORES_I2C_0_BASE, ADXL_ADDR, 0);	
	I2C_write(OPENCORES_I2C_0_BASE, addr, 0);
	I2C_start(OPENCORES_I2C_0_BASE, ADXL_ADDR, 1);
	
	data = I2C_read(OPENCORES_I2C_0_BASE, 1);
	
	return data;
}

/* Calibrate function */
void calibrate(void)
{
	write_I2C(OFSX, 0);
	write_I2C(OFSY, 0);
	write_I2C(OFSZ, -28);
}


/* Complement function */
int16_t signe(int16_t nb)
{
	nb = -(0xFFFF - nb + 1) - (0xFFFF - nb + 1);
	
	return nb * 3.9;
}

/* BCD_TO_SEG function */
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

	/* Write on BCD 7 SEGMENTS */
	IOWR_ALTERA_AVALON_PIO_DATA(SEG1_BASE, data[0]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG2_BASE, data[1]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG3_BASE, data[2]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG4_BASE, data[3]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG5_BASE, data[4]);
}

/* timer function */
static void timer_interrupt(void *Context)
{
	X0 = read_I2C(DATAX0);
	X1 = read_I2C(DATAX1);
	Y0 = read_I2C(DATAY0);
	Y1 = read_I2C(DATAY1);
	Z0 = read_I2C(DATAZ0);
	Z1 = read_I2C(DATAZ1);
	
	/* Concatenation LSB and MSB */
	X = (X1 << 8) | X0;
	Y = (Y1 << 8) | Y0;
	Z = (Z1 << 8) | Z0;
	
	/* Signe of the value X, Y and Z */
	signe(X);
	signe(Y);
	signe(Z);
		
	/* Sends data over the UART link */
	alt_printf("X : %x | Y : %x | Z : %x\n", X, Y, Z);
	
	switch(cpt_push)
	{
		case 0:
			BCD_TO_SEG(X);
		break;
		
		case 1:
			BCD_TO_SEG(Y);
		break;
		
		case 2:
			BCD_TO_SEG(Z);
		break;
	}
	
	/* Reset the timer interrupt */
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0b1);
}

/* key function */
static void key_interrupt(void *Context)
{
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0b1);
	cpt_push++;
	
	/*
	switch(cpt_push)
	{
		case 0: 
			alt_printf("Switch on X\n");
		break;
		
		case 1: 
			alt_printf("Switch on Y\n");
		break;
		
		case 2:
			alt_printf("Switch on Z\n");
		break;
		
		default:
			alt_printf("Switch on X\n");
		break;
	}
	*/
	
	if (cpt_push > 3)
	{
		cpt_push = 0;
	}
	
	
	
}
int main(void)
{
	/* Enable IRQ key */
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0b1);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0b1);
	
	/* I2C initialiation */
	I2C_init(OPENCORES_I2C_0_BASE, ALT_CPU_CPU_FREQ, I2C_SPEED);
	
	/* calibration */
	calibrate();
	
	alt_printf("Offset : %x | %x | %x\n", read_I2C(OFSX), read_I2C(OFSY), read_I2C(OFSZ));
	
	/* Register IRQ */
	alt_irq_register(TIMER_0_IRQ, NULL, timer_interrupt);
	alt_irq_register(KEY_IRQ, NULL, key_interrupt);
	
	while(1)
	{}
	
	return 0;	
}
