#ifndef __MATRIX_KEYPAD_H
#define __MATRIX_KEYPAD_H

#include "main.h"

/**
 * Matrix keypad driver - row drive / column read
 *
 * Pin map:
 *   Rows: R1=PB5, R2=PB4, R3=PB3, R4=PA15
 *   Cols: C1=PB6, C2=PB7, C3=PB8, C4=PB9
 */

// 矩阵键盘配置
#define MATRIX_ROWS 4
#define MATRIX_COLS 4
#define MATRIX_KEYS (MATRIX_ROWS * MATRIX_COLS)

// 按键值定义 (4x4矩阵键盘，返回0-15表示按键位置)
#define MATRIX_KEY_NONE 0

// 按键映射 (可根据实际键盘修改)
// 标准4x4矩阵键盘布局：
//  1  2  3  A
//  4  5  6  B
//  7  8  9  C
//  *  0  #  D

typedef enum {
    MATRIX_KEY_1   = 16,
    MATRIX_KEY_2   = 15,
    MATRIX_KEY_3   = 14,
    MATRIX_KEY_A   = 13,
    MATRIX_KEY_4   = 12,
    MATRIX_KEY_5   = 11,
    MATRIX_KEY_6   = 10,
    MATRIX_KEY_B   = 9,
    MATRIX_KEY_7   = 8,
    MATRIX_KEY_8   = 7,
    MATRIX_KEY_9   = 6,
    MATRIX_KEY_C   = 5,
    MATRIX_KEY_STAR= 4,   // *
    MATRIX_KEY_0   = 3,
    MATRIX_KEY_HASH= 2,   // #
    MATRIX_KEY_D   = 1
} MatrixKeyCode;

// 函数声明
void Matrix_Keypad_Init(void);
uint8_t Matrix_Keypad_Scan(void);
uint8_t Matrix_Keypad_GetKey(void);
char Matrix_Keypad_GetChar(uint8_t key);

#endif /* __MATRIX_KEYPAD_H */
