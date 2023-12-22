#ifndef UTILS_RESPONDER_H_
#define UTILS_RESPONDER_H_

/*
 * responder.h
 *
 *  Created on: Dec 22, 2023
 *      Author: tryfinally
 */

#include <hal/boolean.h>

/**
 * Activate ISIS TRXVU Responder mode
 */
Boolean trxvu_activate_responder(void);

/**
 * Deactivate ISIS TRXVU Responder mode
 */
Boolean trxvu_deactivate_responder(void);

#endif /* UTILS_RESPONDER_H_ */
