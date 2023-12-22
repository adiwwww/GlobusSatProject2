#ifndef UTILS_RESPONDER_H_
#define UTILS_RESPONDER_H_

/*
 * responder.h
 *
 *  Created on: Dec 22, 2023
 *      Author: tryfinally
 */

#include <stdint.h>
#include <hal/boolean.h>

/**
 * Activate ISIS TRXVU Responder mode
 */
Boolean trxvu_activate_responder(void);

/**
 * Deactivate ISIS TRXVU Responder mode
 */
Boolean trxvu_deactivate_responder(void);

/**
 * Set RSSI Transponder Threshold
 * @param threshold RSSI threshold value. between 0 and 4095.
 * @return
 */
Boolean trxvu_set_responder_rssi_threshold(uint16_t rssi);

#endif /* UTILS_RESPONDER_H_ */
