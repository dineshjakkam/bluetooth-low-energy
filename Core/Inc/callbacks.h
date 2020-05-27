/*
 * callbacks.h
 *
 *  Created on: May 22, 2020
 *      Author: venkatadineshjakkampudi
 */

#ifndef INC_CALLBACKS_H_
#define INC_CALLBACKS_H_

#include<stdint.h>

void cb_on_gap_connection_complete(uint8_t *, uint16_t);
void cb_on_gap_disconnection_complete(void);
void cb_on_read_request(uint16_t);
uint8_t is_connected(void);

#endif /* INC_CALLBACKS_H_ */
