/******************************************
*                _____  __     _   ___    *
*               (  _  )(  )   / ) / _ \   *
*    |\---/|     )(_)(  )(__ / _ \\_  /   *
*    | ,_, |    (_____)(____)\___/ (_/    *
*     \_`_/-..----.                       *
*  ___/ `   ' ,""+ \                      *
* (__...'   __\    |`.___.';              *
*  (_,...'(_,.`__)/'.....+                *
*                                         *
*             Exersice: Heap              *
******************************************/
#ifndef HEAP_H
#define HEAP_H

typedef struct heap heap_t;

/* Compare function returns 1 if data2 > data1, 0 otherwise. 
   Compare must not be NULL.
   Create returns an initialized heap on success
   and NULL on failure */
heap_t *HeapCreate(int (*IsData2BeforeData1)(const void *data1,
                                             const void *data2,
                                             const void *params));

/* heap must not be null */
void HeapDestroy(heap_t *heap);

/* heap must not be null.r eturns 0
   on success, 1 on failure */
int HeapPush(heap_t *heap, void *data);

/* returns the root of the heap.
   Behavior is undefined if the heap is empty
   heap must not be null */
void *HeapPeek(const heap_t *heap);

/* Behavior is undefined if the heap is empty */
/* heap must not be null */
void HeapPop(heap_t *heap);

/* heap must not be NULL. Compare function returns
   1 if data needs to be removed, Otherwise returns
   0. Compare must not be NULL.
   the function returns 0 if it removed the element, otherwise 1. */
int HeapRemove(heap_t *heap, 
           int (*ShouldRemove)(const void *data,
                   const void *key,
                   const void *params),
            const void *key);
            
/* heap must not be NULL */
size_t HeapSize(const heap_t *heap);

/* heap must not be NULL */
int HeapIsEmpty(const heap_t *heap);

#endif
