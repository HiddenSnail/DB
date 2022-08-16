#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;

Pager* PagerOpen(const char* filename);

void* GetPage(Pager* pager, uint32_t page_num);

void PagerFlush(Pager* pager, uint32_t page_num, uint32_t size);