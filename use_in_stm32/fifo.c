#include "fifo.h"

/**
 * @brief 初始化FIFO队列
 * @param fifo: 队列结构体指针
 * @param buffer: 外部提供的缓冲区
 * @param max_size: 队列最大容量（元素个数）
 * @param item_size: 每个元素的大小（字节）
 * @return 初始化成功返回true，失败返回false
 */
bool fifo_init(fifo_t *fifo, void *buffer, size_t max_size, size_t item_size)
{
    // 参数检查
    if (fifo == NULL || buffer == NULL || max_size == 0 || item_size == 0) {
        return false;
    }
    
    // 初始化队列结构体
    fifo->buffer = buffer;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->max_size = max_size;
    fifo->item_size = item_size;

    return true;
}
 


/**
 * @brief 向队列中添加一个元素
 * @param fifo: 队列结构体指针
 * @param item: 要添加的元素指针
 * @return 添加成功返回true，失败返回false
 */
bool fifo_put(fifo_t *fifo, const void *item)
{
    // 参数检查
    if (fifo == NULL || item == NULL) {
        return false;
    }
    
    // 检查队列是否已满
    if (((fifo->tail + 1) % fifo->max_size) == fifo->head) {
        
        // 移动头指针丢弃最旧的数据
        fifo->head = (fifo->head + 1) % fifo->max_size;
    }
    
    // 计算目标位置并复制数据
    uint8_t *target = (uint8_t *)fifo->buffer + (fifo->tail * fifo->item_size);
    memcpy(target, item, fifo->item_size);
    
    // 移动尾指针
    fifo->tail = (fifo->tail + 1) % fifo->max_size;
    
    return true;
}


/**
 * @brief 从队列中获取一个元素
 * @param fifo: 队列结构体指针
 * @param item: 存储获取到的元素的指针
 * @return 获取成功返回true，队列为空返回false
 */
bool fifo_get(fifo_t *fifo, void *item)
{
    // 参数检查
    if (fifo == NULL || item == NULL) {
        return false;
    }
    
    // 检查队列是否为空
    if (fifo->head == fifo->tail) {
        return false;
    }
    
    // 计算源位置并复制数据
    uint8_t *source = (uint8_t *)fifo->buffer + (fifo->head * fifo->item_size);
    memcpy(item, source, fifo->item_size);
    
    // 移动头指针
    fifo->head = (fifo->head + 1) % fifo->max_size;
    
    return true;
}


/**
 * @brief 查看队列头部的元素但不移除它
 * @param fifo: 队列结构体指针
 * @param item: 存储查看到的元素的指针
 * @return 查看成功返回true，队列为空返回false
 */
bool fifo_peek(const fifo_t *fifo, void *item)
{
    // 参数检查
    if (fifo == NULL || item == NULL) {
        return false;
    }
    
    // 检查队列是否为空
    if (fifo->head == fifo->tail) {
        return false;
    }
    
    // 计算源位置并复制数据
    uint8_t *source = (uint8_t *)fifo->buffer + (fifo->head * fifo->item_size);
    memcpy(item, source, fifo->item_size);
    
    return true;
}
