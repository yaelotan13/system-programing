/**********************************
		Author: Yael Lotan
		Reviewer: Sasha Limarev
**********************************/

#define _GNU_SOURCE

#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc */
#include <unistd.h> /* sleep */
#include <signal.h> /* sigaction */
#include <sys/sem.h> /* semop */
#include <pthread.h> /* pthread_create */
#include <string.h> /* strcpy */

#include "scheduler.h" /* scheduler header */
#include "watchdog.h" /* watchdog header */

#define NUM_OF_SEMAPHORES (3)
#define SEND_SIGUSR1_INTERVAL (1)
#define CHECK_SIGUSR1_INTERVAL (4)
#define ADDITIONAL_ARGUMENTS (3)
#define BUFFER_SIZE (11)
#define WD_FILE ("./wd")
#define CURRENT_FILE_NAME ("./watchdog.c")
#define NUM_OF_OPERATIONS (1)
#define UNUSED(x) ((void)(x))

typedef struct sembuf sembuf_t;

int g_sig1_counter = 0;
pid_t g_wd_pid = 0;
int g_semid = 0;
int g_recevied_sigusr2 = 0;
int g_stop = 0;
key_t g_key = 0;
char **g_wd_argv;

static void FreeArgv(char **argv)
{
	free(argv[0]);
	free(argv[1]);
	free(argv);
}

static long SendSigusr1(void *params)
{
	UNUSED(params);

	if (g_stop)
	{
		return -1;
	}

	printf("\033[1;32mapp sends SIGUSR1 to %d\033[0m\n", g_wd_pid);
	kill(g_wd_pid, SIGUSR1);

	return WD_OK;
}

static long CheckSigusr1(void *params)
{
	char **new_argv = (char **)params;
	sembuf_t op_wait2 = {2, -1, 0};

	if (g_stop)
	{
		return -1;
	}

	if (0 == g_sig1_counter) /* WD is dead, raise it again */
	{
		printf("watchdog is dead\n");
		g_wd_pid = fork();
		
		if (!g_wd_pid)
		{
			printf("raised wd again\n");
			if (-1 == execv(new_argv[0], new_argv))
			{	
				exit(WD_E_EXEC); 
			} 
			
			exit(1);
		}

		if (-1 == semop(g_semid, &op_wait2, NUM_OF_OPERATIONS)) /* wait for WD is ready */
		{
			printf("op_wait2: semop error\n");

			return WD_E_SEM;
		} 
	}
	else
	{
		g_sig1_counter = 0;
	}

	return WD_OK;
}

static void SIGUSR1_handler()
{
	printf("wd is alive!\n");
	__sync_fetch_and_add(&g_sig1_counter, 1);
}

static void SIGUSR2_handler()
{
	printf("WD confirms his death\n");
	__sync_fetch_and_add(&g_recevied_sigusr2, 1);
}

static void *ManageScheduler(void *params)
{	
	ScdRun((scd_t *)params);
	printf("destroy the scheduler of the app\n");
	ScdDestroy((scd_t *)params);

	g_recevied_sigusr2 += 1;

	return NULL;
}

static char **CreateArgv(int argc, char **argv, int key)
{
	int i = 0, j = 0;
	char *wd_name = "./wd";
	
	g_wd_argv = (char **)malloc(sizeof(char *) * (argc + ADDITIONAL_ARGUMENTS));
	if (!g_wd_argv)
	{
		return NULL;
	}

	g_wd_argv[0] = (char *)malloc(BUFFER_SIZE);
	if (!g_wd_argv[0])
	{
		return NULL;
	}

	strcpy(g_wd_argv[0], wd_name);

	g_wd_argv[1] = (char *)malloc(BUFFER_SIZE);
	if (!g_wd_argv[1])
	{
		return NULL;
	}

	sprintf(g_wd_argv[1], "%d", key);

	j = 2;

	for (i = 0; i < argc; ++i)
	{
		g_wd_argv[i + j] = argv[i];
	}

	g_wd_argv[i + j] = NULL;


	return WD_OK;
}

static int InitDefaultValuesOfSemaphores()
{
	int result = 0;

	result = semctl(g_semid, 0, SETVAL, 0);
	result = semctl(g_semid, 1, SETVAL, 0);
	result = semctl(g_semid, 2, SETVAL, 0);

	return result;
}

static void InitSemaphores(sembuf_t *op_post1, sembuf_t *op_wait2, sembuf_t *op_post2)
{
	op_post1->sem_num = 1;
	op_post1->sem_op = 1; /* add one to app is ready */
	op_post1->sem_flg = 0; 
	
	op_wait2->sem_num = 2;
	op_wait2->sem_op = -1; /* wait for WD is ready to be 1 */
	op_wait2->sem_flg = 0;

	op_post2->sem_num = 2;
	op_post2->sem_op = 1; /* after waiting for WD ready, set it back on to 1 */
	op_post2->sem_flg = 0;
}

static int InitSigHandlers()
{
	struct sigaction sig_user1; 
	struct sigaction sig_user2;

	memset(&sig_user1, 0, sizeof(sig_user1));
	memset(&sig_user2, 0, sizeof(sig_user2));

	sig_user1.sa_sigaction = SIGUSR1_handler;
	sig_user2.sa_sigaction = SIGUSR2_handler;

	if (0 != sigaction(SIGUSR1, &sig_user1, NULL))
	{
		return WD_E_SIGACT;
	}
	if (0 != sigaction(SIGUSR2, &sig_user2, NULL))
	{
		return WD_E_SIGACT;
	}

	return WD_OK;
}

