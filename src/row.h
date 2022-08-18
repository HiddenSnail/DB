#pragma once

#include <stdint.h>
#include "table.h"
#include "cursor.h"

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

void SerializeRow(Row* source, void* destination);

void DeserializeRow(void* source, Row* destination);

void* CursorValue(Cursor* cursor);

void PrintRow(Row* row);

Table* DbOpen(const char* filename);

void DbClose(Table* table);

