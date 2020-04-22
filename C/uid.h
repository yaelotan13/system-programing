#ifndef UID_H
#define UID_H

#include <sys/time.h>
#include <sys/types.h>

typedef struct unid
{
	pid_t pid;
	struct timeval time;
	size_t counter;
} unid_t;

/* if the creation of task failes, this bad_uid is returned */ 
extern const unid_t bad_uid; 

/* creates a new uid for the task, returns the uid */
unid_t UIDCreate();
/* checks if two uids are the same, returns 1 if they are the same 
and 0 if the are not the same */
int UIDIsSame(unid_t uid1, unid_t uid2);
/* checks if a certain uid is a bad_uid, returns 1 if it is, 0 if it is not */
int UIDIsBad(unid_t uid);

#endif

