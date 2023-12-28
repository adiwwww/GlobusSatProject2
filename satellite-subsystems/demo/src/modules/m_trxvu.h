/*
 * m_trxvu.h
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#ifndef MODULES_M_TRXVU_H_
#define MODULES_M_TRXVU_H_

#include <stdint.h>
#include <hal/boolean.h>

#define M_TRXVU_RX_FRAME_SIZE		(30)
#define M_TRXVU_TX_FRAME_SIZE		(235)

/**
 * Initialize the TRXVU component
 * @return TRUE on success
 */
Boolean m_trxvu_init(void);

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

#endif /* MODULES_M_TRXVU_H_ */
