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
char MyName[8] = "Calle";
char LastName[8] = "Svensson";
char Text1[8] = "Hej";
char Text2[8] = "Jag";
char Text3[8] = "Heter";
char buf_to_read[8];

void main (void) 
{
	
	i2c_init();
	uart_init();
	sei();
	//eeprom_write_byte(0x10, 0b01000011);  // C
	//eeprom_write_byte(0x11, 0b01100001);  // a
	//eeprom_write_byte(0x12, 0b01101100); // l
	//eeprom_write_byte(0x13, 0b01101100); //l
	//eeprom_write_byte(0x14, 0b01100101); //e

	eeprom_write_page(0x20, Text1);
	eeprom_write_page(0x28, Text2);
	eeprom_write_page(0x30, Text3);
	eeprom_write_page(0x38, MyName);
	eeprom_write_page(0x40, LastName);
	
	while (1) 
	{
		eeprom_sequential_read(buf_to_read, 0x20, 40);
		printf_P(PSTR("%x "), buf_to_read);
		_delay_ms(500);
		eeprom_sequential_read(buf_to_read, 0x28, 8);
		printf_P(PSTR("%x"), buf_to_read);
		_delay_ms(500);
		eeprom_sequential_read(buf_to_read, 0x30, 8);
		printf_P(PSTR(" %x"), buf_to_read);
		_delay_ms(500);
		eeprom_sequential_read(buf_to_read, 0x38, 8);
		printf_P(PSTR(" %x"), buf_to_read);
		_delay_ms(500);
		eeprom_sequential_read(buf_to_read, 0x40, 8);
		printf_P(PSTR(" %x\n"), buf_to_read);
		_delay_ms(500);

	}
}

