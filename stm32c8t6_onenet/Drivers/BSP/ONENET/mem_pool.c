#include "mem_pool.h"
#include <string.h>
#include <stdio.h>

/* 内存块头结构 */
typedef struct
{
    uint16_t size;      /* 分配的大小 */
    uint8_t used;       /* 是否已使用 */
    uint8_t reserved;
} mem_block_t;

/* 内存池 */
static uint8_t mem_pool[MEM_POOL_SIZE];
static mem_block_t* first_block = NULL;

/**
 * @brief 初始化内存池
 */
void mem_pool_init(void)
{
    first_block = (mem_block_t*)mem_pool;
    first_block->size = MEM_POOL_SIZE - sizeof(mem_block_t);
    first_block->used = 0;
    
    printf("[MEM_POOL] Initialized: %d bytes\r\n", MEM_POOL_SIZE);
}

/**
 * @brief 从内存池分配内存
 * @param size 要分配的字节数
 * @return 分配的内存指针，失败返回 NULL
 */
void* mem_pool_malloc(size_t size)
{
    mem_block_t* block = first_block;
    
    if (size == 0)
        return NULL;
    
    /* 查找足够大的空闲块 */
    while ((uintptr_t)block - (uintptr_t)mem_pool < MEM_POOL_SIZE)
    {
        if (!block->used && block->size >= size)
        {
            /* 找到合适的块 */
            block->used = 1;
            
            /* 如果块足够大，分割块 */
            if (block->size > size + sizeof(mem_block_t))
            {
                mem_block_t* next_block = (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + size);
                next_block->size = block->size - size - sizeof(mem_block_t);
                next_block->used = 0;
                block->size = size;
            }
            
            return (void*)((uint8_t*)block + sizeof(mem_block_t));
        }
        
        block = (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + block->size);
    }
    
    printf("[MEM_POOL] ERROR: Allocation failed for %d bytes\r\n", (int)size);
    return NULL;
}

/**
 * @brief 释放内存池中的内存
 * @param ptr 要释放的指针
 */
void mem_pool_free(void* ptr)
{
    mem_block_t* block;
    
    if (ptr == NULL)
        return;
    
    /* 获取块头 */
    block = (mem_block_t*)((uint8_t*)ptr - sizeof(mem_block_t));
    
    /* 验证指针有效性 */
    if ((uintptr_t)block < (uintptr_t)mem_pool || 
        (uintptr_t)block >= (uintptr_t)(mem_pool + MEM_POOL_SIZE))
    {
        printf("[MEM_POOL] ERROR: Invalid pointer to free\r\n");
        return;
    }
    
    block->used = 0;
}

/**
 * @brief 获取内存池使用统计
 */
void mem_pool_stat(void)
{
    mem_block_t* block = first_block;
    uint16_t used_total = 0;
    uint16_t free_total = 0;
    
    while ((uintptr_t)block - (uintptr_t)mem_pool < MEM_POOL_SIZE)
    {
        if (block->used)
            used_total += block->size;
        else
            free_total += block->size;
        
        block = (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + block->size);
    }
    
    printf("[MEM_POOL] Used: %d bytes, Free: %d bytes\r\n", used_total, free_total);
}