static int CreateOrConnectToSemaphores(int wd_id)
{
	g_key = ftok(CURRENT_FILE_NAME, wd_id);
	if (-1 == g_key)
	{
		return WD_E_SEM;
	}

	g_semid = semget(g_key, NUM_OF_SEMAPHORES, 0666 | IPC_CREAT | IPC_EXCL); 
	if (-1 == g_semid)
	{
		g_semid = semget(g_key, NUM_OF_SEMAPHORES, 0); /* connect to an exsisting semaphore */
		if (-1 == g_semid)
		{
			return WD_E_SEM;
		}

		g_wd_pid = getppid();
	}
	else
	{
		if (-1 == InitDefaultValuesOfSemaphores())
		{
			return WD_E_SEM;
		}
	} 

	return WD_OK;
}

static int AppIsReadyForSignals(sembuf_t *op_post1)
{
	if (-1 == semop(g_semid, op_post1, NUM_OF_OPERATIONS)) /* app is ready */
	{
		return WD_E_SEM;
	}

	return WD_OK;
}

static int CreateWatchdogIfNotAlive()
{
	int wd_sem_value = semctl(g_semid, 0, GETVAL);
	int result = 0;

	if (-1 == wd_sem_value) /* error with semctl */
	{
		result = WD_E_SEM;
	}

	if (0 == wd_sem_value) /* WD is not alive */
	{
		g_wd_pid = fork();
		if (!g_wd_pid)
		{
			printf("create the WD process\n");
			if (-1 == execv(g_wd_argv[0], g_wd_argv)) /* create the WD process */
			{
				result = WD_E_EXEC;
			}

			exit(1);
		}
	}

	return result;
}

static scd_t *CreateSchedulerAndTasks(scd_t *app_scheduler)
{
	unid_t send_sigusr1, check_siguser1_arrived;
	
	app_scheduler = ScdCreate();
	if (NULL == app_scheduler)
	{
		return NULL;
	}

	send_sigusr1 = ScdAdd(app_scheduler, SEND_SIGUSR1_INTERVAL, SendSigusr1, NULL);
	if (UIDIsBad(send_sigusr1))
	{
		return NULL;
	}

	check_siguser1_arrived = ScdAdd(app_scheduler, CHECK_SIGUSR1_INTERVAL, CheckSigusr1, g_wd_argv);
	if (UIDIsBad(check_siguser1_arrived))
	{
		return NULL;
	}

	return app_scheduler;
}

static int WaitFotWatchdogToBeReady(sembuf_t *op_wait2, sembuf_t *op_post2)
{
	if (-1 == semop(g_semid, op_wait2, NUM_OF_OPERATIONS)) /* wait for WD is ready */
	{
		return WD_E_SEM;
	} 

	if (-1 == semop(g_semid, op_post2, NUM_OF_OPERATIONS)) /* post WD is ready */
	{
		return WD_E_SEM;
	}

	return WD_OK;
}

static int CreateThread(scd_t *app_scheduler)
{
	pthread_t wd_thread;

	if (-1 == pthread_create(&wd_thread, NULL, ManageScheduler, app_scheduler))
	{
		return WD_E_THREAD;
	}

	if (-1 == pthread_detach(wd_thread))
	{
		return WD_E_THREAD;
	}

	return WD_OK;
}

int WDStart(int argc, char *argv[], int wd_id)
{
	int result = 0;
	sembuf_t op_post1, op_wait2, op_post2;
	scd_t *app_scheduler = NULL;
	
	if (WD_OK != InitSigHandlers())
	{
		return WD_E_SIGACT;
	} 

	if (WD_OK != CreateOrConnectToSemaphores(wd_id))
	{
		return WD_E_SEM;
	}

	if (WD_OK != CreateArgv(argc, argv, g_key)) /* create argv for the WD process */
	{
		return WD_E_MEM;
	}
	
	InitSemaphores(&op_post1, &op_wait2, &op_post2);

	if (WD_OK != AppIsReadyForSignals(&op_post1))
	{
		return WD_E_SEM;
	}

	result = CreateWatchdogIfNotAlive(); /* there are two different reasons for failure */
	if (WD_OK != result)
	{
		return result;
	}

	app_scheduler = CreateSchedulerAndTasks(app_scheduler);
	if (!app_scheduler)
	{
		return WD_E_MEM;
	}

	if (WD_OK != WaitFotWatchdogToBeReady(&op_wait2, &op_post2))
	{
		return WD_E_SEM;
	}

	if (WD_OK != CreateThread(app_scheduler))
	{
		return WD_E_THREAD;
	}

	return WD_OK;
}

void WDStop()
{
	int number_of_seconds = 40;
	int i = 0;

	g_stop = 1;

	while (i < number_of_seconds && 2 > g_recevied_sigusr2) /* make sure it recived back SIGUSER2 and also freed all the resources */
	{
		printf("send SIGUSR2 to WD\n");
		kill(g_wd_pid, SIGUSR2);
		sleep(1);
		++i;
	}
	if (!g_recevied_sigusr2)
	{
		kill(g_wd_pid, SIGKILL);
	}

	FreeArgv(g_wd_argv);

	if(-1 == semctl(g_semid, 0, IPC_RMID, 0)) /* destroy the semaphors */
	{
		exit(-1); 
	}
}