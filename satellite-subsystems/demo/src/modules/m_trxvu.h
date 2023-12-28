/*
 * m_trxvu.h
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#ifndef MODULES_M_TRXVU_H_
#define MODULES_M_TRXVU_H_

#include <hal/boolean.h>

Boolean m_trxvu_init(void);

#define M_TRXVU_RX_FRAME_SIZE		(30)
#define M_TRXVU_TX_FRAME_SIZE		(235)

#endif /* MODULES_M_TRXVU_H_ */
