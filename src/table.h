#pragma once

#include <stdint.h>
#include "pager.h"

typedef struct {
    uint32_t root_page_num; // root page下标，从0开始
    Pager* pager;
} Table;
