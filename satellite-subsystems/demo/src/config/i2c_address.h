/*
 * i2c_address.h
 *
 *  Created on: Dec 22, 2023
 *      Author: tryfinally
 */

#ifndef I2C_ADDRESS_H_
#define I2C_ADDRESS_H_

enum {
	I2C_SPEED	= 100000
};


enum {
	GOM_EPS_ADDRESS	  =		0x02,	///< I2C address of GOMSpace EPS
	ANTENNA_A_ADDRESS =		0x31,	///< I2C address of the ANTENNA Side A
	ANTENNA_B_ADDRESS =		0x32,	///< I2C address of the ANTENNA Side B
	TRXVU_RC_ADDRESS  =		0x60,	///< I2C address of the VU_RC
	TRXVU_TC_ADDRESS  =		0x61,	///< I2C address of the VU_TC
};

#endif /* I2C_ADDRESS_H_ */
