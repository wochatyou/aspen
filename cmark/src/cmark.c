#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "node.h"
#include "houdini.h"
#include "cmark.h"
#include "buffer.h"

int cmark_version(void) { return CMARK_VERSION; }

const char *cmark_version_string(void) { return CMARK_VERSION_STRING; }

static void *xcalloc(size_t nmem, size_t size) 
{
  void *ptr = calloc(nmem, size); /// calloc有两个参数，第一个是块的数量，第二个是每个块的大小
  if (!ptr) {
    fprintf(stderr, "[cmark] calloc returned null pointer, aborting\n");
    abort();
  }
  return ptr;
}

static void *xrealloc(void *ptr, size_t size) 
{
  void *new_ptr = realloc(ptr, size); /// 重新分配内存位size大小
  if (!new_ptr) {
    fprintf(stderr, "[cmark] realloc returned null pointer, aborting\n");
    abort();
  }
  return new_ptr;
}

cmark_mem DEFAULT_MEM_ALLOCATOR = {xcalloc, xrealloc, free}; /// 内存分配和释放的函数指针

cmark_mem *cmark_get_default_mem_allocator(void) {
  return &DEFAULT_MEM_ALLOCATOR;
}


char *cmark_markdown_to_html(const char *text, size_t len, int options) {
  cmark_node *doc;
  char *result;

  doc = cmark_parse_document(text, len, options);

  result = cmark_render_html(doc, options);
  cmark_node_free(doc);

  return result;
}
