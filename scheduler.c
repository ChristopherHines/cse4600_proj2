/*
*    Names: Sam Akardet Tancharoensuksavai
*           Stephen Kenney
*           Christopher Hines
*    Course: 4600 Operating Systems
*    Date: 5/4/2017
*    Description: This is a simple program that computes the different
*    variables for assessing the various types of schedulers. This program 
*    accepts an input file of processes (see generateprocesses.c)
*    and generates a schedule according to what you select: FIFO, RR, or sjf.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Processes
{
  int job,
      arrival,
      runtime,
      priority,
      start,
      endtime,
      turnaround,
      waittime,
      response;
};

int comp (const void * elem1, const void * elem2);
void fifo(struct Processes process[51], int total);
void rr(struct Processes process[51], int total);
void sjf(struct Processes process[51], int total);
void print(struct Processes process[51], int total, char *scheduler, int switches);
void sjfprint(struct Processes process[51], int total, char *scheduler);

int main (int argc, char **argv) {
  // Check whether the user imput a specific scheduling algorithm 
  if(argc == 1) {
    printf("Invalid Execute: Please state scheduler\n");
    printf("usage ./a.out [fifo, sjf, rr]\n");
    return 0;
  }

  char *scheduler = argv[1];
  struct Processes process[51];
  char buff[20];
  int val, totalProcesses = 0;

  FILE *fp = fopen("processes.txt", "r");

  // Get the total number of processes in the file then reset file pointer
  while(fgets(buff, 255, (FILE*)fp) != NULL) {
    totalProcesses++;
  }
  rewind(fp);

  // Build struct process array
  int i, j;
  for(i = 1; i <= totalProcesses; i++) {
    process[i].job = i;
    for(j = 0; j <= 3; j++) {
      fscanf(fp, "%s", buff);
      val = strtol(buff, (char **)NULL, 10);
      if(j % 4 == 1) {
        process[i].arrival = val;
      }
      else if(j % 4 == 2) {
        process[i].runtime = val;
      }
      else if(j % 4 == 3) {
        process[i].priority = val;
      }
    }
  }

  if (strcmp(scheduler, "fifo") == 0) {
    printf("Running FIFO\n");
    fifo(process, totalProcesses);
  }
  else if (strcmp(scheduler, "sjf") == 0) {
    printf("Running SJF\n");
    sjf(process, totalProcesses);
  }
  else if (strcmp(scheduler, "rr") == 0) {
    printf("Running RR\n");
    rr(process, totalProcesses);
  }
  else
    printf("ERROR: Invalid Scheduler\n");

  fclose(fp);

return 0;
}

int comp (const void * elem1, const void * elem2) //used to sort runtimes for RR
{
    return (*(int*)elem1 > *(int*)elem2);
}

void fifo(struct Processes process[51], int total) {
/*
   Start time = Clock time
   End time = Clock time + runtime
   Turnaround = Endtime - Arrival time
   Wait time = Start time - Arrival time
   Response time = Start time - Arrival time
*/
  struct Processes current;
  int clock = 0;
  int complete = 0;
  int i;

  // Determine scheduling variables for first process
  process[1].start = 0;
  process[1].endtime = process[1].runtime;
  process[1].turnaround = process[1].endtime;
  process[1].waittime = 0;
  process[1].response = 0;

  // Set the current process to the first process in queue
  current = process[1];

  // Loop through the list of processes ordered in arrival time
  for(i = 2; i <= 50; i++) {
    clock += current.runtime; // Add current process runtime to the clocktime
    process[i].start = clock;
    process[i].endtime = clock + process[i].runtime;
    process[i].turnaround = process[i].endtime - process[i].arrival;
    process[i].waittime = process[i].start - process[i].arrival;
    process[i].response = process[i].start - process[i].arrival;
    current = process[i];
  }

  print(process, total, "FIFO", 0);
}

void rr(struct Processes process[51], int total) {
/*Start time = Clock time at arrival
   End time = Clock time when process ends
   Turnaround = wait time + runtime
   Wait time = time since process arrived but not running
   Response time = Start time - Arrival time
*/
  int i, j;
  int runtime[total];
  for(i = 0; i < total; i++){ //create list of the runtimes to find 80th percentile to determine Quantum
    runtime[i] = process[i + 1].runtime;
  }
  qsort (runtime, sizeof(runtime)/sizeof(*runtime), sizeof(*runtime), comp);
  float index = total * .8; //index for percentile, to be rounded
  int quantum = runtime[(index > (int)index ? (int)index + 1 : (int)index)]; //quantum amount of time each process gets to run
  printf("QUANTUM : %d\n", quantum);
  
  int runningT[51]; //tracked runtime for RR
  int deltaT = 0;
  int switches = 0;
  int waitT[51];
  int clock = 0;
  int complete = 0;
  
  for (i = 1; i <= total; i++){ //set default values
    runningT[i] = 0;
    waitT[i] = 0;
    //process[i].priority = 0;
    process[i].start = -1;
  }
  
  while(complete < total){ //until every process is complete
    for(i = 1; i <= total; i++) {
    if(process[i].arrival > clock || runningT[i] >= process[i].runtime) //if process has not arrived or has finished, skip to the next one
      continue;
    
    if(process[i].arrival <= clock && process[i].start == -1) //process arrived & hasn't started, mark start time
      process[i].start = clock;
    
    deltaT = (process[i].runtime < quantum ? process[i].runtime : quantum); //advance runtime up to 1 quantum
    runningT[i] += deltaT;
    clock += deltaT;
    if(deltaT == quantum)
      switches++;
    
    for(j = 1; j <= total; j++){ //update the wait time for every relevant process
      if(i != j && clock > process[j].arrival && runningT[j] < process[j].runtime){ //if the process has arrived and not finished and not running
        if(waitT[j] == 0 && process[j].arrival > process[i].arrival){ 
          waitT[j] += clock - process[j].arrival; //advance it's wait time by the difference in the clock
        }
        else 
          waitT[j] += deltaT;
      }
    }
    
    if(runningT[i] >= process[i].runtime){ //process completed
      complete++;
      process[i].endtime = clock;
      process[i].turnaround = waitT[i] + process[i].runtime;
      process[i].response = process[i].start - process[i].arrival;
      process[i].waittime = waitT[i];
    }
    }
  }
  print(process, total, "RR", switches);
}

