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
#include "callbacks.h"

#include<stdint.h>

const uint8_t service_uuid_pb[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xdf, 0xf2, 0x73, 0xd9};
const uint8_t service_uuid[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe0, 0xf2, 0x73, 0xd9};
const uint8_t char_uuid_pb[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe1, 0xf2, 0x73, 0xd9};
const uint8_t char_uuid_led[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe2, 0xf2, 0x73, 0xd9};
const uint8_t char_uuid_led_status[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe3, 0xf2, 0x73, 0xd9};
const uint8_t char_desc_uuid[2] = {0x12, 0x34};

uint16_t nucleoServHandle, pbServHandle, pbCharHandle, ledCharHandle, ledStatusCharHandle, myCharDescHandle;
static uint8_t LED_STATUS = 0;
charactFormat charFormat;

/*
 * @brief defines a service with the char and corresponding descriptors
 * @retvalue status of success
 */
tBleStatus addNucleoService(void){
	tBleStatus ret;

	aci_gatt_add_serv(UUID_TYPE_128,
			service_uuid,
			PRIMARY_SERVICE,
			0x07,
			&nucleoServHandle);

	//characteristic to read led status
	aci_gatt_add_char(nucleoServHandle,
			UUID_TYPE_128,
			char_uuid_led_status,
			2,
			CHAR_PROP_READ,
			ATTR_PERMISSION_NONE,
			GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
			16,
			0,
			&ledStatusCharHandle);

	//characteristic that toggles LED on write from client
	ret = aci_gatt_add_char(nucleoServHandle,
			UUID_TYPE_128,
			char_uuid_led,
			2,
			CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
			ATTR_PERMISSION_NONE,
			GATT_NOTIFY_ATTRIBUTE_WRITE,
			16,
			0,
			&ledCharHandle);


	charFormat.format = FORMAT_SINT16;
	charFormat.exp = -1;
	charFormat.unit = UNIT_UNITLESS;
	charFormat.name_space = 0;
	charFormat.desc = 0;

	ret = aci_gatt_add_char_desc(nucleoServHandle,
			ledCharHandle,
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
 * @brief The service that handles the push button interrupt
 * `		Notifies the state of LED on PB press
 */
tBleStatus addPbService(void){
	tBleStatus ret;
	ret = aci_gatt_add_serv(UUID_TYPE_128,
			service_uuid_pb,
			PRIMARY_SERVICE,
			0x07,
			&pbServHandle);

	//characteristic that send notification on PB press
	ret = aci_gatt_add_char(pbServHandle,
			UUID_TYPE_128,
			char_uuid_pb,
			2,
			CHAR_PROP_NOTIFY,
			ATTR_PERMISSION_NONE,
			0,
			16,
			0,
			&pbCharHandle);

	return ret;
}

/*
 * @brief updates data in the character
 * @param newData The data to update to
 */
void update_data(uint16_t newData){
	aci_gatt_update_char_value(nucleoServHandle, ledCharHandle, 0, 2, (uint8_t *)&newData);
}

/*
 * @brief This is call back called through interrupt on push button pressed
 * 			On PB pressed notify the client through notification characteristic
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(is_connected()){
		aci_gatt_update_char_value(pbServHandle, pbCharHandle, 0, 1, (uint8_t *)&LED_STATUS);
	}
}



