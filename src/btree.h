#pragma once

#include <stdint.h>
#include "row.h"
#include "pager.h"
#include "table.h"
#include "cursor.h"

typedef enum { NODE_INTERNAL, NODE_LEFT } NodeType;

// Common node header layout
#define NODE_TYPE_SIZE (sizeof(uint8_t)) // 存储节点类型
#define NODE_TYPE_OFFSET (0)
#define IS_ROOT_SIZE (sizeof(uint8_t)) // 存储是否是根节点
#define IS_ROOT_OFFSET (NODE_TYPE_SIZE)
#define PARENT_POINTER_SIZE (sizeof(uint32_t)) // 存储父节点指针
#define PARENT_POINTER_OFFSET (IS_ROOT_OFFSET + IS_ROOT_SIZE)
#define COMMON_NODE_HEADER_SIZE (NODE_TYPE_SIZE + IS_ROOT_OFFSET + PARENT_POINTER_SIZE)

// Leaf node head layout
#define LEAF_NODE_NUM_CELLS_SIZE (sizeof(uint32_t)) // 存储叶子节点保存了多少kv对
#define LEAF_NODE_NUM_CELLS_OFFSET (COMMON_NODE_HEADER_SIZE)
#define LEAF_NODE_HEADER_SIZE (COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE) // 叶子节点头，包含common头

// Leaf node body layout
#define LEAF_NODE_KEY_SIZE (sizeof(uint32_t))
#define LEAF_NODE_KEY_OFFSET (0)
#define LEAF_NODE_VALUE_SIZE (ROW_SIZE) // value的大小就是row的大小
#define LEAF_NODE_VALUE_OFFSET (LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE)
#define LEAF_NODE_CELL_SIZE (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)
#define LEAF_NODE_CELL_SPACE_FOR_CELLS (PAGE_SIZE - LEAF_NODE_HEADER_SIZE)
#define LEAF_NODE_MAX_CELLS (LEAF_NODE_CELL_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE) // 每个page能够装的最多的kv对个数

// 对应叶子节点的Cell个数
uint32_t* LeafNodeNumCells(void* node);

// 获取叶子节点的对应Cell指针
void* LeafNodeCell(void* node, uint32_t cell_num);

// 获取叶子节点的对应Cell索引
uint32_t* LeafNodeKey(void* node, uint32_t cell_num);

// 获取叶子节点的对应Cell索引所指的对象
void* LeafNodeValue(void* node, uint32_t cell_num);

// 初始化叶子节点
void InitializeLeafNode(void* node);

void LeafNodeInsert(Cursor* cursor, uint32_t key, Row* value);

Cursor* LeafNodeFind(Table* table, uint32_t page_num, uint32_t key);
