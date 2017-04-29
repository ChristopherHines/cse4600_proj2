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
      wait,
      response;
};

void fifo(struct Processes process[51], int total);
void print(struct Processes process[51], int total);

int main (int argc, char **argv) {
  if(argc == 1) {
    printf("Invalid Execute: Please state scheduler\nex. ./a.out fifo\n");
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
  //totalProcesses--;
  rewind(fp);

  // Build 2D process array
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

  print(process, totalProcesses);

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
*/
}

void print(struct Processes process[51], int total) {
  int i;
  for(i = 1; i <= total; i++) {
    printf("P%d %d %d %d\n", process[i].job, 
                             process[i].arrival, 
                             process[i].runtime, 
                             process[i].priority);
  }
}