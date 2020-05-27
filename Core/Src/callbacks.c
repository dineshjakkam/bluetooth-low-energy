/*
 * callbacks.c
 *
 *  Created on: May 22, 2020
 *      Author: venkatadineshjakkampudi
 */

#include "callbacks.h"
#include "services.h"
#include "app_ble.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"

#include<stdint.h>
#include<stdbool.h>
#include<stdlib.h>

static volatile uint8_t CONNECTED = FALSE;
static uint16_t connection_handle = 0;
extern uint16_t ledCharHandle;

void cb_on_gap_connection_complete(uint8_t *, uint16_t);
void cb_on_gap_disconnection_complete(void);
void cb_on_read_request(uint16_t);
void cb_on_attribute_modified(uint16_t, uint16_t, uint8_t []);
uint8_t is_connected(void);

/*
 * @brief Call back called on successful connection complete
 * @param peer_addr An array that contains peer address
 * @param handle connection handle
 */
void cb_on_gap_connection_complete(uint8_t peer_addr[], uint16_t handle){
	CONNECTED = TRUE;
	connection_handle = handle;
	reset_connectable_status();
}

void cb_on_gap_disconnection_complete(void){
	CONNECTED = FALSE;
	connection_handle = 0;
	set_connectable_status();
}

void cb_on_read_request(uint16_t handle){
	if(handle==(ledCharHandle+1)){
		uint16_t data = (rand()%100);
		update_data(data);
	}
	if(CONNECTED && connection_handle!=0){
		aci_gatt_allow_read(connection_handle);
	}
}

/*
 * @ brief return the status of connection
 * @retvalue True if connected, else False
 */
uint8_t is_connected(void){
	return CONNECTED;
}


/*
 * @brief set the LED depending on value written by the mobile device
 * @param handle The characteristic handle to compare against
 * @param len The length of the data arrived at radio
 * @param data The data arrived
 * @retvalue None
 */
void cb_on_attribute_modified(uint16_t handle, uint16_t len, uint8_t data[]){

}
