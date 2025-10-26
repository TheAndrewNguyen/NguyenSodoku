// Sudoku puzzle verifier and solver

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include<stdlib.h> 
#include <string.h> 

#define NUM_THREADS 3 // number of threads im using

// takes puzzle size and grid[][] representing sudoku puzzle
// and two booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
typedef struct {
  int size; // n x n size 
  int **grid; 
} parameters;

typedef struct {
  bool valid;
  bool complete; 
} Result;

// checks all the rows in the sodoku puzzle for completeness and validity 
void* checkRows(void* arg) {
  parameters *p = (parameters*) arg; 
  int size = p->size + 1; 
  Result* result = malloc(sizeof(Result));

  //error handling 
  if(!result) {
    perror("malloc failed");
    pthread_exit(NULL); 
  }

  // preset is true unless we solve otherwise 
  result->valid = true; 
  result->complete = true; 
  
  // check all the rows
  for(int row = 1; row < size; row++) { // row
    bool seen[size + 1]; 
    memset(seen, false, sizeof(seen)); 

    // check all the cols
    for(int col = 1; col < size; col++) {

      int num = p->grid[row][col]; 

      // completeness check 
      if(num == 0) {
        result->complete = false; 
      }
      
      // validity check 
      if(seen[num] == true) {
        result->valid = false; 
      }

      // update seen 
      seen[num] = true; 
      
      // early exit if both values are false 
      if(result->valid == false && result->complete == false) { 
        pthread_exit((void*) result); 
      }
    }
  }

  // return findings
  pthread_exit((void*) result); 
}

// checks all the columns for validity and completeness
void* checkColumns(void* arg) {
  parameters *p = (parameters*) arg; 
  Result* result = malloc(sizeof(Result));
  int size = p->size + 1; 

  //error handling 
  if(!result) {
    perror("malloc failed");
    pthread_exit(NULL); 
  }

  // preset is true 
  result->valid = true; 
  result->complete = true; 

  for(int col = 1; col < size; col++) {
    bool seen[size + 1]; 
    memset(seen, false, sizeof(seen)); 

    for(int row = 1; row < size; row++) {
      int num = p->grid[col][row]; 
      if(num == 0) {
        result->complete = false; 
      }

      if(seen[num] == true) {
        result->valid = false; 
      }

      if(result->valid == false && result->complete == false) {
        pthread_exit((void*) result); 
      }

      seen[num] = true; 
    }
  }

  // check every element in the row
  pthread_exit((void*) result); 
}


void* checkBoxes(void* arg) {
  parameters *p = (parameters*) arg; 
  Result* result = malloc(sizeof(Result));

  //error handling 
  if(!result) {
    perror("malloc failed");
    pthread_exit(NULL); 
  }

  // preset is true unless we solve otherwise 
  result->valid = true; 
  result->complete = true; 

  int hello = p->grid[1][2]; 
  printf("hello from thread 3: %d\n", hello); 
  // check every element in the row
  pthread_exit((void*) result); 
}

//MAIN threadder 
void checkPuzzle(int psize, int **grid, bool *complete, bool *valid) {
  
  // base 
  *valid = true;
  *complete = true;

  // parameters 
  parameters param; 
  param.size = psize; 
  param.grid = grid; 

  // threads
  pthread_t threads[NUM_THREADS];
  
  Result* results[NUM_THREADS];
  void* (*threadFuncs[NUM_THREADS])(void*) = {checkRows, checkColumns, checkBoxes};

  // create the threads 
  for(int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, threadFuncs[i], &param); 
  }

  // join the threads
  for(int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], (void**)&results[i]); 
  }

  // result from thread 1:
  printf("ROW CHECK valid: %d complete: %d \n", results[0]->valid, results[0]->complete); 
  printf("COL CHECK valid: %d complete: %d \n", results[1]->valid, results[1]->complete); 
}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char *filename, int ***grid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
  int psize;
  fscanf(fp, "%d", &psize);
  int **agrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    agrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }
  fclose(fp);
  *grid = agrid;
  return psize;
}

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int **grid) {
  printf("%d\n", psize);
  for (int row = 1; row <= psize; row++) {
    for (int col = 1; col <= psize; col++) {
      printf("%d ", grid[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int **grid) {
  for (int row = 1; row <= psize; row++) {
    free(grid[row]);
  }
  free(grid);
}

// expects file name of the puzzle as argument in command line
int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./sudoku puzzle.txt\n");
    return EXIT_FAILURE;
  }
  // grid is a 2D array
  int **grid = NULL;
  // find grid size and fill grid
  int sudokuSize = readSudokuPuzzle(argv[1], &grid);
  bool valid = false;
  bool complete = false;
  checkPuzzle(sudokuSize, grid, &complete, &valid);
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(sudokuSize, grid);
  deleteSudokuPuzzle(sudokuSize, grid);
  return EXIT_SUCCESS;
}
