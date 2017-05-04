/*
*    Names: Sam Akardet Tancharoensuksavai
*           Stephen Kenney
*           Christopher Hines
*    Course: 4600 Operating Systems
*    Date: 5/4/2017
*    Description: This is a simple program that creates a text file of 
*                 fifty random processes
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Function to randomly generate a number that is a multiple of 5 */
int randomNumber5() {
  return (rand() % 60) * 5;
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
  int i;

  FILE *f = fopen("processes.txt", "w");
  srand(time(NULL));
  
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  // Generate 50 random processes
  for(i = 0; i < 50; i++) {
    arrival = randomNumber5();
    burst = randomNumber5() + 5;
    priority = rand() % 20 + 1;
    
    processes[i][0] = i;
    processes[i][1] = arrival;
    processes[i][2] = burst;
    processes[i][3] = priority;
  }

  // Sort by arrival time
  qsort (processes, sizeof(processes)/sizeof(*processes), sizeof(*processes), comp);

  // Write processes to file
  for(i = 0; i < 50; i++) {
    processes[i][0] = i+1;
    fprintf(f, "P%d %d %d %d\n", processes[i][0], processes[i][1], processes[i][2], processes[i][3]);
  }

  fclose(f);

return 0;
}
