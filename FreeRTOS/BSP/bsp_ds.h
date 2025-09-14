/*
 * bsp_ds.h
 *
 *  Created on: Jul 30, 2024
 *      Author: ygh20
 */

#ifndef BSP_DS_H_
#define BSP_DS_H_



#include "bsp.h"

//q0 id

#define A1 1
#define A2 2
#define A3 3
#define A6 4
#define A5 5
#define A4 6

#define B6 7
#define B5 8
#define B4 9
#define B1 10
#define B2 11
#define B3 12

#define C1 13
#define C2 14
#define C3 15
#define C6 16
#define C5 17
#define C4 18

#define D6 19
#define D5 20
#define D4 21
#define D1 22
#define D2 23
#define D3 24

#define NOTHING 0

#define false 0
#define true 1

//q1 position
void data_to_array(void);
void choose_s_pos();
int choose_pos(void);

int check_array(void);

void show(void);




#endif /* BSP_DS_H_ */
