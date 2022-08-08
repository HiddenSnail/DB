#pragma once

#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];    
} Table;

// 序列化
void SerializeRow(Row* source, void* destination);

// 反序列化
void DeserializeRow(void* source, Row* destination);

void* RowSlot(Table* table, uint32_t row_num);

void PrintRow(Row* row);

Table* NewTable();

void FreeTable(Table* table);