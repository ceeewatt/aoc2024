#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_ROWS  (140)
#define NUM_COLS  (140)

// Plus one for terminating null character
#define SIZE_ROWS (NUM_ROWS + 1)
#define SIZE_COLS (NUM_COLS + 1)

struct crossword_search {
  char crossword[SIZE_ROWS][SIZE_COLS];
  char map[SIZE_ROWS][SIZE_COLS];
  int state;
};

bool crossword_search_update(struct crossword_search *cs, char letter, int row, int col);
void crossword_search_reset_state(struct crossword_search *cs);
int crossword_search_diagonal(struct crossword_search *cs);
int crossword_search_off_diagonal(struct crossword_search *cs);
void crossword_print_map(struct crossword_search *cs);

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Missing file name in second argument position\n");
    return EXIT_FAILURE;
  }

  char *filename = argv[1];
  FILE *f = fopen(filename, "r");

  if (f == NULL) {
    printf("Zoinks\n");
    return EXIT_FAILURE;
  }

  struct crossword_search cs;
  cs.state = 0b0000;

  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      cs.map[r][c] = '.';
    }
  }

  int row = 0;
  while (fgets(&cs.crossword[row][0], SIZE_ROWS, f)) {
    // Discard trailing newline character at end
    //  of each row.
    (void)fgetc(f);

    row++;
  }

  int xmas_count = 0;
  char letter;

  // Traverse vertically downward each column
  for (int c = 0; c < NUM_COLS; c++) {
    for (int r = 0; r < NUM_ROWS; r++) {
      letter = cs.crossword[r][c];

      if (crossword_search_update(&cs, letter, r, c)) {
        xmas_count++;
      }
    }

    crossword_search_reset_state(&cs);
  }

  // Traverse horizontally across each row left-to-right
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      letter = cs.crossword[r][c];

      if (crossword_search_update(&cs, letter, r, c)) {
        xmas_count++;
      }
    }

    crossword_search_reset_state(&cs);
  }

  xmas_count += crossword_search_diagonal(&cs);
  xmas_count += crossword_search_off_diagonal(&cs);

  printf("XMAS count: %d\n", xmas_count);

  //crossword_print_map(&cs);

  return EXIT_SUCCESS;
}

bool crossword_search_update(struct crossword_search *cs, char letter, int row, int col)
{
  static int idx[4][2] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}
  };

  bool match = false;

  switch (letter) {
    case 'X':
      idx[0][0] = row;
      idx[0][1] = col;

      if (cs->state == 0b1110)  cs->state = 0b1111;
      else                      cs->state = 0b0001;
      break;

    case 'M':
      idx[1][0] = row;
      idx[1][1] = col;

      if (cs->state == 0b0001)       cs->state = 0b0011;
      else if (cs->state == 0b1100)  cs->state = 0b1110;
      else                       cs->state = 0b0000;
      break;

    case 'A':
      idx[2][0] = row;
      idx[2][1] = col;

      if (cs->state == 0b0011)       cs->state = 0b0111;
      else if (cs->state == 0b1000)  cs->state = 0b1100;
      else                       cs->state = 0b0000;
      break;

    case 'S':
      idx[3][0] = row;
      idx[3][1] = col;

      if (cs->state == 0b0111)  cs->state = 0b1111;
      else                      cs->state = 0b1000;
      break;

    default:
      cs->state = 0b0000;
      break;
  };

  if (cs->state == 0b1111) {
    match = true;

    // Account for the case where two instances of the word
    //  overlap (e.g.: XMASAMX or SAMXMAS).
    if (letter == 'S') {
      cs->state = 0b1000;
    }
    else if (letter == 'X') {
      cs->state = 0b0001;
    }
    else {
      cs->state = 0b0000;
    }

    cs->map[idx[0][0]][idx[0][1]] = 'X';
    cs->map[idx[1][0]][idx[1][1]] = 'M';
    cs->map[idx[2][0]][idx[2][1]] = 'A';
    cs->map[idx[3][0]][idx[3][1]] = 'S';
  }

  return match;
}

void crossword_search_reset_state(struct crossword_search *cs)
{
  cs->state = 0b0000;
}

// Traverse crossword like so:
// ///
// ///
// ///
int crossword_search_diagonal(struct crossword_search *cs)
{
  int row;
  int col;
  char letter;
  int xmas_count = 0;
  int stripe = 0;

  // Traverse diagonally starting from the top of each column
  for (int i = 0; i < NUM_COLS; i++) {
    col = i;
    row = 0;

    // Column index determines number of cells in the diagonal
    //printf("[%d]: ", stripe);
    for (int j = i; j >= 0; j--) {
      letter = cs->crossword[row][col];

      //printf("%c ", letter);
      if (crossword_search_update(cs, letter, row, col)) {
        xmas_count++;
      }

      row++;
      col--;
    }
    //printf("\n");

    crossword_search_reset_state(cs);
    stripe++;
  }

  // The previous loop only traverses the diagonals starting
  //  from the top of each column. Now, we need to traverse
  //  the remaining diagonals in the bottom portion of the
  //  matrix.
  for (int i = 0; i < (NUM_ROWS - 1); i++) {
    col = NUM_COLS - 1 - i;
    row = NUM_ROWS - 1;

    //printf("[%d] ", stripe);
    for (int j = i; j >= 0; j--) {
      letter = cs->crossword[row][col];

      //printf("%c ", letter);
      if (crossword_search_update(cs, letter, row, col)) {
        xmas_count++;
      }

      row--;
      col++;
    }
    //printf("\n");

    crossword_search_reset_state(cs);
    stripe++;
  }

  return xmas_count;
}

// Traverse crossword like so:
// \\\
// \\\
// \\\
//
int crossword_search_off_diagonal(struct crossword_search *cs)
{
  int row;
  int col;
  char letter;
  int xmas_count = 0;
  int stripe = 0;

  // Traverse diagonally starting from the first element of each
  //  row.
  for (int i = (NUM_ROWS - 1); i >= 0; i--) {
    col = 0;
    row = i;

    //printf("[%d]: ", stripe);
    for (int j = (NUM_ROWS - 1 - i); j >= 0; j--) {
      letter = cs->crossword[row][col];

      //printf("%c ", letter);
      if (crossword_search_update(cs, letter, row, col)) {
        xmas_count++;
      }

      row++;
      col++;
    }
    //printf("\n");

    crossword_search_reset_state(cs);
    stripe++;
  }

  // The previous loop only traverses the bottom triangular
  //  portion of the matrix. Now, we need to traverse the
  //  remaining diagonals starting from column at index 1.
  for (int i = 1; i < NUM_COLS; i++) {
    col = i;
    row = 0;

    //printf("[%d]: ", stripe);
    for (int j = 0; j < (NUM_COLS - i); j++) {
      letter = cs->crossword[row][col];

      //printf("%c ", letter);
      if (crossword_search_update(cs, letter, row, col)) {
        xmas_count++;
      }

      row++;
      col++;
    }
    //printf("\n");

    crossword_search_reset_state(cs);
    stripe++;
  }

  return xmas_count;
}

void crossword_print_map(struct crossword_search *cs)
{
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      printf("%c", cs->map[r][c]);
    }
    printf("\n");
  }
}
