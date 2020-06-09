#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
 
#include<bits/stdc++.h> 
using namespace std;

int timer=0;
int tempTimerAddOrRemove=0;


void handler(int signum)
{
 
 timer=timer+1;

tempTimerAddOrRemove=tempTimerAddOrRemove-1;

 alarm(1);
killpg(getpgrp(),SIGUSR2);
   
}
void handler1(int signum)
{

}
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


int main()
{
  key_t msgqidProccesses;
  key_t msgqidDiskUP;
  key_t msgqidDiskDown;

  msgqidProccesses = msgget(IPC_PRIVATE, 0644); 
  msgqidDiskUP = msgget(IPC_PRIVATE, 0644); 
  msgqidDiskDown = msgget(IPC_PRIVATE, 0644);

  if(msgqidProccesses == -1)
  {	
    perror("Error in create");
    exit(-1);
  }
 if( msgqidDiskUP== -1)
  {	
    perror("Error in create");
    exit(-1);
  }
if( msgqidDiskDown== -1)
  {	
    perror("Error in create");
    exit(-1);
  }

 

int processescount=1;
int pid;
int index;

for(int i=0;i<processescount;i++)
    { 
	pid=fork() ;
        index=i;
        if(pid==0) 
        { 
            break;
           
        } 
       sleep(1);
    } 
  
  if (pid== -1)
  	perror("error in fork");
  	
  else if (pid== 0)
 	{ 
		char msgqidProccessestemp[100];
		sprintf(msgqidProccessestemp,"%d",msgqidProccesses);
		char indextemp[100];
		sprintf(indextemp,"%d",index);

		char*args[]={"/home/nour/Downloads/operatingsys/labs/messagequeue/processss.o",msgqidProccessestemp,indextemp,NULL};
		execv(args[0],args);
		
  	}   
  else 	
  	{
                
		int diskpid;

		diskpid=fork() ;
		
		if (diskpid== -1)
		  	perror("error in fork");
		  	
		  else if (diskpid== 0)
		{ 
			
			char  msgqidDiskUPtemp[100];
			sprintf( msgqidDiskUPtemp,"%d", msgqidDiskUP);
			char   msgqidDiskDowntemp[100];
			sprintf( msgqidDiskDowntemp,"%d",  msgqidDiskDown);
 
			char*args[]={"/home/nour/Downloads/operatingsys/labs/messagequeue/disk.o", msgqidDiskUPtemp, msgqidDiskDowntemp ,NULL};
			execv(args[0],args);
			
  		}   
		else
		{
			sleep(1);
			signal (SIGALRM, handler);
			signal (SIGUSR2, handler1);
			ofstream out("Logs.txt");
			alarm(1);
                        
			std::queue<processmsg> ourQueue;	

			

                        while(1)
			{//
				int rec_val;
				struct processmsg message;
				
				// receive all types of messages 
				rec_val = msgrcv(msgqidProccesses, &message, sizeof(message)-sizeof(message.mtype), 0, IPC_NOWAIT);  
				  
				if(rec_val == -1);	
				else
				{
				string tempop;
				if(message.addOrRemove==0)
					tempop="add";
				else
					tempop="del";
				   out<<"at time "<<timer<<"  required operation "<<tempop<<endl;
				    ourQueue.push(message);
				
				}	

				
				if(tempTimerAddOrRemove<=0&&!ourQueue.empty())
				{//
					
					struct processmsg messagetemp=ourQueue.front();
					ourQueue.pop();
					kill(diskpid,SIGUSR1);

					int rec_val2;
					struct discstatus status;
					  
					// receive all types of messages 
					rec_val2 = msgrcv( msgqidDiskUP, &status, sizeof(status)-sizeof(status.mtype), 0, !IPC_NOWAIT); 
					int freeslots=0;
					for(int h=0;h<10;h++)
						if(status.slots[h]==0)
							freeslots++;
					out<<"at time "<<timer<<"  number of free slots in disk ="<<freeslots<<endl;
					if(messagetemp.addOrRemove==0)//add
					{//
						bool checkslots=false;
						int l;
						for(l=0;l<10;l++)
							if(status.slots[l]==0)
							{
								checkslots=true;
								break;
							}
						if(checkslots)
						{//
							int send_val;
							tempTimerAddOrRemove=3;
							send_val = msgsnd( msgqidDiskDown, &messagetemp, sizeof(messagetemp)-		sizeof(messagetemp.mtype), IPC_NOWAIT);
	 					out<<"at time "<<timer<<" successful add is made at slot "<<l<<endl;
						}//
						else
						{
						out<<"at time "<<timer<<" unsuccessful add "<<endl;
						}


					}//
					if(messagetemp.addOrRemove==1)//remove
					{//
						
						bool checkslots=false;
						if(status.slots[messagetemp.mtext[0]-48]==1)//filled
							checkslots=true;
						if(checkslots)
						{//
							int send_val;
							tempTimerAddOrRemove=1;
							send_val = msgsnd( msgqidDiskDown, &messagetemp, sizeof(messagetemp)-		sizeof(messagetemp.mtype), IPC_NOWAIT);
	  
					out<<"at time "<<timer<<" successful del is made at slot "<<messagetemp.mtext[0]-48<<endl;
						}//
						else
						{out<<"at time "<<timer<<" unsuccessful delete "<<endl;
						}

					}// 


				}// 
					

			}// 
			out.close(); 
		}

 	}
return 0;
}




