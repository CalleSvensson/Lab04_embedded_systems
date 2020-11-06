#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"



void i2c_init(void) 
{
	/*set SCL to 100kHz     
	SCL frequency = CPU Clock frequency/ 16+2(TWBR)*(PrescalerValue)
	100 000 = 16 000 000 / 16+ 2TWBR * 1
	16 + 2TWBR = 16 000 000 /100 000
	2TWBR = 160 - 16
	2TWBR/2 = 144/2
	TWBR = 72
	
	*/
	TWSR = 0x00; // prescaler 1
	TWBR = 0x48; // 72

	TWCR = (1 << TWEN);  // enable TWI two wire interface
}

void i2c_meaningful_status(uint8_t status) 
{
	switch (status) {
		case 0x08: // START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("START\n"));
			break;
		case 0x10: // repeated START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("RESTART\n"));
			break;
		case 0x38: // NAK or DATA ARBITRATION LOST
			printf_P(PSTR("NOARB/NAK\n"));
			break;
		// MASTER TRANSMIT
		case 0x18: // SLA+W transmitted, ACK received
			printf_P(PSTR("MT SLA+W, ACK\n"));
			break;
		case 0x20: // SLA+W transmitted, NAK received
			printf_P(PSTR("MT SLA+W, NAK\n"));
				break;
		case 0x28: // DATA transmitted, ACK received
			printf_P(PSTR("MT DATA+W, ACK\n"));
			break;
		case 0x30: // DATA transmitted, NAK received
			printf_P(PSTR("MT DATA+W, NAK\n"));
			break;
		// MASTER RECEIVE
		case 0x40: // SLA+R transmitted, ACK received
			printf_P(PSTR("MR SLA+R, ACK\n"));
			break;
		case 0x48: // SLA+R transmitted, NAK received
			printf_P(PSTR("MR SLA+R, NAK\n"));
			break;
		case 0x50: // DATA received, ACK sent
			printf_P(PSTR("MR DATA+R, ACK\n"));
			break;
		case 0x58: // DATA received, NAK sent
			printf_P(PSTR("MR DATA+R, NAK\n"));
			break;
		default:
			printf_P(PSTR("N/A %02X\n"), status);
			break;
	}
}

inline void i2c_start() 
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition
	while (!(TWCR & (1 << TWINT))); //Wait for TWINT Flag 
}

inline void i2c_stop() 
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); //Send STOP condtion
	while ((TWCR & (1 << TWSTO)));  // wait for stop
}

inline uint8_t i2c_get_status(void) 
{
	uint8_t status;	
	status = (TWSR & 0xF8); //mask status  1111 1000  mask away the prescaler bits
	return status;
}

inline void i2c_emit_addr(uint8_t address, uint8_t rw) 
{
	TWDR = (address & 0xfe) | (rw & 0x01); // Set EEPROM address and Write or Read bit.
	TWCR = (1 << TWINT) | (1 << TWEN); //clear interrupt to start transmission 
	while (!(TWCR & (1 << TWINT)));  // flag
}

inline void i2c_emit_byte(uint8_t data) 
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);//clear interrupt to start transmission 
	while (!(TWCR & (1 << TWINT)));  // flag
}

inline uint8_t i2c_read_ACK() 
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // TWEA ack bit
	while (!(TWCR & (1 << TWINT))); // wait for flag
	return TWDR; // return read data
}

inline uint8_t i2c_read_NAK() 
{
	TWCR = (1 << TWINT) | (1 << TWEN); // no ack bit
	while (!(TWCR & (1 << TWINT))); // wait for flag
	return TWDR; // return read data
}

inline void eeprom_wait_until_write_complete() 
{
	//ACKNOWLEDGE POLLING
	while(i2c_get_status() != 0x18) // Wait for Master Transmit SLA+W transmitted, ACK received
	{
	i2c_start();
	i2c_emit_addr(0xA0, I2C_W);
	}
}

uint8_t eeprom_read_byte(uint8_t addr) 
{
	uint8_t data = 0;
	i2c_start();
	
	i2c_emit_addr(EEPROM_address, I2C_W); // write
	
	i2c_emit_byte(addr);  // set eeprom memory address 
	
	i2c_start();
	
	i2c_emit_addr(EEPROM_address, I2C_R); // read

	data = i2c_read_NAK();
	i2c_stop();

	return data;
}

void eeprom_write_byte(uint8_t addr, uint8_t data) 
{
	i2c_start();

	i2c_emit_addr(EEPROM_address, I2C_W);

	i2c_emit_byte(addr);

	i2c_emit_byte(data);

	i2c_stop();

	eeprom_wait_until_write_complete();
}

void eeprom_write_page(uint8_t addr, uint8_t *data) 
{
	// ... (VG)
	/* check start at addresse is not integer multiples of the page buffer size (8)*/
	while (addr % 8 != 0)
	{
		/*check if closest integer multipels of 8 is up or down*/
		if (addr % 8 > 4) addr++;
		else addr--;
	}
	i2c_start(); // start
	i2c_emit_addr(EEPROM_address, I2C_W); 
	i2c_emit_byte(addr); 
	int i;
	for (i = 0; i < 8; i++)
	{
	i2c_emit_byte(data[i]); // write byte for byte;
	}

	i2c_stop();

	eeprom_wait_until_write_complete();
}

void eeprom_sequential_read(uint8_t *buf, uint8_t start_addr, uint8_t len) 
{
	// ... (VG)
	while (start_addr % 8 != 0)
	{
		//start_addr++;
		if (start_addr % 8 > 4) start_addr++;
		else start_addr--;
	}
	i2c_start();
	i2c_emit_addr(EEPROM_address, I2C_W); // write
	i2c_emit_byte(start_addr);  // set eeprom memory address 
	i2c_start();
	i2c_emit_addr(EEPROM_address, I2C_R); // read

	int j;
	for (j = 0; j < (len-1); j++)
	{
		buf[j] = i2c_read_ACK(); // send ACK while the byte to read isent the last
	}
	buf[len-1] = i2c_read_NAK(); // Read the last byte and with NAK
	i2c_stop();

}