void sjf(struct Processes process[51], int total) {
  //job, arrival, runtime, priority, start, endtime, turnaround, wait, response
  int i, j, k, r, u;//I explain what these are used for when they are used in loops
  int clock = 0;
  int counter = 0;
  int smallest = 1; //keep track of shortest runtime process that have arrived
  int done[51] = {0};//this keeps track of what processes have completed
  struct Processes current;
  
  process[1].start = 0;
  process[1].endtime = process[1].runtime;
  process[1].turnaround = process[1].endtime;
  process[1].waittime = 0;
  process[1].response = 0;
  process[1].priority = 0;
  current = process[1];
  
  for(r=1; r<=total; r++)//goes through all 50 processes
  {
    for(k=1; k<=total; k++)//figures out how many processes have arrived
    {
      if(process[k].arrival <= clock)
        counter++;//how many processes have arrived each time a process finishes
    }
    u = 9000;
    for(j=2; j<=counter; j++) //only goes through what has arrived
    {
      if(done[j]==0)
      {
        if(u >= process[j].runtime) //if there is something shorter than the current
        {
          u = process[j].runtime;//if we find something smaller we store the runtime
          smallest = j;//take track of what process has the shortest remaining time
        }
      } 
    }
    done[smallest] = 1;//mark the smallest runtime process as 'done'

    process[smallest].start = clock;//record the start time before we increment
    clock = clock + process[smallest].runtime;
    
    process[smallest].waittime = process[smallest].start - process[smallest].arrival;
    process[smallest].endtime = process[smallest].start + process[smallest].runtime;
    process[smallest].turnaround = process[smallest].endtime - process[smallest].arrival;
    process[smallest].response = process[smallest].start - process[smallest].arrival;
    process[smallest].priority = r;
    current = process[smallest];  
    counter = 0;
    
    if(r==1)//if we are on the first process
      {
        process[1].endtime = process[1].runtime;
        process[1].turnaround = process[1].endtime;
        process[1].waittime = 0;
        process[1].response = 0;
      }
  }
  
  //averages
  float turn_average;
  int sum = 0;
  for(i=1; i<=total; i++)
  {
    sum = sum + process[i].turnaround;
    
  }
  turn_average = (float)sum/(float)total;
  
  float wait_average;
  sum = 0;
  for(i=1; i<=total; i++)
  {
    sum = sum + process[i].waittime;
    
  }
  wait_average = (float)sum/(float)total;
  
  float resp_average;
  sum = 0;
  for(i=1; i<=total; i++)
  {
    sum = sum + process[i].response;
  }
  resp_average = (float)sum/(float)total;
  
  sjfprint(process, total, "SJF");//print processes
  
  printf("---------------------------------------------------------------------\n");
  printf("Average Turnaround Time\t\t= %f\n", turn_average);
  printf("Average wait Time \t\t= %f\n", wait_average);
  printf("Average Response Time \t\t= %f\n", resp_average);
  printf("Number of Context Switches \t= 0  (non-preemptive)\n");
}

void print(struct Processes process[51], int total, char *scheduler, int switches) {
  int i;
  float avgTT = 0, avgWT = 0, avgRT = 0;
  printf("===> %s\n", scheduler);
  printf("JOB\t ARIV\t RUNT\t PRIO\t STRT\t ENDT\t TURN\t WAIT\t RESP\t\n");
  printf("---------------------------------------------------------------------\n");
  for(i = 1; i <= total; i++) {
    printf("P%2d\t %3d\t %3d\t    0\t %4d\t %4d\t %4d\t %4d\t %4d\n",
               process[i].job,
                             process[i].arrival, 
                             process[i].runtime, 
                             //process[i].priority,
                             process[i].start,
                             process[i].endtime,
                             process[i].turnaround,
                             process[i].waittime,
                             process[i].response);
   avgTT += process[i].turnaround;
   avgWT += process[i].waittime;
   avgRT += process[i].response;
  }
  avgTT /= total;
  avgWT /= total;
  avgRT /= total;
  printf("---------------------------------------------------------------------\n");
  printf("Average Turnaround Time\t\t= %6f\n", avgTT);
  printf("Average Waiting Time\t\t= %6f\n", avgWT);
  printf("Average Response Time\t\t= %6f\n", avgRT);
  printf("Number Context Switches\t\t= %4d\n", switches);
}

void sjfprint(struct Processes process[51], int total, char *scheduler) {
  int i;
  printf("===> %s\n", scheduler);
  printf("'LIST' is the order that the processes are run in.\n");
  printf("JOB\t ARIV\t RUNT\t LIST\t STRT\t ENDT\t TURN\t WAIT\t RESP\t\n");//list is the run order of the processes
  printf("---------------------------------------------------------------------\n");
  for(i = 1; i <= total; i++) {
    printf("P%2d\t %3d\t %3d\t %3d\t %4d\t %4d\t %4d\t %4d\t %4d\n", 
               process[i].job, 
                             process[i].arrival, 
                             process[i].runtime, 
                             process[i].priority,
                             process[i].start,
                             process[i].endtime,
                             process[i].turnaround,
                             process[i].waittime,
                             process[i].response);
  }
}