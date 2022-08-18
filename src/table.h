#pragma once

#include <stdint.h>
#include "pager.h"

typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;
