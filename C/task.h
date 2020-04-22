#ifndef TASK_H
#define TASK_H

#include <sys/time.h>

#include "uid.h" /* getuid */

#ifndef FUNC_T
#define FUNC_T
typedef long(*func_t)(void *params); /* returns negative number for remove task, 0 for no change in inverval or positive number for new inverval  */
#endif

typedef struct task task_t;

task_t *TaskCreate(size_t interval, func_t func, void *params);
void TaskDestroy(task_t *task); 
int TaskExecute(task_t *task); /* returns 1 for recurence or 0 for single execution */
time_t TaskGetTime(const task_t *task);
unid_t TaskGetUID(const task_t *task);

#endif
