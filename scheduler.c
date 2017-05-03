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
void srt(struct Processes process[51], int total);
void print(struct Processes process[51], int total, char *scheduler, int switches);

int main (int argc, char **argv) {
  if(argc == 1) {
    printf("Invalid Execute: Please state scheduler\n");
    printf("usage ./a.out [fifo, srt, rr]\n");
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

  printf("Total %d\n", totalProcesses);

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
  else if (strcmp(scheduler, "srt") == 0) {
    printf("Running SRT\n");
  srt(process, totalProcesses);
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
/* 1) Make a ready queue
   2) Look at first process runtime
   3) Put all processes within runtime in ready queue then sort by priority
   
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
  int switches = 0;

  process[1].start = 0;
  process[1].endtime = process[1].runtime;
  process[1].turnaround = process[1].endtime;
  process[1].waittime = 0;
  process[1].response = 0;
  current = process[1];
  for(i = 2; i <= 50; i++) {
    clock += current.runtime;
    process[i].start = clock;
    process[i].endtime = clock + process[i].runtime;
    process[i].turnaround = process[i].endtime - process[i].arrival;
    process[i].waittime = process[i].start - process[i].arrival;
    process[i].response = process[i].start - process[i].arrival;
    current = process[i];
  }

  print(process, total, "FIFO", switches);
}

void rr(struct Processes process[51], int total) {
/*Start time = Clock time at arrival
   End time = Clock time when process ends
   Turnaround = wait time + runtime
   Wait time = time since process arrived but not running
   Response time = Start time - Arrival time
*/
  int i, j;
  int runtime[51];
  for(i = 1; i <= total; i++){ //create list of the runtimes to find 80th percentile to determine Quantum
    runtime[i] = process[i].runtime;
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
    switches++; //if process arrived and is not finished increment switches
    
    if(process[i].arrival <= clock && process[i].start == -1) //process arrived & hasn't started, mark start time
      process[i].start = clock;
    
    deltaT = (process[i].runtime < quantum ? process[i].runtime : quantum); //advance runtime up to 1 quantum
    runningT[i] += deltaT;
    clock += deltaT;
    
    for(j = 1; j <= total; j++){ //update the wait time for every relevant process
      if(i != j && clock > process[j].arrival && runningT[j] < process[j].runtime){ //if the process has arrived and not finished and not running
        if(waitT[j] == 0 && process[j].arrival > process[i].arrival){ 
          waitT[j] += deltaT - process[j].arrival; //advance it's wait time by the difference in the clock
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

void srt(struct Processes process[51], int total) {
  //job, arrival, runtime, priority, start, endtime, turnaround, wait, response
  int i, j, k, r, u;
  int clock = 20;
  int counter = 0;
  int smallest = 1; //keep track of shortest runtime process that have arrived
  int done[50] = {0};
  struct Processes current;
  int switches = 0;
  
  process[1].start = 0;
  process[1].endtime = process[1].runtime;
  process[1].turnaround = process[1].endtime;
  process[1].waittime = 0;
  process[1].response = 0;
  current = process[1];
  
  for(r=1; r<=total; r++)//goes through all 50 processes
  {
    for(k=1; k<=total; k++)//figures out how many processes have arrived
    {
      if(process[k].arrival <= clock)
        counter++;//how many processes have arrived each time
    }
    printf("\nProcesses to go through the first time:%d \n", counter);
    u = 9000;
    for(j=2; j<=counter; j++) //only goes through what has arrived
    {
      if(done[j]==0)
      {
        if(process[j].arrival <= clock) //process has arrived
        {
          if(u >= process[j].runtime) //if there is something shorter than the current
          {
            u = process[j].runtime;//if we find something smaller we store the runtime
            smallest = j;//take track of what process has the shortest remaining time
          }
        }
        printf("\nsmallest: %d\n", smallest);
      }
      //printf("\nsmallest: %d\n", smallest);
      //done[smallest] = 1; 
    }
    done[smallest] = 1;
    clock = clock + process[smallest].runtime;
    
    process[smallest].start = clock - process[smallest].arrival; 
    process[smallest].waittime = process[smallest].start - process[smallest].arrival;
    process[smallest].endtime = process[smallest].start + process[smallest].runtime;
    process[smallest].turnaround = process[smallest].endtime - process[smallest].arrival;
    process[smallest].response = process[smallest].start - process[smallest].arrival;
    current = process[smallest];
    
    counter = 0;
    
  }
  print(process, total, "SRT", switches);
}

void print(struct Processes process[51], int total, char *scheduler, int switches) {
  int i;
  int avgTT = 0, avgWT = 0, avgRT = 0;
  printf("===> %s\n", scheduler);
  printf("JOB\t ARIV\t RUNT\t PRIO\t STRT\t ENDT\t TURN\t WAIT\t RESP\t\n");
  printf("---------------------------------------------------------------------\n");
  for(i = 1; i <= total; i++) {
    printf("P%2d\t %3d\t %3d\t %2d\t %4d\t %4d\t %4d\t %4d\t %4d\n",
               process[i].job,
               process[i].arrival, 
               process[i].runtime, 
               process[i].priority,
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
  printf("Average Turnaround Time\t\t= %6d\n", avgTT);
  printf("Average Waiting Time\t\t= %6d\n", avgWT);
  printf("Average Response Time\t\t= %6d\n", avgRT);
  printf("Number Context Switches\t\t= %6d\n", switches);
}

























