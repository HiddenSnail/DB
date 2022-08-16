#pragma once

#include <stdint.h>
#include "pager.h"

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;


void SerializeRow(Row* source, void* destination);

void DeserializeRow(void* source, Row* destination);

void* RowSlot(Table* table, uint32_t row_num);

void PrintRow(Row* row);

Table* DbOpen(const char* filename);

void DbClose(Table* table);

