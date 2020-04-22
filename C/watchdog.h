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
*           Exersice: Watchdog            *
******************************************/
#ifndef _OL69_WATCHDOG
#define _OL69_WATCHDOG

enum status
{
    WD_OK = 0,
    WD_E_MEM = 1, /* memory allocation error */
    WD_E_SEM = 2, /* semaphore creation error */
    WD_E_THREAD = 3, /* thread creation error */
    WD_E_EXEC = 4, /* watchdog process execution error */
    WD_E_SIGACT = 5, /* sigaction error */
    WD_E_INVARGV = 6 /* invalid argument vector */
};

/*
 * Function:  WDStart 
 * --------------------
 *  Tell the watchdog to start watching the application,
 *  restarting it whenever it stops responding or crashes.
 *
 *  argc: the number of arguments in argv.
 * 
 *  argv: The array of arguments you want the watchdog to
 *     start the application with. This should at least
 *     include the name of the application. The last 
 *     element in the array needs to be a NULL pointer.
 * 
 *  wd_id: a unique id for the watchdog. each watchdog 
 *     instance will guard an app with the same combination
 *     of wd_id and program name (full path). wd_id must not
 *     be 0.
 * 
 *  Return value: Returns 0 if the watchdog started running
 *     successfully or one of the error status codes as
 *     mentioned above.
 * 
 *  Time-Complexity: O(1).
 */
int WDStart(int argc, char *argv[], int wd_id);

/*
 * Function:  WDStop 
 * --------------------
 *  Tell the watchdog to stop watching the application.
 * 
 *  Time-Complexity: O(1).
 */
void WDStop();

#endif /* _OL69_WATCHDOG */