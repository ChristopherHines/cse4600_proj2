#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Function to randomly generate a number that is a multiple of 5 */
int randomNumber5() {
  int n = 1;

  while (n % 5 != 0) {
    n = rand() % 300;
  } 

  return n;
}

/* Comparison function for qSort.
   Sorts by arrival time of processes array*/
int comp (const void * elem1, const void * elem2) 
{
    int f = *((int*)elem1+1);
    int s = *((int*)elem2+1);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

int main (int argc, char **argv) {
  int processes[51][5]; // 2D array that holds 50 processeses and its variables
  int arrival, burst, priority;
  int i, j;

  FILE *f = fopen("processes.txt", "w");
  srand(time(NULL));
  
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  // Generate 50 random processes
  for(i = 1; i <= 50; i++) {
    j = 0;
    arrival = randomNumber5();
    burst = randomNumber5() + 5;
    priority = rand() % 20 + 1;
    
    processes[i][j] = i;
    processes[i][j+1] = arrival;
    processes[i][j+2] = burst;
    processes[i][j+3] = priority;
  }

  // Sort by arrival time
  qsort (processes, sizeof(processes)/sizeof(*processes), sizeof(*processes), comp);

  processes[1][1] = 0;
  processes[1][2] = 20;
  processes[1][3] = 1;
  // Write processes to file
  for(i = 1; i <= 50; i++) {
    j = 0;
    processes[i][j] = i;
    fprintf(f, "P%d %d %d %d\n", processes[i][j], processes[i][j+1], processes[i][j+2], processes[i][j+3]);
  }

  fclose(f);

return 0;
}
