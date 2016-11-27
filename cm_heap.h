#ifndef CM_HEAP_H
#define CM_HEAP_H

#define CM_HEAP_MAX_KVAL 32  /* 4 gig */

typedef struct cm_heap_chunk {
  char allocated;
  char kval;
  struct cm_heap_chunk *prev, *next;
} cm_heap_Chunk;

typedef struct {
  char* mem;
  size_t size;
  cm_heap_Chunk free_lists [CM_HEAP_MAX_KVAL];
} cm_Heap;

cm_Heap* cm_heap_init (char* mem, size_t size);
void* cm_heap_alloc (cm_Heap* heap, size_t size);
void cm_heap_free (cm_Heap* heap, void* chunk);

#endif 


