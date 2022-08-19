#include <stdio.h> 
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "row.h"
#include "cursor.h"
#include "btree.h"

typedef struct {
    char* buffer;
    size_t buffer_length; // size_t无符号，buffer最大长度
    ssize_t input_length; // ssize_t有符号, 保存getline返回的长度
} InputBuffer;

InputBuffer* NewInputBuffer() 
{
    InputBuffer* buff = (InputBuffer*)malloc(sizeof(InputBuffer));
    buff->buffer = NULL;
    buff->buffer_length = 0;
    buff->input_length = 0;
    return buff;
}

void DeleteInputBuffer(InputBuffer* buffer) 
{
    free(buffer->buffer);
    free(buffer);
}

void PrintPrompt() 
{
    printf("db> ");
}

void ReadInput(InputBuffer* buffer) 
{
    // ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    // 如果lineptr为NULL,则由系统帮助malloc，使用者需要后续手动释放
    ssize_t bytes_read = getline(&(buffer->buffer), &(buffer->buffer_length), stdin);
    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    buffer->input_length = bytes_read - 1;
    buffer->buffer[bytes_read - 1] = 0;
}

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
}ExecuteResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

void PrintConstants()
{
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_CELL_SPACE_FOR_CELLS: %d\n", LEAF_NODE_CELL_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void PrintLeafNode(void* node)
{
    uint32_t num_cells = *LeafNodeNumCells(node);
    printf("leaf (size %d)\n", num_cells);
    for (uint32_t i = 0; i < num_cells; ++i) {
        uint32_t key = *LeafNodeKey(node, i);
        printf("  - %d : %d\n", i, key);
    }
}

MetaCommandResult DoMetaCommand(InputBuffer* buffer, Table* table) 
{
    if (strcmp(buffer->buffer, ".exit") == 0) {
        DbClose(table);
        exit(EXIT_SUCCESS);
    } else if (strcmp(buffer->buffer, ".constants") == 0) {
        printf("Constants:\n");
        PrintConstants();
        return META_COMMAND_SUCCESS;
    } else if (strcmp(buffer->buffer, ".btree") == 0) {
        printf("Tree:\n");
        PrintLeafNode(GetPage(table->pager, 0));
        return META_COMMAND_SUCCESS;
    }
    else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

ExecuteResult ExecuteInsert(Statement* statement, Table* table)
{
    void* node = GetPage(table->pager, table->root_page_num);
    if ((*LeafNodeNumCells(node) >= LEAF_NODE_MAX_CELLS)) {
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);
    Cursor* cursor = TableEnd(table);
    LeafNodeInsert(cursor, row_to_insert->id, row_to_insert);
    free(cursor);
    return EXECUTE_SUCCESS;
}

ExecuteResult ExecuteSelect(Statement* statement, Table* table)
{
    Cursor* cursor = TableStart(table);
    Row row;
    while (!(cursor->end_of_table)) {
        DeserializeRow(CursorValue(cursor), &row);
        PrintRow(&row);
        CursorAdvance(cursor);
    }
    free(cursor);
    return EXECUTE_SUCCESS;
}

ExecuteResult ExecuteStatement(Statement* statement, Table* table) {

    switch (statement->type) {
    case (STATEMENT_INSERT):
        //printf("This is where we would do an insert.\n");
        //break;
        return ExecuteInsert(statement, table);
    case (STATEMENT_SELECT):
        //printf("This is where we would do an select.\n");
        //break;
        return ExecuteSelect(statement, table);
    }
}

PrepareResult PrepareInsert(InputBuffer* buffer, Statement* statement)
{
    statement->type = STATEMENT_INSERT;
    char* keyword = strtok(buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if ((id_string == NULL) || (username == NULL) || (email == NULL)) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }

    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

PrepareResult PrepareStatement(InputBuffer* buffer, Statement* statement) 
{
    if (strncmp(buffer->buffer, "insert", 6) == 0) {
        return PrepareInsert(buffer, statement);
    }
    // 此处是否也需要修改为strncmp接口？
    if (strcmp(buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }
    char* filename = argv[1];
    Table* table = DbOpen(filename);

    InputBuffer* input_buffer = NewInputBuffer();
    while (true) {
        PrintPrompt();
        ReadInput(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (DoMetaCommand(input_buffer, table)) {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognize command '%s'.\n", input_buffer->buffer);
                continue;
            }
        }

        Statement statement;
        switch (PrepareStatement(input_buffer, &statement)) {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_SYNTAX_ERROR:
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case PREPARE_NEGATIVE_ID:
            printf("ID must be positive.\n");
            continue;
        case PREPARE_STRING_TOO_LONG:
            printf("String is too long.\n");
            continue;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
            continue;
        }

        switch (ExecuteStatement(&statement, table))
        {
        case EXECUTE_SUCCESS:
            printf("Executed.\n");
            break;
        case EXECUTE_TABLE_FULL:
            printf("Error: Table full.\n");
            break;

        }
    }

    return 0;
}
