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
#include "main.h"

charactFormat charFormat;

const uint8_t service_uuid_pb[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xdf, 0xf2, 0x73, 0xd9};
const uint8_t service_uuid[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe0, 0xf2, 0x73, 0xd9};
const uint8_t char_uuid_pb[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe1, 0xf2, 0x73, 0xd9};
const uint8_t char_uuid_led[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe2, 0xf2, 0x73, 0xd9};
const uint8_t char_uuid_led_status[16] = {0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e, 0xe2, 0x11, 0x9e, 0xb1, 0xe3, 0xf2, 0x73, 0xd9};
const uint8_t char_desc_uuid[2] = {0x12, 0x34};

static uint16_t nucleoServHandle, pbServHandle, pbCharHandle, ledCharHandle;
static uint16_t ledStatusCharHandle, myCharDescHandle, connectionHandle;

volatile static uint8_t LED_STATUS = 0;
volatile static uint8_t NOTIFICATION_PENDING = FALSE;

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
			1,
			&ledStatusCharHandle);

	//characteristic that toggles LED on write from client
	ret = aci_gatt_add_char(nucleoServHandle,
			UUID_TYPE_128,
			char_uuid_led,
			20,
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
			20,
			CHAR_PROP_NOTIFY,
			ATTR_PERMISSION_NONE,
			0,
			16,
			1,
			&pbCharHandle);

	return ret;
}


/*
 *  @brief set/reset connection handle on successful completion
 *  		of either the events. Will be called from call_backs.c
 *  @param handle Handle to the connection on successful connection
 *  				0 on successful disconnection
 */
void set_connection_handle(uint16_t handle){
	connectionHandle = handle;
}


/*
 * @brief Set the flag to true on push button pressed
 * 			so that the notification will be sent out
 * 			on next available slot
 */
void set_notification_pending(void){
	NOTIFICATION_PENDING = TRUE;
}


/*
 *  @brief return connection handle cached on successful
 *  		connection setup
 *  @retvalue connection handle
 */
uint16_t get_connection_handle(void){
	return connectionHandle;
}


/*
 * @brief Checks if the attribute change is corresponding to notification
 * 			characteristic
 * @param handle Handle corresponding to PB notification
 * @retvalue returns bool corresponding to comparison
 */
bool is_pb_notification_attribute(uint16_t handle){
	return (handle == (pbCharHandle+2));
}


/*
 * @brief Checks if the attribute change is corresponding to write
 * 			characteristic
 * @param handle Handle corresponding to write property
 * @retvalue returns bool corresponding to comparison
 */
bool is_led_control_attribute(uint16_t handle){
	return (handle == (ledCharHandle+1));
}


/*
 * @brief Checks if the characteristic is corresponding to LED
 * 			status read property
 * @param handle Handle corresponding to the property
 * @retvalue returns bool corresponding to comparison
 */
bool is_led_status_read_charac(uint16_t handle){
	return (handle == (ledStatusCharHandle+1));
}

/*
 * @brief Update the current LED status on request from the
 * 			appropriate characteristic and service
 * 	@param serv_handle Service handle
 * 	@param charc_handle Corresponding characteristic handle
 */
void update_current_led_status(uint16_t serv_handle, uint16_t charc_handle){
	if(is_connected())
		aci_gatt_update_char_value(serv_handle, charc_handle, 0, 1, (uint8_t *)&LED_STATUS);
}

/*
 *  @brief Send out notification on push button press
 */
void send_notification(void){
	if(is_notification_enabled() && NOTIFICATION_PENDING){
		update_current_led_status(pbServHandle, pbCharHandle);
		NOTIFICATION_PENDING = FALSE;
	}
}

/*
 * @brief Updates current LED status on request from client
 *
 */
void service_read_request(void){
	update_current_led_status(nucleoServHandle, ledStatusCharHandle);
}


/*
 * @brief Change the LED status as set by the client
 * @param len Len of the data received from client
 * @param data[] The array holding the data
 * @retvalue None
 */
void change_led_state(uint16_t len, uint8_t data[]){
	LED_STATUS = data[0];
	HAL_GPIO_WritePin(GreenLED_GPIO_Port, GreenLED_Pin, LED_STATUS);
}




