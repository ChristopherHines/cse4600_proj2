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

void fifo(struct Processes process[51], int total);
void print(struct Processes process[51], int total);

int main (int argc, char **argv) {
  if(argc == 1) {
    printf("Invalid Execute: Please state scheduler\n");
    printf("usage ./a.out [fifo, srt, sjf]\n");
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

  //print(process, totalProcesses);

  if (strcmp(scheduler, "fifo") == 0) {
    printf("Running FIFO\n");
    fifo(process, totalProcesses);
  }
  else if (strcmp(scheduler, "srt") == 0) {
    printf("Running SRT\n");
  }
  else if (strcmp(scheduler, "sjf") == 0) {
    printf("Running SJF\n");
  }
  else
    printf("ERROR: Invalid Scheduler\n");

  fclose(fp);

return 0;
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

  print(process, total);
}

void print(struct Processes process[51], int total) {
  int i;
  printf("===> FIFO\n");
  printf("JOB\t ARIV\t RUNT\t PRIO\t STRT\t ENDT\t TURN\t WAIT\t RESP\t\n");
  printf("---------------------------------------------------------------------\n");
  for(i = 1; i <= total; i++) {
    printf("P%2d\t %3d\t %3d\t %2d\t %4d\t %4d\t %4d\t %4d\t %4d\n", process[i].job, 
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