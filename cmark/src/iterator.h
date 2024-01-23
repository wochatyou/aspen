#ifndef CMARK_ITERATOR_H
#define CMARK_ITERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmark.h"

typedef struct {
  cmark_event_type ev_type; /// 这是一个枚举类型
  cmark_node *node;
} cmark_iter_state;

struct cmark_iter { /// 遍历树所使用的数据结构
  cmark_mem *mem;
  cmark_node *root; /// 根节点始终保持不变
  cmark_iter_state cur;
  cmark_iter_state next;
};

#ifdef __cplusplus
}
#endif

#endif
