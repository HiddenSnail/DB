#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "table.h"

typedef struct {
    Table* table;
    uint32_t page_num; // page下标，从0开始计数
    uint32_t cell_num; // cell下标，从0开始计数
    bool end_of_table; // 表示最后一个元素之后的位置
} Cursor;

Cursor* TableStart(Table* table);

Cursor* TableFind(Table* table, uint32_t key);

void* CursorValue(Cursor* cursor);

void CursorAdvance(Cursor* cursor);
