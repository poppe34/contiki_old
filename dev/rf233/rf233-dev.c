/*
 * rf233-reg.c
 *
 *  Created on: Apr 2, 2014
 *      Author: poppe
 */
#include "contiki.h"
#include "stdint.h"

#include "rf233.h"

/*----------------------------------------------------------------------------*/
/** \brief  This function reads data from one of the radio transceiver's registers.
 *
 *  \param  address Register address to read from. See datasheet for register
 *                  map.
 *
 *  \see Look at the at86rf230_registermap.h file for register address definitions.
 *
 *  \returns The actual value of the read register.
 */
uint8_t
rf233_register_read(uint8_t address)
{
    uint8_t register_value;
    /* Add the register read command to the register address. */
    address |= 0x80;

    RF233_SPI_TRANSFER_OPEN();
    clock_delay(3);
    /*Send Register address and read register content.*/
    rf233_arch_write(address);
    register_value = rf233_arch_write(0xAA);

    clock_delay(3);

    RF233_SPI_TRANSFER_CLOSE();
    return register_value;
}


void
rf233_register_write(uint8_t address, uint8_t value)
{
    /* Add the Register Write (short mode) command to the address. */
    address = 0xc0 | address;

    RF233_SPI_TRANSFER_OPEN();
    clock_delay(3);
    /*Send Register address and write register content.*/
    rf233_arch_write(address);
    rf233_arch_write(value);

    clock_delay(3);
    RF233_SPI_TRANSFER_CLOSE();
    clock_delay(20);
}

uint8_t
rf233_subregister_read(uint8_t address, uint8_t mask, uint8_t position)
{
    /* Read current register value and mask out subregister. */
    uint8_t register_value = rf233_register_read(address);
    register_value &= mask;
    register_value >>= position; /* Align subregister value. */

    return register_value;
}

void
rf233_subregister_write(uint8_t address, uint8_t mask, uint8_t position,
                            uint8_t value)
{
    /* Read current register value and mask area outside the subregister. */
    uint8_t register_value = rf233_register_read(address);
    register_value &= ~mask;

    /* Start preparing the new subregister value. shift in place and mask. */
    value <<= position;
    value &= mask;

    value |= register_value; /* Set the new subregister value. */

    /* Write the modified register value. */
    rf233_register_write(address, value);
}

/*----------------------------------------------------------------------------*/
/** \brief  Transfer a frame from the radio transceiver to a RAM buffer
 *
 *          This version is optimized for use with contiki RF230BB driver.
 *          The callback routine and CRC are left out for speed in reading the rx buffer.
 *          Any delays here can lead to overwrites by the next packet!
 *
 *          If the frame length is out of the defined bounds, the length, lqi and crc
 *          are set to zero.
 *
 *  \param  rx_frame    Pointer to the data structure where the frame is stored.
 */
void
rf233_frame_read(rf233_buffer_t *rx_frame)
{
	   uint8_t frame_length, *rx_data, status;

	    /*Send frame read (long mode) command.*/
	    RF233_SPI_TRANSFER_OPEN();
	    rf233_arch_write(0x20);

	    /*Read frame length. This includes the checksum. */
	    frame_length = rf233_arch_write(0xAA);

	    /*Check for correct frame length. Bypassing this test can result in a buffer overrun! */
	    if ((frame_length < RF233_MIN_FRAME_LENGTH) || (frame_length > RF233_MAX_FRAME_LENGTH)) {
	        /* Length test failed */
	        rx_frame->len = 0;
	        rx_frame->lqi    = 0;
	        rx_frame->crc    = CRC_BAD;
	    }
	    else {
	        rx_data = (rx_frame->data);
	        rx_frame->len = frame_length;

	        /*Transfer frame buffer to RAM buffer */

	        do{
	            *rx_data++ = rf233_arch_write(0xAA);

	        } while (--frame_length > 0);


	        /*Read LQI value for this frame.*/
	        rx_frame->lqi = rf233_arch_write(0xAA);
	        status = rf233_arch_write(0xAA);
	        /* If crc was calculated set crc field in hal_rx_frame_t accordingly.
	         * Else show the crc has passed the hardware check.
	         */
	        /* FIXME: I need to set the CRC From the packet not an auto true */
	        (void)status;
	        rx_frame->crc   = CRC_OK;
	    }

	    RF233_SPI_TRANSFER_CLOSE();
}

/*----------------------------------------------------------------------------*/
/** \brief  This function will download a frame to the radio transceiver's frame
 *          buffer.
 *
 *  \param  write_buffer    Pointer to data that is to be written to frame buffer.
 *  \param  length          Length of data. The maximum length is 127 bytes.
 */
void
rf233_frame_write(uint8_t *write_buffer, uint8_t length) {

RF233_SPI_TRANSFER_OPEN();

/* Send Frame Transmit (long mode) command and frame length */
rf233_arch_write(0x60);
rf233_arch_write(length);

/* Download to the Frame Buffer.
 * When the FCS is autogenerated there is no need to transfer the last two bytes
 * since they will be overwritten.
 */
#if !RF233_CONF_CHECKSUM
length -= 2;
#endif
do rf233_arch_write(*write_buffer++); while (--length);

RF233_SPI_TRANSFER_CLOSE();
}