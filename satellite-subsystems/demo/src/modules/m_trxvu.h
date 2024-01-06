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
#include <satellite-subsystems/IsisTRXVU.h>

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

/**
 * Set bitrate for AX25
 * @param bitrate
 * @return success or failure
 */
Boolean trxvu_set_ax25_bitrate(ISIStrxvuBitrate bitrate);

/**
 * Send a message using default to/from callsigns
 * @param data	message data
 * @param length message length
 * @return
 */
Boolean trxvu_send_message(unsigned char const* data, unsigned short length);

/**
 * Retrieve the number of telecommand frames present in the receive buffer of the TRXVU.
 * @return number of frames
 */
int trxv_get_incoming_frames(void);

/**
 * Retrieve and delete a telecommand frame from the TRXVU.
 * @param frame Received frame structure
 * @return TRUE in success
 */
Boolean trxvu_get_frame(ISIStrxvuRxFrame* frame);

#endif /* MODULES_M_TRXVU_H_ */
