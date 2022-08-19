#include "row.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "btree.h"

void SerializeRow(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    // memcpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    // memcpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
    // 使用strncpy内存会被初始化，而memcpy内存是随机的
    strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void DeserializeRow(void* source, Row* destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(destination->username, source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(destination->email, source + EMAIL_OFFSET, EMAIL_SIZE);
}

void PrintRow(Row* row)
{
    printf("(%u, %s, %s)\n", row->id, row->username, row->email);
}

Table* DbOpen(const char* filename)
{
    Pager* pager = PagerOpen(filename);
    Table* table = (Table*)malloc(sizeof(Table));
    table->pager = pager;
    table->root_page_num = 0;
    if (pager->num_pages == 0) {
        // 新的数据库文件,初始化page0 作为根节点
        void* root_node = GetPage(pager, 0);
        InitializeLeafNode(root_node);
    }
    return table;
}

void DbClose(Table* table)
{
    Pager* pager = table->pager;
    for (uint32_t i = 0; i < pager->num_pages; ++i) {
        if (pager->pages[i] == NULL) {
            // 内存中的该块page没有数据需要持久化
            continue;
        }
        PagerFlush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        void* page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}

