#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 队列结构体
typedef struct {
    void *buffer;      // 数据缓冲区
    size_t head;       // 队头指针
    size_t tail;       // 队尾指针
    size_t max_size;   // 队列最大容量
    size_t item_size;  // 每个元素的大小
}fifo_t;



bool fifo_init(fifo_t *fifo, void *buffer, size_t max_size, size_t item_size);
bool fifo_put(fifo_t *fifo, const void *item);
bool fifo_get(fifo_t *fifo, void *item);
bool fifo_peek(const fifo_t *fifo, void *item);
bool fifo_is_empty(const fifo_t *fifo);
bool fifo_is_full(const fifo_t *fifo);
size_t fifo_size(const fifo_t *fifo);
size_t fifo_available(const fifo_t *fifo);
void fifo_flush(fifo_t *fifo);









#endif


