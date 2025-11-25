#include "matrix_keypad.h"

// Row: R1=PB5, R2=PB4, R3=PB3, R4=PA15
static const uint16_t row_pins[MATRIX_ROWS] = {
    MATRIX_ROW1_Pin,
    MATRIX_ROW2_Pin,
    MATRIX_ROW3_Pin,
    MATRIX_ROW4_Pin
};
static GPIO_TypeDef* const row_ports[MATRIX_ROWS] = {
    MATRIX_ROW1_GPIO_Port,
    MATRIX_ROW2_GPIO_Port,
    MATRIX_ROW3_GPIO_Port,
    MATRIX_ROW4_GPIO_Port
};

// Column: C1=PB6, C2=PB7, C3=PB8, C4=PB9 (all on GPIOB)
static const uint16_t col_pins[MATRIX_COLS] = {
    MATRIX_COL1_Pin,
    MATRIX_COL2_Pin,
    MATRIX_COL3_Pin,
    MATRIX_COL4_Pin
};
static GPIO_TypeDef* const col_ports[MATRIX_COLS] = {
    MATRIX_COL1_GPIO_Port,
    MATRIX_COL2_GPIO_Port,
    MATRIX_COL3_GPIO_Port,
    MATRIX_COL4_GPIO_Port
};

// Standard 4x4 key map (index 1..16, index 0 unused)
static const char key_chars[MATRIX_KEYS + 1] = {
    '\0',
    'D', '#', '0', '*',   // 1..4
    'C', '9', '8', '7',   // 5..8
    'B', '6', '5', '4',   // 9..12
    'A', '3', '2', '1'    // 13..16
};

// Key state and debounce variables
static uint8_t last_key = MATRIX_KEY_NONE;
static uint8_t key_state = 0;           // 0=released, 1=pressed, 2=held
static uint32_t key_time = 0;           // press timestamp
static uint8_t debounce_count = 0;      // debounce counter

#define DEBOUNCE_COUNT 1                // minimal debounce for fastest response
#define KEY_REPEAT_DELAY 500            // long-press start delay (ms)
#define KEY_REPEAT_RATE 100             // long-press repeat rate (ms)
#define SCAN_SETTLE_CYCLES 2            // short delay after driving a row low (keep tiny for speed)

static void Matrix_SetAllRowsHigh(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        HAL_GPIO_WritePin(row_ports[i], row_pins[i], GPIO_PIN_SET);
    }
}

// Check columns for a low level, return column index (1-4) or 0 if none
static uint8_t Matrix_ReadPressedCol(void)
{
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        if (HAL_GPIO_ReadPin(col_ports[col], col_pins[col]) == GPIO_PIN_RESET) {
            return col + 1;
        }
    }
    return 0;
}

void Matrix_Keypad_Init(void)
{
    last_key = MATRIX_KEY_NONE;
    key_state = 0;
    key_time = 0;
    debounce_count = 0;

    // MX_GPIO_Init already set rows as outputs and cols as inputs; just ensure rows start high
    Matrix_SetAllRowsHigh();
}

/**
 * @brief Fast matrix scan without debounce
 * @return key index (1-16) or MATRIX_KEY_NONE
 * @note  Only toggles row outputs; no GPIO reconfiguration inside the scan
 */
uint8_t Matrix_Keypad_Scan(void)
{
    uint8_t key = MATRIX_KEY_NONE;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_RESET);
        for (volatile uint32_t i = 0; i < SCAN_SETTLE_CYCLES; i++);  // allow signals to settle

        uint8_t col = Matrix_ReadPressedCol();
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_SET);

        if (col != 0U) {
            // 1-based index reversed so物理左上为1
            key = row * MATRIX_COLS + col;
            break;
        }
    }

    return key;
}

/**
 * @brief Get key with debounce and long-press handling
 * @return key index (1-16) or MATRIX_KEY_NONE when no stable key
 * @note  Call every 10-20 ms
 */
uint8_t Matrix_Keypad_GetKey(void)
{
    uint8_t current_key = Matrix_Keypad_Scan();
    uint8_t result = MATRIX_KEY_NONE;

    // Debounce
    if (current_key == last_key) {
        if (debounce_count < DEBOUNCE_COUNT) {
            debounce_count++;
        }
    } else {
        debounce_count = 0;
        last_key = current_key;
    }

    if (debounce_count >= DEBOUNCE_COUNT) {
        if (current_key != MATRIX_KEY_NONE) {
            // Pressed
            if (key_state == 0) {
                key_state = 1;
                key_time = HAL_GetTick();
                result = current_key;
            } else if (key_state == 1) {
                key_state = 2;
            } else if (key_state == 2) {
                uint32_t hold_time = HAL_GetTick() - key_time;
                if (hold_time > KEY_REPEAT_DELAY) {
                    if ((hold_time - KEY_REPEAT_DELAY) % KEY_REPEAT_RATE < 20) {
                        result = current_key;
                    }
                }
            }
        } else {
            // Released
            key_state = 0;
        }
    }

    return result;
}

char Matrix_Keypad_GetChar(uint8_t key)
{
    if (key > 0 && key <= MATRIX_KEYS) {
        return key_chars[key];
    }
    return '\0';
}
