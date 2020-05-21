/*
 * services.c
 *
 *  Created on: May 21, 2020
 *      Author: venkatadineshjakkampudi
 */

#include "app_ble.h"
#include "bluenrg_gap.h"
#include "hci_const.h"
#include "bluenrg_hal_aci.h"
#include "hci.h"
#include "hci_le.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_utils.h"
#include "services.h"

#include<stdint.h>

const uint8_t service_uuid[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe0, 0xf2, 0x73, 0xd9};
const uint8_t char_uuid[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe1, 0xf2, 0x73, 0xd9};

uint16_t myServHandle, myCharHandle;


tBleStatus addSimpleService(void){
	tBleStatus ret;
	aci_gatt_add_serv(UUID_TYPE_128,
			service_uuid,
			PRIMARY_SERVICE,
			0x07,
			&myServHandle);

	ret = aci_gatt_add_char(myServHandle,
			UUID_TYPE_128,
			char_uuid,
			1,
			CHAR_PROP_NOTIFY,
			ATTR_PERMISSION_NONE,
			0,
			16,
			0,
			&myCharHandle);

	return ret;
}

