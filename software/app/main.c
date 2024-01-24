#include "system.h"
#include "sys/alt_stdio.h"
#include "alt_types.h"
#include "io.h"
#include "unistd.h"
#include "sys/alt_sys_init.h"
#include "opencores_i2c.h"
#include "opencores_i2c_regs.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include <stdio.h>
#include <stdint.h>

#define ALT_ADDR 0x1D
#define SPEED 100000

#define ACT_INACT_CTL 0X27
#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31

#define X0 0x32
#define X1 0x33
#define Y0 0x34
#define Y1 0x35
#define Z0 0x36
#define Z1 0x37


#define OFSX 0x1E
#define OFSY 0x1F
#define OFSZ 0x20

#define CALIBX 0
#define CALIBY 2
#define CALIBZ 18

int16_t A0,A1;

//Fonction pour lire avec l'I2C
int16_t lecture_i2c(int16_t base,int16_t addr){
	
	int16_t data = 0;
	I2C_start(base, ALT_ADDR, 0);
	I2C_write(base,addr,0);
	
	I2C_start(base,ALT_ADDR,1);
	data = I2C_read(base,1);
	
	return data;
}

//Fonction pour ecrire avec l'I2C
void ecriture_i2c(int16_t base,int16_t addr,int16_t value){
	I2C_start(base, ALT_ADDR, 0);
	I2C_write(base,addr,0);
	I2C_write(base,value,1);
}

//Fonction pour la calibration 
void calibration(){
	//Ecriture OFSZ
	ecriture_i2c(OPENCORES_I2C_0_BASE,OFSZ,CALIBZ);
	usleep(100000);
	
	//Ecriture OFSX
	ecriture_i2c(OPENCORES_I2C_0_BASE,OFSX,CALIBX);
	usleep(100000);
	
	//Ecriture OFSY
	ecriture_i2c(OPENCORES_I2C_0_BASE,OFSY,CALIBY);
	usleep(100000);
	
}

//Fonction qui prend un entier et le decoupe et ecrit sur les registres des 7 segments
void int_to_seg(int16_t nbr){
	int8_t i =0;
	int32_t tab[5] = {0,0,0,0,0};
	//nbr = (short)nbr;
	
	if(nbr < 0){
		IOWR_ALTERA_AVALON_PIO_DATA(SEG5_BASE,0b0111111);
		nbr = nbr * -1;
		
	}
	else{
		IOWR_ALTERA_AVALON_PIO_DATA(SEG5_BASE,0b1000000);
		
	}
	
	while (nbr >= 10) {
		tab[i] = nbr % 10;
		nbr = nbr / 10;
		i = i+1;
	}
	tab[i] = nbr;

	IOWR_ALTERA_AVALON_PIO_DATA(SEG1_BASE,tab[0]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG2_BASE,tab[1]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG3_BASE,tab[2]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG4_BASE,tab[3]);
	IOWR_ALTERA_AVALON_PIO_DATA(SEG5_BASE,tab[4]);

}

// Prend LSB MSB et donne le data final pour l'affichache sur les 7seg
void affichage_XYZ(int16_t A0,int16_t A1){
	int16_t a0,a1,a;
	a0 = lecture_i2c(OPENCORES_I2C_0_BASE,A0);
	a1 = lecture_i2c(OPENCORES_I2C_0_BASE,A1);
	a = (a1<<8)|a0;
	
	a=	(short) a;
	a = a*3.9;
	
	int_to_seg(a);
}

//Fonction pour l'affichache par UART
void affichache_UART(){
	int16_t x1,x0,y1,y0,z1,z0;
	//Lecture X0 Lecture X1
	x0 = lecture_i2c(OPENCORES_I2C_0_BASE,X0);
	x1 = lecture_i2c(OPENCORES_I2C_0_BASE,X1);
	//Lecture Y0 Lecture Y1
	y0 = lecture_i2c(OPENCORES_I2C_0_BASE,Y0);
	y1 = lecture_i2c(OPENCORES_I2C_0_BASE,Y1);
	//Lecture Z0 Lecture Z1
	z0 = lecture_i2c(OPENCORES_I2C_0_BASE,Z0);
	z1 = lecture_i2c(OPENCORES_I2C_0_BASE,Z1);
	
	alt_printf("X= %x, Y= %x, Z= %x\n",(x1<<8)|x0,(y1<<8)|y0,(z1<<8)|z0);
	alt_printf("--------------------\n");
}

