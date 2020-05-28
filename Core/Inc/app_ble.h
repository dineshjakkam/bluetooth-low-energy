/*
 * app_ble.h
 *
 *  Created on: May 25, 2020
 *      Author: venkatadineshjakkampudi
 */

#ifndef INC_APP_BLE_H_
#define INC_APP_BLE_H_

#include "hci_const.h"

void MX_BlueNRG_MS_Init(void);
void MX_BlueNRG_MS_Process(void);
void event_user_notify(void *);
void set_connectable_status(void);
void reset_connectable_status(void);
tBleStatus establish_connection(void);


#endif /* INC_APP_BLE_H_ */
