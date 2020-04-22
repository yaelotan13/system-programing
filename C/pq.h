#ifndef _PQ_H
#define _PQ_H

#include "srtl.h" /* it_t */

/*struct pq
{
 srtl_t *list;
};*/

typedef struct pq pq_t;
typedef int(*pqpriority_f)(const void *data, const void *new_data, const void *params);
typedef int(*pqcmp_f)(const void *data, const void *key, const void *params);

pq_t *PQCreate(pqpriority_f PriorityFunction);
void PQDestroy(pq_t *pq);
int PQEnqueue(pq_t *pq, void *data);
void *PQPeek(const pq_t *pq);
int PQDequeue(pq_t *pq);
int PQIsEmpty(const pq_t *pq);
size_t PQSize(const pq_t *pq);
void PQClear(pq_t *pq);
int PQErase(pq_t *pq, const void *key, pqcmp_f ShouldErase);

#endif  
