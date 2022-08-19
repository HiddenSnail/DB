#include "cursor.h"

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

Cursor* TableEnd(Table* table)
{
    Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    // Q:不理解为什么还是取root节点
    void* root_node = GetPage(table->pager, table->root_page_num);
    uint32_t num_cells = *LeafNodeNumCells(root_node);
    cursor->cell_num = num_cells;
    cursor->end_of_table = true;
    return cursor;
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
