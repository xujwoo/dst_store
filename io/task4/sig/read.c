/************************************************************************
   > File Name:    write.c
   > Author:       Lin
   > Mail:         linbb@farsight.cn
   > Created Time: Mon 10 Oct 2016 11:09:16 AM CST
*******************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
void handler(int sig)
{

}
typedef struct shmbuf
{
	pid_t pid;
	char buf[64];
}SHM;
int main(int argc, const char *argv[])
{
	SHM *shmb;
	key_t key;
	pid_t opid;
	int shmid;
	signal(SIGUSR1,handler);
	key = ftok("..",250);
	if(-1 == key)
	{
		perror("ftok");
		return -1;
	}
	shmid = shmget(key,sizeof(SHM),IPC_CREAT|IPC_EXCL|0666);
	if(-1 == shmid)
	{
		if(errno == EEXIST)
		{
			shmid = shmget(key,sizeof(SHM),0);
		}else
		{
			perror("shmget");
			return -1;
		}
	}
	printf("shmid = %d\n",shmid);
	if(-1 == *(int *)(shmb = (SHM *)shmat(shmid,NULL,0)))
	{
		perror("shmat");
		return -1;
	}
	opid = shmb->pid;
	shmb->pid = getpid();
	kill(opid,SIGUSR1);
	while(1)
	{
		pause();
		fprintf(stdout,"%s",shmb->buf);
		if(0 == strncmp(shmb->buf,"quit",4))
			break;
		memset(shmb->buf,'\0',64);
		kill(opid,SIGUSR1);
//		fgets(shmb->buf,64,stdin);
	}
	if(0 != shmctl(shmid,IPC_RMID,NULL))
	{
		perror("shmctl");
		return -1;
	}

	return 0;
}
