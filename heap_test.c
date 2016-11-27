#include <stdlib.h>
#include <stdio.h>
#include "cm_table.h"
#include "cm_heap.h"

/* the classic knuthian buddy function */
#define BUDDY(chunk, k, heap) \
((cm_heap_Chunk*) \
 (heap->mem + (((char*) chunk - (char*) heap->mem) ^ (1 << k))))


static void
fail (void)
{
  printf ("test failed\n");
  exit (1);
}

static void
memory_error (void)
{
  printf ("memory error\n");
  exit (1);
}

static int
R (int max)  /* R - return random x | 0 <= x < max */
{
  extern double drand48();
  return (int) (drand48() * max);
}


/* 
 * Chunk - used to make maps of heaps.  
 * not to be confused with cm_heap_Chunk.
 */

typedef struct {
  void* addr;
  size_t size;
} Chunk;  

static Chunk*
chunk_free (Chunk* chunk)
{
  free (chunk);
  return NULL;
}

static Chunk*
chunk_new (void* addr, size_t size)
{
  Chunk* chunk = malloc (sizeof (Chunk));
  if (!chunk) return NULL;

  chunk->addr = addr;
  chunk->size = size;

  return chunk;
}


/* 
 * make_map - build a cm_Table of allocated Chunks indexed by chunk->addr.
 */

static cm_Table* 
make_map (cm_Heap* heap)
{
  cm_heap_Chunk* chunk = (void*) heap->mem;

  cm_Table* map = cm_table_new (NULL);
  if (!map) memory_error();

  while ((void*) chunk < (void*) (heap->mem + heap->size))
  {
    size_t size = 1 << chunk->kval;
    
    if (chunk->allocated)
      if (!cm_table_put (map, chunk, chunk_new (chunk, size)))
	memory_error();

    chunk = (cm_heap_Chunk*) ((char*) chunk + size);
  }

  return map;
}


int /* bool */
map_eq (cm_Table* map1, cm_Table* map2)
{
  Chunk *chunk1, *chunk2;
  char *addr1, *addr2;

  if (cm_table_size (map1) != cm_table_size (map2))
    return 0;

  for (chunk1 = cm_table_get_min (map1, (void**) &addr1),
       chunk2 = cm_table_get_min (map2, (void**) &addr2);
       chunk1 && chunk2;
       chunk1 = cm_table_get_next (map1, (void**) &addr1),
       chunk2 = cm_table_get_next (map2, (void**) &addr2))
  {
    if (chunk1->addr != chunk2->addr || chunk1->size != chunk2->size)
      return 0;
  }

  return 1;
}

  

/*
 * check_free_lists - 
 *
 * check that there are no overlapping free chunks.
 * check that all free buddies are coalesced.
 */

static void
check_free_lists (cm_Heap* heap)
{
  cm_Table* free_map = NULL;
  Chunk *chunk, *last_chunk;
  void* addr;
  size_t size;
  int k;

  free_map = cm_table_new (NULL);
  if (!free_map) memory_error();

  /* make map of free chunks */
  for (k = 0;  k < CM_HEAP_MAX_KVAL;  k++)
  {
    cm_heap_Chunk* chunk;

    for (chunk = heap->free_lists[k].next;
	 chunk != &heap->free_lists[k];
	 chunk = chunk->next)
    {
      void* addr = chunk;
      size_t size = 1 << chunk->kval;
      if (!cm_table_put (free_map, addr, chunk_new (addr, size)))
	memory_error();
    }
  }

  last_chunk = cm_table_get_max (free_map, NULL);

  /* look for overlapping free chunks */
  for (chunk = cm_table_get_min (free_map, &addr);
       chunk != last_chunk;
       chunk = cm_table_get_next (free_map, &addr))
  {
    cm_heap_Chunk* buddy = BUDDY (chunk, chunk->kval, heap);
    void* addr2 = (char*) chunk->addr + chunk->size;
    Chunk* chunk2 = cm_table_get_prev (free_map, &addr2);
    if (chunk != chunk2) 
      fail();
  }
}


static void
heap_print (cm_Heap* heap)
{
  cm_heap_Chunk* chunk = (void*) heap->mem;

  printf ("----------------------\n");

  while ((void*) chunk < (void*) (heap->mem + heap->size))
  {
    size_t size = 1 << chunk->kval;
    printf ("%d\t%x\t%d\n", chunk->allocated, chunk, size);
    chunk = (cm_heap_Chunk*) ((char*) chunk + size);
  }

  printf ("----------------------\n");
}
  
  
/* 
 * until the heap is exhausted, we randomly allocate and deallocate chunks of
 * random size.  we allocate with somewhat more probability than we deallocate,
 * so the heap is eventually exhausted.  during this process, we build a map of
 * the allocated chunks.  afterwards, we compare this map with that generated
 * by make_map(). 
 */

#define CM_HEAP_OVERHEAD sizeof (cm_heap_Chunk)

char MEM [1024 * 1024];

static int
SIZEUP (size_t size)
{
  int k;
  size += sizeof (cm_heap_Chunk);
  for (k = 0;  (1 << k) < size;  k++);
  return 1 << k;
}


main ()
{
  int done = 0;
  cm_Heap* heap;
  cm_Table *map, *map2;
  Chunk* chunk;
  char* addr;
  size_t size;

  map = cm_table_new (NULL);
  if (!map) memory_error();

  heap = cm_heap_new (MEM, sizeof MEM);
  if (!heap) fail();

  size = SIZEUP (sizeof (cm_Heap));
  addr = (char*) heap - CM_HEAP_OVERHEAD;
  if (!cm_table_put (map, addr, chunk_new (addr, size)))
    memory_error();
      
  while (!done)
  {
    heap_print (heap);

    switch (R(3))  
    {
      case 0:  case 1:  /* allocate a chunk */
      {
	size = R (1024);  /* pick a random size */
	addr = cm_heap_alloc (heap, size);  /* allocate a chunk */

 	if (!addr)  /* heap exhausted */
	{
	  done = 1;  /* this is the exit */
	  break;
	}

	/* record the allocation in the map */
	size = SIZEUP (size);
	addr -= CM_HEAP_OVERHEAD;
	if (!cm_table_put (map, addr, chunk_new (addr, size)))
	  memory_error();
      }
      break;

      case 2:  /* free a chunk */
      {
	if (!cm_table_size (map))  /* map is empty */
	  break;  /* nothing allocated - nothing to free */

	/* pick an allocated chunk randomly out out the map */
	addr = heap->mem + R (sizeof MEM);
	chunk = cm_table_del_next (map, (void**) &addr);
	if (!chunk) chunk = cm_table_del_max (map, NULL);
	if (!chunk) fail();
	
	/* free the chunk */
	cm_heap_free (heap, chunk->addr + CM_HEAP_OVERHEAD);
      }
      break;
    } /* end switch */
  } /* end while */

  map2 = make_map (heap);

  if (!map_eq (map, map2))
    fail();

  check_free_lists (heap);

  printf ("test passed\n");
}
