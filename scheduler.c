#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fifo(int processes[][5], int total);

int main (int argc, char **argv) {
  if(argc == 1) {
    printf("Invalid Execute: Please state scheduler\nex. ./a.out fifo\n");
    return 0;
  }

  char *scheduler = argv[1];
  int processes[51][5];
  char buff[20];
  int val, totalProcesses = 0;

  FILE *fp = fopen("processes.txt", "r");

  // Get the total number of processes in the file then reset file pointer
  while(fgets(buff, 255, (FILE*)fp) != NULL) {
    totalProcesses++;
  }
  totalProcesses--;
  rewind(fp);

  // Build 2D process array
  int i, j;
  for(i = 1; i <= totalProcesses; i++) {
    processes[i][0] = i;
    for(j = 0; j <= 3; j++) {
      fscanf(fp, "%s", buff);
      
      if(j % 4 != 0) {
        val = strtol(buff, (char **)NULL, 10);
        processes[i][j] = val;
      }
    }
  }


  if (strcmp(scheduler, "fifo") == 0) {
    printf("Running FIFO\n");
    fifo(processes, totalProcesses);
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

void fifo(int processes[][5], int total) {
/* 1) Make a ready queue
   2) Look at first process runtime
   3) Put all processes within runtime in ready queue then sort by priority
*/
  int readyQueue[51];
  int processChart[51][10];

}