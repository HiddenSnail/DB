#include "cursor.h"

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

Cursor* TableStart(Table* table)
{
    Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;
    void* root_node = GetPage(table->pager, table->root_page_num);
    uint32_t num_cells = *LeafNodeNumCells(root_node);
    cursor->end_of_table = (num_cells == 0);
    return cursor;
}

Cursor* TableFind(Table* table, uint32_t key)
{
    uint32_t root_page_num = table->root_page_num;
    void* root_node = GetPage(table->pager, root_page_num);

    if (GetNodeType(root_node) == NODE_LEAF) {
        return LeafNodeFind(table, root_page_num, key);
    } else {
        printf("Need to implement searching an internal node\n");
        exit(EXIT_FAILURE);
    }
}

void* CursorValue(Cursor* cursor)
{
    uint32_t page_num = cursor->page_num;
    void* page = GetPage(cursor->table->pager, page_num);
    return LeafNodeValue(page, cursor->cell_num);
}

void CursorAdvance(Cursor* cursor)
{
    uint32_t page_num = cursor->page_num;
    void* node = GetPage(cursor->table->pager, page_num);
    cursor->cell_num += 1;

    if (cursor->cell_num >= *LeafNodeNumCells(node)) {
        cursor->end_of_table = true;
    }
}
