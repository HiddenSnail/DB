#pragma once

#include <stdint.h>
#include "pager.h"

typedef struct {
    uint32_t root_page_num;
    Pager* pager;
} Table;
