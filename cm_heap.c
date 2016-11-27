#include <stdlib.h>		/* for size_t */
#include "cm_heap.h"

#define BUDDY(chunk, heap) \
((cm_heap_Chunk*) \
 (heap->mem + (((char*) chunk - (char*) heap->mem) ^ (1 << chunk->kval))))

#define MIN(x, y) (x < y ? x : y)


/*
 * cm_heap_new - init a new heap. 
 */

cm_Heap*
cm_heap_init (char* mem, size_t size)
{
  cm_Heap* heap;
  int i, k, chunk_size;

  /* make sure we have room to allocate the heap object out of the heap */
  if (size < sizeof (cm_Heap) + sizeof (cm_heap_Chunk))
    return NULL;

  /* steal some memory for the heap object itself */
  heap = (cm_Heap*) mem;
  mem += sizeof (cm_Heap);
  size -= sizeof (cm_Heap);
     
  /* init the heap object */
  heap->mem = mem;
  heap->size = size;

  /* init free-lists empty */
  for (i = 0;  i < CM_HEAP_MAX_KVAL;  i++)
    heap->free_lists[i].prev = heap->free_lists[i].next = 
      &heap->free_lists[i];

  /* calculate largest remaining power of 2 less-than or equal-to size */
  for (k = 0;  (1 << (k + 1)) <= size;  k++);
  chunk_size = 1 << k;

  /* enter mem into free lists in chunks whose sizes are powers of 2 */
  /* ignore the little bit at the end which is too small to hold a chunk */
  while (chunk_size <= size && chunk_size >= sizeof (cm_heap_Chunk))
  {
    cm_heap_Chunk* chunk = (cm_heap_Chunk*) mem;
    chunk->allocated = 0;
    chunk->kval = k;
    chunk->next = heap->free_lists[k].next;
    chunk->prev = &heap->free_lists[k];
    chunk->next->prev = chunk->prev->next = chunk;
    mem += chunk_size;
    size -= chunk_size;
    k--;  chunk_size /= 2;
  }

  return heap;
}
  

void*
cm_heap_alloc (cm_Heap* heap, size_t size)
{
  cm_heap_Chunk* chunk;
  int k, kval;

  /* add in space for overhead */
  size += sizeof (cm_heap_Chunk);

  /* round size up to next power of 2 and calculate kval(size) */
  for (kval = 0;  (1 << kval) < size;  kval++);
  size = 1 << kval;

  /* look for free-chunk of sufficient size */
  for (k = kval;  k < CM_HEAP_MAX_KVAL;  k++)
    if (heap->free_lists[k].next != &heap->free_lists[k])
      break;  /* this free list is not empty */

  if (k == CM_HEAP_MAX_KVAL)
    return NULL;  /* memory exhausted */

  /* remove first chunk from free-list */
  chunk = heap->free_lists[k].next;
  chunk->next->prev = chunk->prev;
  chunk->prev->next = chunk->next;

  while (chunk->kval > kval)
  {
    cm_heap_Chunk* buddy;

    /* cut free chunk in half */
    chunk->kval--;

    buddy = BUDDY (chunk, heap);

    buddy->kval = chunk->kval;
    buddy->allocated = 0;
      
    /* put buddy in free list */
    buddy->next = heap->free_lists[k].next;
    buddy->prev = &heap->free_lists[k];
    buddy->next->prev = buddy->prev->next = buddy;
  }

  chunk->allocated = 1;  /* mark allocated */
  chunk++;  /* move pointer over overhead */
  memset ((void*) chunk, 0, size - sizeof (cm_heap_Chunk));  /* zero it */

  return (void*) chunk;
}


void
cm_heap_free (cm_Heap* heap, void* ptr)
{
  cm_heap_Chunk* chunk = ptr;

  chunk--;  /* move pointer over overhead */

  while (1)  /* coalesce chunk with buddy as long as buddy not allocated */
  {
    cm_heap_Chunk* buddy = BUDDY (chunk, heap);

    if (buddy->allocated)
      break;  /* cannot coalesce any further */

    /* remove buddy from free-list */
    buddy->next->prev = buddy->prev;
    buddy->prev->next = buddy->next;

    /* coalesce chunk and buddy, bumping kval */
    chunk = MIN (chunk, buddy);
    chunk->kval++;
  }

  /* put chunk in free-list */
  chunk->allocated = 0;
  chunk->next = heap->free_lists[chunk->kval].next;
  chunk->prev = &heap->free_lists[chunk->kval];
  chunk->next->prev = chunk->prev->next = chunk;
}
