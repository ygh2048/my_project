/*
 * bsp_agreement.h
 *
 *  Created on: Jul 21, 2024
 *      Author: ygh20
 */

#ifndef BSP_AGREEMENT_H_
#define BSP_AGREEMENT_H_

#include "bsp.h"
#include "nclink.h"




void unpack_process(UART_RX_TypeDef *pRecvUartData);
void unpack_process_gh(UART_RX_TypeDef *pRecvUartData);
void print3dpos(void);

void speaker_gh(float x,float y,float z,int16_t q0,int16_t q1,int16_t q2,int16_t q3);

#endif /* BSP_AGREEMENT_H_ */
