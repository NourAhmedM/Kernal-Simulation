#include <bits/stdc++.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

int counter=0;

void handler1 (int sig_num) {
	counter++;
}

struct processmsg
{
   long mtype;
   int addOrRemove; //0 if add and 1 if remove 
   char mtext[64];
};

struct instruction {

	int execution_time;
	string command;
	string data;

	bool operator < (const instruction & a)const {
        return execution_time < a.execution_time;
        }

        bool operator > (const instruction & a)const {
        return execution_time > a.execution_time;
        }
};

struct cmp_Arrival {
	bool operator()(const instruction & a, const instruction & b) { 
		return a.execution_time > b.execution_time;
	}
};


int main (int argc, char* argv[]) {

        signal( SIGUSR2, handler1);

        key_t mailbox_id = atoi(argv[1]);
        char * proc_index = argv[2];

	//concatenate to get the file name

	strcat(proc_index, ".txt");
        //cout << "after concatination "<< proc_index << endl; 
        
        ifstream infile(proc_index);
        
        int x;
        string y,z;

        priority_queue<instruction, vector<instruction>, cmp_Arrival>pq;

        while (! infile.eof()){
                

		infile >> x >> y;
                infile.ignore();
                getline( infile, z);
                if( infile.eof() ) break;
                instruction temp;
                temp.execution_time = x ;
		temp.command = y;
		temp.data = z;  // to be handled that z may be more than one word
                pq.push(temp);
              
                // for the sake of checking
                //cout << temp.execution_time << " " << temp.command << " " << temp.data << endl;
	}

        // for the sake of checking
        //cout << pq.size()  << endl;





        while (!pq.empty()) {

		 instruction temp = pq.top();
		 if (temp.execution_time <= counter) {  //this 5 is to be changed
			 pq.pop();

			 processmsg sent;
			 sent.mtype=1;
			 sent.addOrRemove = (temp.command == "ADD") ? 0 : 1;

                         // for the sake of checking
                         //cout << temp.data<< endl;
                         //cout << sent.message << endl;
			 memset(&sent.mtext[0], 0, sizeof(sent.mtext));  // to clear the char array before reusing it
                         if (sent.addOrRemove  == 0)
                         {
                            //cout << temp.data << endl;
                            temp.data = temp.data.substr (1, temp.data.size()-2);
                            temp.data.copy(sent.mtext, temp.data.size() );
                            //cout << sent.message << endl;
                         }
                         else if ( sent.addOrRemove  == 1 ) 
                         { 
                            //cout << temp.data << endl;
                            sent.mtext[0] = temp.data[1];
                            //cout << sent.message << endl;
                         }

                         


                         // for the sake of checking
			 //cout << "sent mtype   " << sent.mtype << endl;
			 //cout << "sent command   " << sent.add_or_remove	 << endl;
			 //cout << "sent message   " << sent.message << endl;
			 

			 msgsnd(mailbox_id, &sent, sizeof(sent)-sizeof(sent.mtype), !IPC_NOWAIT);
		 }
	 }

}

