/*
 * services.h
 *
 *  Created on: May 21, 2020
 *      Author: venkatadineshjakkampudi
 */

#ifndef INC_SERVICES_H_
#define INC_SERVICES_H_

#include "bluenrg_aci_const.h"

tBleStatus addNucleoService(void);
tBleStatus addPbService(void);
void update_data(uint16_t);

#endif /* INC_SERVICES_H_ */