//Fonction pour l'initialisation du ADXL345
void init_ADXL345(){
	
	alt_printf("Initialisation I2C\n");
	I2C_init(OPENCORES_I2C_0_BASE, ALT_CPU_FREQ, SPEED);
	if(I2C_start(OPENCORES_I2C_0_BASE, ALT_ADDR, 0)== 0){
		
		alt_printf("Init ok\n");
	}
	//Ecriture POWER_CTL
	alt_printf("Ecriture 0X08 -> POWER_CTL\n");
	ecriture_i2c(OPENCORES_I2C_0_BASE,POWER_CTL,0x08);
	usleep(100000);
	
	//Lecture POWER_CTL
	alt_printf("Lecture POWER_CTL\n");
	alt_printf("POWER_CTL = %x\n",lecture_i2c(OPENCORES_I2C_0_BASE,POWER_CTL));
	
	//D1=1 et D0=1 -> +-16g 
	//Ecriture DATA_FORMAT
	alt_printf("Ecriture 0x07 -> DATA_FORMAT\n");
	ecriture_i2c(OPENCORES_I2C_0_BASE,DATA_FORMAT,0x07);
	usleep(100000);
	
	//Lecture DATA_FORMAT
	alt_printf("Lecture DATA_FORMAT\n");
	alt_printf("DATA_FORMAT = %x\n\n",lecture_i2c(OPENCORES_I2C_0_BASE,DATA_FORMAT));
	
	A0 = X0;
	A1 = X1;
	
}

//Fonction qui gere l'interruption avec le key
static void key_interrupt(void *Context, alt_u32 id){
	
	alt_printf("INTERRUPT Key\n\n");
	
	switch(A0){
		case X0:
			alt_printf("switch sur Y\n\n");
			A0 = Y0;
			A1 = Y1;
			break;
		case Y0:
			alt_printf("switch sur Z\n\n");
			A0 = Z0;
			A1 = Z1;
			break;
		case Z0:
			alt_printf("switch sur X\n\n");
			A0 = X0;
			A1 = X1;
			break;
		default:
			alt_printf("switch sur Z\n\n");
			A0 = X0;
			A1 = X1;
			break;
	}
	
	
	//Reset la detection des boutons
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE,0b1);
}

//Fonction pour l'initialisation de l'interruption du key
void init_key_interrupt(){
	
	// applique un mask 0b11 afin d'activer les boutons
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE,0b1);
	
	// active la detection des boutons
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE,0b1);
	
	if(alt_irq_register(KEY_IRQ,NULL,key_interrupt) != 0){
		alt_printf("Erreur creation interruption pour key\n");
	}
	
}

//Fonction qui gere l'interruption avec le timer
static void timer_interrupt(void *Context, alt_u32 id){
	
	alt_printf("INTERRUPT timer\n");
	
	affichache_UART();
	affichage_XYZ(A0,A1);
	
	// RESET le bit TO de status pour relancer le timer
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0b01);
}

//Fonction pour l'initialisation de l'interruption du timer
void init_timer_interrupt(){
	if(alt_irq_register(TIMER_0_IRQ,NULL, timer_interrupt) != 0){
		alt_printf("Erreur creation interruption pour le timer\n");
	}
}


int main(){
	alt_printf("\n\n\n\nDans le main\n\n");
	
	//Initialisation de l'i2c
	init_ADXL345();
	
	//Calibration
	calibration();
	
	//Initialisation de l'interruption pour le key
	init_key_interrupt();
	
	//Initialisation de l'interruption pour le timer
	init_timer_interrupt();
	
	
	while(1){
		
	}
}