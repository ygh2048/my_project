#include <stdint.h>
#include <stdio.h>

/**
 * @brief 生成CRC8查找表
 * @param polynomial: CRC8多项式
 * @param table: 存储生成的CRC8表的数组（256字节）
 * @param refin: 输入数据是否反转
 * @param refout: 输出结果是否反转
 * @param xorout: 输出异或值
 */
void generate_crc8_table(uint8_t polynomial, uint8_t table[256], 
                        bool refin, bool refout, uint8_t xorout)
{
    for (uint16_t i = 0; i < 256; i++) {
        uint8_t crc = (refin) ? reverse_byte(i) : i;
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
        
        if (refout) {
            crc = reverse_byte(crc);
        }
        
        crc ^= xorout;
        table[i] = crc;
    }
}

/**
 * @brief 反转一个字节的位顺序
 * @param byte: 要反转的字节
 * @return 反转后的字节
 */
uint8_t reverse_byte(uint8_t byte)
{
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

/**
 * @brief 使用生成的表计算CRC8
 * @param data: 数据指针
 * @param len: 数据长度
 * @param table: CRC8查找表
 * @param init: 初始CRC值
 * @return 计算得到的CRC8值
 */
uint8_t calculate_crc8_with_table(uint8_t *data, uint16_t len, 
                                 const uint8_t table[256], uint8_t init)
{
    uint8_t crc = init;
    for (uint16_t i = 0; i < len; i++) {
        crc = table[crc ^ data[i]];
    }
    return crc;
}