/****************************************************
            Author: Yael Lotan
            Reviewer: Alex Zilberman
*****************************************************/

#include <assert.h> /* assert */
#include <time.h> /* time */
#include <stdlib.h> /* malloc */

#include "task.h" /* task header file */
#include "uid.h" /* uid header file */

struct task
{
	func_t func;
	void *params;
	unid_t uid;
	size_t interval;
	time_t time_to_run;
};

task_t *TaskCreate(size_t interval, func_t func, void *params)
{   
    task_t *new_task = (task_t *)malloc(sizeof(task_t));
    if (NULL == new_task)
    {
        return NULL;
    }
    
    new_task->func = func;
    new_task->params = params;
    new_task->uid = UIDCreate();
    new_task->interval = interval;
    new_task->time_to_run = time(NULL) + (time_t)interval;
    
    return new_task;
}

void TaskDestroy(task_t *task)
{   
    free(task);
}

int TaskExecute(task_t *task)
{
    long result = 0;  
    if (time(NULL) <= task->time_to_run)
    {
        result = task->func(task->params);
        
        if (0 > result)
        {
            return 0;
        }
        
        if (0 < result)
        {
            task->interval = result;
        }
     } 
      
    task->time_to_run = time(NULL) + (time_t)task->interval;
    
    return 1; 
}

time_t TaskGetTime(const task_t *task)
{   
    return task->time_to_run;
}

unid_t TaskGetUID(const task_t *task)
{    
    return task->uid;
}
