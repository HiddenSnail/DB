#include "btree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t* LeafNodeNumCells(void* node)
{
    // 数据存放在node的前4个字节
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void* LeafNodeCell(void* node, uint32_t cell_num)
{
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t* LeafNodeKey(void* node, uint32_t cell_num)
{
    return LeafNodeCell(node, cell_num);
}

void* LeafNodeValue(void* node, uint32_t cell_num)
{
    return LeafNodeCell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

// 把对应node的cell的个数设置成0，即可完成初始化
void InitializeLeafNode(void* node)
{
    *LeafNodeNumCells(node) = 0;
}

void LeafNodeInsert(Cursor* cursor, uint32_t key, Row* value)
{
    void* node = GetPage(cursor->table->pager, cursor->page_num);
    uint32_t num_cells = *LeafNodeNumCells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node full
        printf("Need to implement splitting a leaf node.\n");
        exit(EXIT_FAILURE);
    }

    if (cursor->cell_num < num_cells) {
        // Make room for new cell
        for (uint32_t i = num_cells; i > cursor->cell_num; --i) {
            memcpy(LeafNodeCell(node, i), LeafNodeCell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(LeafNodeNumCells(node)) += 1;
    *(LeafNodeKey(node, cursor->cell_num)) = key;
    SerializeRow(value, LeafNodeValue(node, cursor->cell_num));
}
