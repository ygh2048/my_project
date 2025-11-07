#ifndef _MEM_POOL_H_
#define _MEM_POOL_H_

#include <stdint.h>
#include <stddef.h>

/* 内存池大小 */
#define MEM_POOL_SIZE  (4 * 1024)  /* 4KB 用于 MQTT */

/* 初始化内存池 */
void mem_pool_init(void);

/* 从内存池分配内存 */
void* mem_pool_malloc(size_t size);

/* 释放内存池中的内存 */
void mem_pool_free(void* ptr);

#endif /* _MEM_POOL_H_ */
