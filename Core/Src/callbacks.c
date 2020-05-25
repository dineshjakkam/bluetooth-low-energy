/*
 * callbacks.c
 *
 *  Created on: May 22, 2020
 *      Author: venkatadineshjakkampudi
 */

#include "callbacks.h"
#include "services.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"

#include<stdint.h>
#include<stdbool.h>
#include<stdlib.h>

static uint8_t CONNECTED = FALSE;
static uint16_t connection_handle = 0;
extern uint16_t myCharHandle;

void cb_on_gap_connection_complete(uint8_t *, uint16_t);
void cb_on_gap_disconnection_complete(void);
void cb_on_read_request(uint16_t);

void cb_on_gap_connection_complete(uint8_t peer_addr[], uint16_t handle){
	CONNECTED = TRUE;
	connection_handle = handle;
}

void cb_on_gap_disconnection_complete(void){
	//
}

void cb_on_read_request(uint16_t handle){
	if(handle==(myCharHandle+1)){
		uint16_t data = (rand()%100);
		update_data(data);
	}
	if(connection_handle!=0){
		aci_gatt_allow_read(connection_handle);
	}
}
