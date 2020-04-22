/*****************************************
        Author: Yael Lotan
        Reviewer: Sandra Hadad
*****************************************/

#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */

#include "dvec.h" /* srtl header file */
#include "heap.h" /* heap header file */

#define NUM_OF_ELEMENTS 4

static size_t IndexLeftChildren(size_t root);
static size_t IndexRightChildren(size_t root);
static int LeftChildIsBiggerThanRoot(heap_t *heap, size_t root);
static int RightChildIsBiggerThanRoot(heap_t *heap, size_t root);
static int RootIsbiggerThanChildren(heap_t *heap, size_t root);
static int WhichChildIsBigger(heap_t *heap, size_t root);
static void **FindAddress(heap_t *heap, size_t index);
static void HeapifyDown(heap_t *heap, size_t cur_index);
static void HeapifyUp(heap_t *heap, size_t cut_index);
static void Swap(void **data1, void **data2);
static int RootIsBiggerThanCurChild(heap_t *heap, size_t current);
static int HasChildren(heap_t *heap, size_t root);

struct heap
{
	dvec_t *vector;
	int (*IsData2BeforeData1)(const void *data1,
                                             const void *data2,
                                             const void *params);
};

static int HasChildren(heap_t *heap, size_t root)
{
    size_t last_i = HeapSize(heap) - 1;
    int result = last_i - root > 2;
    
    if (0 == root)
    {
        return last_i > 0;
    }
    
    return result;
}

static size_t IndexLeftChildren(size_t root)
{
    if (0 == root)
    {
        return 1;
    }
    
    return (root * 2 + 1);
}

static size_t IndexRightChildren(size_t root)
{
    if (0 == root)
    {
        return 2;
    }
    
    return (root * 2 + 2);
}

static int LeftChildIsBiggerThanRoot(heap_t *heap, size_t root)
{
    size_t l_child_index = IndexLeftChildren(root);
    void **root_address = FindAddress(heap, root);
    void **l_child_address = FindAddress(heap, l_child_index);
    int result = heap->IsData2BeforeData1(*root_address, *l_child_address, NULL);
    
    return result;
}

static int RightChildIsBiggerThanRoot(heap_t *heap, size_t root)
{
    size_t r_child_index = IndexRightChildren(root);
    void **root_address = FindAddress(heap, root);
    void **r_child_address = FindAddress(heap, r_child_index);
    int result = heap->IsData2BeforeData1(*root_address, *r_child_address, NULL);
    
    if (2 == HeapSize(heap))
    {
        return 0;
    }
    
    return result;
}

/* return value: 1 if root is bigger than both children, 0 otherwise */
static int RootIsbiggerThanChildren(heap_t *heap, size_t root)
{
    int l_child_is_bigger = LeftChildIsBiggerThanRoot(heap, root);
    int r_child_is_bigger = RightChildIsBiggerThanRoot(heap, root);
    
    if (!l_child_is_bigger && !r_child_is_bigger)
    {
        return 1; 
    }
    
    return 0;
}

static int RootIsBiggerThanCurChild(heap_t *heap, size_t current)
{
    void **root_address = NULL;
    void **cur_address = DVECGetItemAddress(heap->vector, current);
    int result = 0;
    size_t add = 0;
    
    if (0 == current)
    {
        return 1;
    }
    if (0 == current % 2)
    {
        add = 1;
    }
    
    root_address = DVECGetItemAddress(heap->vector, (current - add)/ 2);
    result = heap->IsData2BeforeData1(*cur_address, *root_address, NULL);
    
    return result;
}

/* return value: 1 if left is bigger, -1 if right is bigger, 0 if they are equal */
static int WhichChildIsBigger(heap_t *heap, size_t root)
{
    size_t l_child_index = IndexLeftChildren(root);
    size_t r_child_index = IndexRightChildren(root);
    void **l_child_address = NULL;
    void **r_child_address = NULL;
    int result = 0;
    size_t last_index = HeapSize(heap) - 1;
    
    if (r_child_index > last_index) /* if there is no right child */
    {
        return 1;
    }
    
    l_child_address = FindAddress(heap, l_child_index);
    r_child_address = FindAddress(heap, r_child_index);
    result = heap->IsData2BeforeData1(*r_child_address, *l_child_address, NULL);
    
    return result;
}

static void **FindAddress(heap_t *heap, size_t index)
{
    void **address = DVECGetItemAddress(heap->vector, index);
    
    return address; 
}

