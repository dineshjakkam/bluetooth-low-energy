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
const uint8_t char_desc_uuid[2] = {0x12, 0x34};

uint16_t myServHandle, myCharHandle, myCharDescHandle;
charactFormat charFormat;

/*
 * @brief defines a service with the char and corresponding descriptors
 * @retvalue status of success
 */
tBleStatus addSimpleService(void){
	tBleStatus ret;

	aci_gatt_add_serv(UUID_TYPE_128,
			service_uuid,
			PRIMARY_SERVICE,
			0x07,
			&myServHandle);

	aci_gatt_add_char(myServHandle,
			UUID_TYPE_128,
			char_uuid,
			2,
			CHAR_PROP_READ,
			ATTR_PERMISSION_NONE,
			GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
			16,
			0,
			&myCharHandle);

	charFormat.format = FORMAT_SINT16;
	charFormat.exp = -1;
	charFormat.unit = UNIT_TEMP_CELSIUS;
	charFormat.name_space = 0;
	charFormat.desc = 0;

	ret = aci_gatt_add_char_desc(myServHandle,
			myCharHandle,
			UUID_TYPE_16,
			(uint8_t *)&char_desc_uuid,
			7,
			7,
			(void *)&charFormat,
			ATTR_PERMISSION_NONE,
			ATTR_ACCESS_READ_ONLY,
			0,
			16,
			FALSE,
			&myCharDescHandle);

	return ret;
}

/*
 * @brief updates data in the character
 * @param newData The data to update to
 */
void update_data(uint16_t newData){
	tBleStatus ret;
	ret = aci_gatt_update_char_value(myServHandle, myCharHandle, 0, 2, (uint8_t *)&newData);
}



