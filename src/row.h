#pragma once

#include <stdint.h>
#include "table.h"
#include "cursor.h"

#define COLUMN_USERNAME_SIZE (32)
#define COLUMN_EMAIL_SIZE (255)
#define size_of_attribute(Struct, Attribute) (sizeof(((Struct*)0)->Attribute))

#define ID_SIZE (size_of_attribute(Row, id))
#define USERNAME_SIZE (size_of_attribute(Row, username))
#define EMAIL_SIZE (size_of_attribute(Row, email))
#define ID_OFFSET (0)
#define USERNAME_OFFSET (ID_SIZE + ID_OFFSET)
#define EMAIL_OFFSET (USERNAME_SIZE + USERNAME_OFFSET)
#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE) // 293 Bytes

#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROW (TABLE_MAX_PAGES * ROWS_PER_PAGE)

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

void SerializeRow(Row* source, void* destination);

void DeserializeRow(void* source, Row* destination);

void PrintRow(Row* row);

Table* DbOpen(const char* filename);

void DbClose(Table* table);

