/*
 * services.h
 *
 *  Created on: May 21, 2020
 *      Author: venkatadineshjakkampudi
 */

#ifndef INC_SERVICES_H_
#define INC_SERVICES_H_

#include "bluenrg_aci_const.h"
#include <stdint.h>
#include <stdbool.h>

void read_data(uint8_t *, uint8_t);
void set_notification_pending(void);
void set_connection_handle(uint16_t);
void send_notification(void);
void update_data(uint16_t);
void service_read_request(void);
void change_led_state(uint16_t, uint8_t []);

tBleStatus addNucleoService(void);
tBleStatus addPbService(void);

uint16_t get_connection_handle(void);

bool is_led_control_attribute(uint16_t);
bool is_pb_notification_attribute(uint16_t);
bool is_led_status_read_charac(uint16_t);

#endif /* INC_SERVICES_H_ */