static void HeapifyDown(heap_t *heap, size_t cur_index)
{
    int found_place = 0;
    
    while (HasChildren(heap, cur_index) && !found_place)
    {
        if (!RootIsbiggerThanChildren(heap, cur_index))
        {
            int left_is_bigger = WhichChildIsBigger(heap, cur_index);
            void **cur_address = FindAddress(heap, cur_index);
            void **l_child_address = FindAddress(heap, IndexLeftChildren(cur_index));
            void **r_child_address = FindAddress(heap, IndexRightChildren(cur_index));
            
            if (0 < left_is_bigger)
            {
                Swap(l_child_address, cur_address);
                cur_index = IndexLeftChildren(cur_index);
            }
            else
            {
                Swap(r_child_address, cur_address); 
                cur_index = IndexRightChildren(cur_index);
            }                  
        }
        else
        {
            found_place = 1;
        }
    } 
}

static void HeapifyUp(heap_t *heap, size_t cur_index)
{
    int found_place = 0;
    size_t add = 0;
    
    while (0 < cur_index && !found_place)
    {
        if (!RootIsBiggerThanCurChild(heap, cur_index))
        {
            void **cur_address = NULL;
            void **root_address =NULL;
            
            if (0 == cur_index % 2)
            {
                add = 1;
            }
    
            cur_address = DVECGetItemAddress(heap->vector, cur_index);
            root_address = DVECGetItemAddress(heap->vector, (cur_index - add)/ 2);
            
            Swap(cur_address, root_address);
            cur_index = (cur_index - add) / 2;   
        }
        else
        {
            found_place = 1;
        }
    } 
}

static void Swap(void **data1, void **data2)
{
    void *temp = *data1;
    
    *data1 = *data2;
    *data2 = temp; 
}

heap_t *HeapCreate(int (*IsData2BeforeData1)(const void *data1,
                                             const void *data2,
                                             const void *params))
{
    heap_t *heap = NULL;
    
    assert (NULL != IsData2BeforeData1);
    
    heap = (heap_t *)malloc(sizeof(heap_t));
    if (NULL == heap)
    {
        return NULL;
    } 
    
    heap->vector = DVECCreate(sizeof(void *), NUM_OF_ELEMENTS);
    if (NULL == heap->vector)
    {
        free(heap);
        
        return NULL;
    }
    
    heap->IsData2BeforeData1 = IsData2BeforeData1;
    
    return heap;
}

void HeapDestroy(heap_t *heap)
{
    assert (NULL != heap);
    
    DVECDestroy(heap->vector);
	heap->vector = NULL;
    
    free(heap);
	heap = NULL;	
}

size_t HeapSize(const heap_t *heap)
{
    assert (NULL != heap);
    
    return DVECSize(heap->vector);
}

int HeapIsEmpty(const heap_t *heap)
{
    assert (NULL != heap);
    
    return (0 == DVECSize(heap->vector)); 
}

void *HeapPeek(const heap_t *heap)
{   
    assert (NULL != heap);
    
    return *(void **)DVECGetItemAddress(heap->vector, 0);
}

void HeapPop(heap_t *heap)
{
    size_t last_index = 0;
    void **last_child_address = NULL;
    void **root_address = NULL;
    
    assert (NULL != heap);

    if (1 == HeapSize(heap))
    {
         DVECPopBack(heap->vector);
         
         return;  
    }
    
    last_index = DVECSize((dvec_t *)heap->vector) - 1;
    last_child_address = FindAddress(heap, last_index);
    root_address = FindAddress(heap, 0);
    
    Swap(last_child_address, root_address);
    DVECPopBack(heap->vector);
    HeapifyDown(heap, 0);
}

int HeapPush(heap_t *heap, void *data)
{
    int status = 0;
    
    assert (NULL != heap);
    
    status = DVECPushBack(heap->vector, &data);
    
    if (1 == HeapSize(heap))
    {
         return status;  
    }
    
    HeapifyUp(heap, DVECSize((dvec_t *)heap->vector) - 1);
    
    return status;
}

int HeapRemove(heap_t *heap, 
           int (*ShouldRemove)(const void *data,
                   const void *key,
                   const void *params),
            const void *key)
{
    size_t i = 0;
    size_t size = 0;
    int finish = 0;
 
    assert (NULL != heap);
    assert (NULL != ShouldRemove);
       
    size = DVECSize((dvec_t *)heap->vector);
    
    if (1 == HeapSize(heap))
    {
        DVECPopBack(heap->vector);
        
        return 0;
    }
    for (; i < size && !finish; ++i)
    {
        void **current = DVECGetItemAddress(heap->vector, i);
        int result = ShouldRemove(*current, key, NULL);
        
        if (result)
        {
            void **last_child_address = FindAddress(heap, size - 1);
            
            if (last_child_address == current)
            {                
                DVECPopBack(heap->vector);
                
                return 1;
            }
            
            finish = 1;
            Swap(last_child_address, current);
            DVECPopBack(heap->vector);
            
            if (RootIsBiggerThanCurChild(heap, i))
            {
                HeapifyDown(heap, i);
            }
            else
            {
                HeapifyUp(heap, i);
            }
            
            return 0;
        }
    }
    
    return 1;
}
