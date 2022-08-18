#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "table.h"

typedef struct {
    Table* table;
    uint32_t row_num; // 当前指向的行数
    bool end_of_table; // 表示最后一个元素之后的位置
} Cursor;

Cursor* TableStart(Table* table);

Cursor* TableEnd(Table* table);

void CursorAdvance(Cursor* cursor);
