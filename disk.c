#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include<bits/stdc++.h> 
using namespace std;

void handler(int signum);
key_t msgqUpid,msgqDownid;
void sendToKernel(int msgqUpid);
void checkOnDownQueue(int msgqDownid);
int timer = 0;

struct processmsg
{
   long mtype;
   int addOrRemove; //0 if add and 1 if remove 
   char mtext[64];
};

struct discstatus
{
   long mtype;
   int slots[10]; 
   
};
processmsg messagesArr[10];
discstatus slotsStatus;

int main(int argc,char ** argv)
{

  for(int i=0;i<10;i++)
  {
	messagesArr[i].mtext[0]='\0';
	slotsStatus.slots[i]=0;
  }
  signal (SIGUSR2, handler);
  signal (SIGUSR1, handler);
  msgqUpid = atoi(argv[1]);
  msgqDownid = atoi(argv[2]);
  
  while(1)
  {
	checkOnDownQueue(msgqDownid);
  }
}
	

void sendToKernel(key_t msgqUpid)
{
	// loop on all the message slots
	int i = 0;
	for(i = 0; i < 10 ; i++)
	{
		// check on the empty slots
		if(messagesArr[i].mtext[0] == '\0')
			slotsStatus.slots[i] = 0;
		else
			slotsStatus.slots[i] = 1;
			
	}
	slotsStatus.mtype=1;
	int send_val = msgsnd(msgqUpid, &slotsStatus, sizeof(slotsStatus.slots), IPC_NOWAIT);
       
  	
}


void checkOnDownQueue(key_t msgqDownid)
{
	int rec_val;
  	struct processmsg queueData;
  
  /* receive all types of messages */
  rec_val = msgrcv(msgqDownid, &queueData, sizeof(queueData) - sizeof(queueData.mtype), 0, IPC_NOWAIT);  
  
  if(rec_val == -1);
 
  //in case of the queue is full i.e. have value
  else
  {
	
	// in case of add
	if(queueData.addOrRemove == 0)
	{
		
		int i = 0;
		for(i = 0; i < 10 ; i++)
		{
                       
			// check on the empty slots
			if(messagesArr[i].mtext[0] == '\0')
			{	
				strcpy(messagesArr[i].mtext,queueData.mtext);
				slotsStatus.slots[i] = 1;
				break;
			}		
		}	
	}
	else
	{
		// removing phase
		int i = 0;
		for(i = 0; i < 10; i++)
		{
			// check on the full slots
			if(queueData.mtext[0] == i+48)
			{
				messagesArr[i].mtext[0] = '\0';
				slotsStatus.slots[i] = 0;
				break;
			}		
		}
	}
	
  }
}


void handler(int signum)
{

 	if(signum == 12)
	{
		timer++;
		
 	} 
 	if(signum == 10)	
	{
		
		//send to kernel the message through the up stream queue
		sendToKernel(msgqUpid);
	}
}

