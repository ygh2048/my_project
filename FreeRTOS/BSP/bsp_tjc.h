/*
 * bsp_tjc.h
 *
 *  Created on: Jul 23, 2024
 *      Author: ygh20
 */

#ifndef BSP_TJC_H_
#define BSP_TJC_H_

#include "bsp.h"

#define STR_LENGTH 100

void refresh(float x,float y,float z);
void refresh_new(float x,float y,float z);

void TJCPrintf(const char *str, ...);


#endif /* BSP_TJC_H_ */
