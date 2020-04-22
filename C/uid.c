/*******************************************
            Author: Yael Lotan
            Reviewer: Alex Zilberman
*******************************************/

#include <unistd.h> /* getpid */
#include <sys/time.h> /* gettimeofday */
#include <pthread.h> /* pthread_t */

#include "uid.h"

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
const unid_t bad_uid;

unid_t UIDCreate()
{
    static size_t counter = 0;
    unid_t unid;
    unid.pid = getpid();
    gettimeofday(&unid.time, NULL);
    pthread_mutex_lock(&counter_mutex);
    unid.counter = ++counter;
    pthread_mutex_unlock(&counter_mutex);
    
    return unid; 
}

int UIDIsSame(unid_t uid1, unid_t uid2)
{
    int is_same = 0;
    
    is_same = uid1.pid == uid2.pid
              && uid1.counter == uid2.counter
              && uid1.time.tv_sec == uid2.time.tv_sec
              && uid1.time.tv_usec == uid2.time.tv_usec;
    
    return is_same;
}

int UIDIsBad(unid_t uid)
{
    int is_same = 0;
    
    UIDIsSame(uid, bad_uid);
              
    return is_same;
}


