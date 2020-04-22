/****************************************************
            Author: Yael Lotan
            Reviewer: Alex Zilberman
*****************************************************/

#include <stdlib.h> /* malloc */
#include <time.h> /* time */
#include <unistd.h> /* sleep */
#include <assert.h> /* assert */
#include <errno.h> /* errno */

#include "scheduler.h" /* scheduler header file */
#include "task.h" /* task header file */
#include "pq.h" /* pq header file */

#define UNUSED(x) (void)(x)

struct scheduler
{
	pq_t *task_list;
	int should_stop;	
};

int IsBefore(const void *data, const void *new_data, const void *params)
{
	UNUSED(params);

	return (TaskGetTime(new_data) < TaskGetTime(data)) ? 1 : 0;
}

int ShouldEraseAndRemove(const void *data, const void *key, const void *params)
{
	int is_same = UIDIsSame(TaskGetUID(data), *(unid_t *)key);	
	UNUSED(params);
	
	if (is_same)
	{
		TaskDestroy((void*)data);
	}

	return is_same;
}

scd_t *ScdCreate()
{
    scd_t *scheduler = (scd_t *)malloc(sizeof(scd_t));
    if (NULL == scheduler)
    {
        return NULL;
    }
    
	scheduler->task_list = PQCreate(IsBefore);
    if (NULL == scheduler->task_list)
    {
        free(scheduler);
        
        return NULL;
    }
    
    scheduler->should_stop = 0;
    
    return scheduler;
}

void ScdDestroy(scd_t *scheduler)
{
    assert(NULL != scheduler);
    
	while (!ScdIsEmpty(scheduler))
	{
		TaskDestroy(PQPeek(scheduler->task_list));
		PQDequeue(scheduler->task_list);
	}

    PQDestroy(scheduler->task_list);
	free(scheduler);
}

int ScdRemove(scd_t *scheduler, unid_t task_uid)
{
    assert(NULL != scheduler);
    
    return PQErase(scheduler->task_list, &task_uid, ShouldEraseAndRemove);
}

size_t ScdSize(const scd_t *scheduler)
{
    assert(NULL != scheduler);
    
    return PQSize(scheduler->task_list);
}

int ScdIsEmpty(const scd_t *scheduler)
{
    assert(NULL != scheduler);
    
    return PQIsEmpty(scheduler->task_list);
}

void ScdStop(scd_t *scheduler)
{
    assert(NULL != scheduler);
    
    scheduler->should_stop = 1;
}

unid_t ScdAdd(scd_t *scheduler, time_t interval, func_t func, void *params)
{
	task_t *new_task = TaskCreate(interval, func, params);

    assert(NULL != scheduler);
    assert(NULL != func);
    
	if (NULL == new_task)
	{
		return bad_uid;
	}

	PQEnqueue(scheduler->task_list, new_task);

	return TaskGetUID(new_task);	
}

size_t ScdRun(scd_t *scheduler)
{
    int pq_worked = 0;
	task_t *task_to_run = NULL;
	time_t sleep_time = 0;
	time_t remaining_time = 0;
	scheduler->should_stop = 0;
	
    assert(NULL != scheduler);
    
	while (!(scheduler->should_stop) && !(ScdIsEmpty(scheduler)))
	{ 
		task_to_run = PQPeek(scheduler->task_list);
		sleep_time = TaskGetTime(task_to_run) - time(NULL);
		if (0 < sleep_time)
		{
		    remaining_time = sleep_time;
		    while (remaining_time)
		    {
		        remaining_time = sleep(sleep_time);
		    }
		}

		if (TaskExecute(task_to_run))
		{
			pq_worked = PQEnqueue(scheduler->task_list, task_to_run);
			if (pq_worked)
			{
			    errno = ENOMEM;
			    ScdStop(scheduler);
			}
			else
			{ 
			    PQDequeue(scheduler->task_list);
			}
		}
		else 
		{
		    PQDequeue(scheduler->task_list);
			TaskDestroy(task_to_run);
		}	
	}
	
	return ScdSize(scheduler);
}
