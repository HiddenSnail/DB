#include <stdio.h> 
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "row.h"

extern const uint32_t TABLE_MAX_ROW;

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
    PREPARE_SYNTAX_ERROR,
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


MetaCommandResult DoMetaCommand(InputBuffer* buffer) 
{
    if (strcmp(buffer->buffer, ".exit") == 0) {
        DeleteInputBuffer(buffer);
        exit(EXIT_SUCCESS);
    }
    else {
        printf("Unrecognize command '%s'.\n", buffer->buffer);
    }
}

ExecuteResult ExecuteInsert(Statement* statement, Table* table)
{
    if (table->num_rows > TABLE_MAX_ROW) {
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);
    SerializeRow(row_to_insert, RowSlot(table, table->num_rows));
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult ExecuteSelect(Statement* statement, Table* table)
{
    Row row;
    for (uint32_t i = 0; i < table->num_rows; ++i) {
        DeserializeRow(RowSlot(table, i), &row);
        PrintRow(&row);
    }
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

PrepareResult PrepareStatement(InputBuffer* buffer, Statement* statement) 
{
    if (strncmp(buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), 
                                    statement->row_to_insert.username, statement->row_to_insert.email);
        if (args_assigned < 3) {
            printf("Syntax error '%s'.\n", buffer->buffer);
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
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
    Table* table = NewTable();
    InputBuffer* input_buffer = NewInputBuffer();
    while (true) {
        PrintPrompt();
        ReadInput(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (DoMetaCommand(input_buffer)) {
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
