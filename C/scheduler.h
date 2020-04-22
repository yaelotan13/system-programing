#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "uid.h"

#ifndef FUNC_T
#define FUNC_T
typedef long(*func_t)(void *params); /* returns negative number for remove task, 0 for no change in inverval or positive number for new inverval  */
#endif

typedef struct scheduler scd_t;

/* create a new scheduler, return vlaue is the new scheduler */
scd_t *ScdCreate();
/* destroy a scheduler */
void ScdDestroy(scd_t *scheduler);
/* adds a new task to the scheduler, returns the uid of the added task */
unid_t ScdAdd(scd_t *scheduler, time_t interval, func_t func, void *params); 
/* remove a certain task, scheduler should not be NULL, func should not be NULL.
Returns 0 on success or 1 on failure (uid not found) */
int ScdRemove(scd_t *scheduler, unid_t task_uid);
/* run the scheduler, scheduler should not be NULL.
Returns the number of remaining tasks in the queue */ 
size_t ScdRun(scd_t *scheduler);
/* stop the scheduler. scheduler should not be NULL. */
void ScdStop(scd_t *scheduler);
/* get the number of tasks in the scheduler, scheduler should not be NULL. */
size_t ScdSize(const scd_t *scheduler);
/* check if the scheduler is empty, scheduler should not be NULL.
Retuns 1 if it is, 0 if it is not empty */
int ScdIsEmpty(const scd_t *scheduler);

#endif

