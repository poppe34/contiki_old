/*
 * rf233-commands.h
 *
 *  Created on: Apr 8, 2014
 *      Author: poppe
 */

#ifndef RF233_COMMANDS_H_
#define RF233_COMMANDS_H_


/*---------------------------------------------------------------------------*/
void
rf233_set_pan_addr(unsigned pan,
                    unsigned addr,
                    const uint8_t ieee_addr[8]);

/*---------------------------------------------------------------------------*/
void
rf233_set_channel(uint8_t c);

/*---------------------------------------------------------------------------*/
uint8_t
rf233_get_channel(void);
/*---------------------------------------------------------------------------*/
void
rf233_set_txpower(uint8_t power);
/*---------------------------------------------------------------------------*/
void
rf233_waitidle(void);

/*---------------------------------------------------------------------------*/
char
rf233_isidle(void);

/*---------------------------------------------------------------------------*/
void
rf233_set_txpower(uint8_t power);

/*---------------------------------------------------------------------------*/
void
set_txpower(uint8_t power);

/*---------------------------------------------------------------------------*/
uint8_t
rf233_get_txpower(void);

/*---------------------------------------------------------------------------*/
uint16_t
rf233_get_panid(void);
#endif /* RF233_COMMANDS_H_ */