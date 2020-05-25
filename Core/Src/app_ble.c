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

#include <stdint.h>
#include <stdbool.h>

#define BDADDR_SIZE 6

static bool CONNECTABLE = TRUE;

void MX_BlueNRG_MS_Init(void);
void MX_BlueNRG_MS_Process(void);
void event_user_notify(void *);

/* @brief BlueNRG-MS initialization
 * @retvalue None
 *
 */
void MX_BlueNRG_MS_Init(void){
	tBleStatus ret;
	const char* name = "Jakkampudi";
	uint8_t server_bdaddr[] = {0x01,0x02,0x03,0x04,0x05,0x06};
	uint8_t bdaddr[BDADDR_SIZE];

	uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

	hci_init(&event_user_notify, NULL);
	hci_reset();
	HAL_Delay(100);

	BLUENRG_memcpy(bdaddr, server_bdaddr, sizeof(server_bdaddr));

	aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
			CONFIG_DATA_PUBADDR_LEN,
			bdaddr);

	//Bluet0oth chip that I have is IDB05A1
	aci_gatt_init();
	aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, strlen(name), &service_handle, &dev_name_char_handle, &appearance_char_handle);
	aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
			strlen(name), (uint8_t *)name);

	//Initialize services here
	addNucleoService();
	addPbService();
}

/*
 *  @brief BlueNRG-MS background task
 *  @retvalue None
 */
void MX_BlueNRG_MS_Process(void){

	if(CONNECTABLE){
		establish_connection();
	}
	hci_user_evt_proc();
}

/*
 * @brief set connectable status on disconnection complete
 */
void set_connectable_status(void){
	CONNECTABLE = TRUE;
}

/*
 * @brief reset connectable status on connection complete
 */
void reset_connectable_status(void){
	CONNECTABLE = FALSE;
}

/*
 * Establish connection by indirect advertising in discoverable mode
 */
tBleStatus establish_connection(void){
	tBleStatus ret;
	const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME, 'D', 'i', 'n', 'e', 's', 'h', '-', 'L', 'a', 'b'};

	hci_le_set_scan_resp_data(0, NULL);

	ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR,
			NO_WHITE_LIST_USE, sizeof(local_name), local_name, 0,
			NULL, 0, 0);
	return ret;
}

/*
 * @brief This is state machine that forwards data to appropriate call-backs
 * 		  depending on the data arrived at hci
 * 	@param *pData The pointer to the buffer that has data arrived at hci
 */
void event_user_notify(void *pData){
	hci_uart_pckt *hci_pkt = pData;
	hci_event_pckt *hci_evt_pkt = (void *)hci_pkt->data;

	if(hci_pkt->type != HCI_EVENT_PKT)
		return;

	switch(hci_evt_pkt->evt){
		case EVT_DISCONN_COMPLETE:
		{
			cb_on_gap_disconnection_complete();
		}
		break;
		case  EVT_LE_META_EVENT:
		{
			evt_le_meta_event *hci_meta_evt = (void *)hci_evt_pkt->data;
			switch(hci_meta_evt->subevent){
				case EVT_LE_CONN_COMPLETE:
				{
					evt_le_connection_complete *hci_con_comp_evt = (void *)hci_meta_evt->data;
					cb_on_gap_connection_complete(hci_con_comp_evt->peer_bdaddr, hci_con_comp_evt->handle);
				}
				break;
			}
		}
		break;
		case EVT_VENDOR:
		{
			evt_blue_aci *vendor_evt = (void *)hci_evt_pkt->data;
			switch(vendor_evt->ecode){
				case EVT_BLUE_GATT_READ_PERMIT_REQ:
				{
					evt_gatt_read_permit_req *read_pmt_req_evt = (void *)vendor_evt->data;
					cb_on_read_request(read_pmt_req_evt->attr_handle);
				}
				break;
				case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
				{
					evt_gatt_attr_modified_IDB05A1 *attr_modified_evt = (void *)vendor_evt->data;
					cb_on_attribute_modified(attr_modified_evt->attr_handle,
							attr_modified_evt->data_length,
							attr_modified_evt->att_data);
				}
			}
		}
		break;
	}
}
