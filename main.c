#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "serial.h"
#include "timer.h"


uint8_t dataFromEeprom;


void main (void) 
{

	i2c_init();
	uart_init();

	sei();
	eeprom_write_byte(0x10, 0b01000011);  // C
	eeprom_write_byte(0x11, 0b01100001);  // a
	eeprom_write_byte(0x12, 0b01101100); // l
	eeprom_write_byte(0x13, 0b01101100); //l
	eeprom_write_byte(0x14, 0b01100101); //e
	while (1) 
	{
		//i2c_start();
		//i2c_meaningful_status(i2c_get_status());
		//printf_P(PSTR("MAIN 1\n"));
		////_delay_ms(100);	

		//i2c_emit_addr(address, I2C_W);
		//i2c_meaningful_status(i2c_get_status());
		//printf_P(PSTR("MAIN 2\n"));
		////_delay_ms(100);
		//
		//i2c_emit_byte(0x69);
		//i2c_meaningful_status(i2c_get_status());
		//printf_P(PSTR("MAIN 3\n"));
		////_delay_ms(100);

		//i2c_emit_byte(0b00100001);
		//i2c_meaningful_status(i2c_get_status());
		//printf_P(PSTR("MAIN 4\n"));
		////_delay_ms(100);
		//i2c_stop();

		//
		//eeprom_wait_until_write_complete();
	

		dataFromEeprom = eeprom_read_byte(0x10); 
		printf_P(PSTR("%c"), dataFromEeprom);
		dataFromEeprom = eeprom_read_byte(0x11); 
		printf_P(PSTR("%c"), dataFromEeprom);
		dataFromEeprom = eeprom_read_byte(0x12); 
		printf_P(PSTR("%c"), dataFromEeprom);
		dataFromEeprom = eeprom_read_byte(0x13); 
		printf_P(PSTR("%c"), dataFromEeprom);
		dataFromEeprom = eeprom_read_byte(0x14); 
		printf_P(PSTR("%c\n"), dataFromEeprom);
		
		//i2c_emit_addr(addr, I2C_R);
		//i2c_meaningful_status(i2c_get_status());
		//printf_P(PSTR("4\n"));
		//_delay_ms(100);

		//dataFromEeprom = i2c_read_ACK();
		//i2c_meaningful_status(i2c_get_status());
		//printf_P(PSTR("From EEPROM: "));
		//printf_P(PSTR("%c\n"), dataFromEeprom);
		//_delay_ms(100);
	}
}

