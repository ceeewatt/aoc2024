/*
*   Advent of Code Day 1
*
*   Link: https://adventofcode.com/2024/day/1
*
*   Part One:
*   Given a file containing two columns of data, find the total
*    distance between the left column and the right column. That
*    is, pair up the smallest number from the left column with
*    the smallest number from the right column and find the
*    difference. Repeat for the second-smallest number and so on.
*    Find the total distance by summing the distances between
*    each pair.
*   Start by parsing the input file and saving it to a buffer.
*    I'm using a dynamically-sized buffer because the input file
*    could potentially have any length of data.
*   Then, sort each column of the data in ascending order. For
*    this, I'm using the standard library function `qsort`. By
*    sorting both columns, the pairs are already created. Now,
*    we can simply loop over the each row and find the sum of
*    the distances.
*
*   Part Two:
*   With the same two columns of data, now we want to find the
*    total similarity score by adding up each number in the left
*    lift after multiplying it by the number of times that number
*    appears in the right list.
*   For each number in the left list, I'm performing a binary
*    search for the number in the right list and counting the
*    number of occurrences.
*/

#include <stdio.h>
#include <stdlib.h>

#define DYNAMIC_BUF_INIT_SIZE  (500)

struct dynamic_buf {
  int idx;
  int max;
  int *buf;
};

int dynamic_buf_init(struct dynamic_buf *dbuf, int size);
int dynamic_buf_insert(struct dynamic_buf *dbuf, int elem);
int dynamic_buf_resize(struct dynamic_buf *dbuf);
void dynamic_buf_cleanup(struct dynamic_buf *dbuf);

// Used by qsort to determine how to sort two elements
int cmp(const void *a, const void *b);

// Given a list of 'size' elements sorted in ascending order,
//  return the number of occurrences of target in that list.
int binary_search(int *list, int size, int target);

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

  // Left and right columns of input file
  struct dynamic_buf left;
  struct dynamic_buf right;

  if (dynamic_buf_init(&left, DYNAMIC_BUF_INIT_SIZE) ||
      dynamic_buf_init(&right, DYNAMIC_BUF_INIT_SIZE))
  {
    printf("Zoinks\n");
    return EXIT_FAILURE;
  }

  // Parse input file; exit early if we encounter an error
  int a, b;
  while (!feof(f)) {
    fscanf(f, "%d   %d\n", &a, &b);

    if (dynamic_buf_insert(&left, a) ||
        dynamic_buf_insert(&right, b))
    {
      fclose(f);
      dynamic_buf_cleanup(&left);
      dynamic_buf_cleanup(&right);
      return EXIT_FAILURE;
    }
  }

  // Sort the lists in ascending order
  qsort(left.buf, left.idx, sizeof(int), cmp);
  qsort(right.buf, right.idx, sizeof(int), cmp);

  // Now that we've sorted both of the columns, find the
  //  distance between each ID and take the sum of these
  //  distances.
  int sum = 0;
  for (int i = 0; i < left.idx; i++) {
    sum += abs(left.buf[i] - right.buf[i]);
  }

  printf("Total distance: %d\n", sum);

  int count = 0;
  int similarity_score = 0;
  for (int i = 0; i < left.idx; i++) {
    count = binary_search(right.buf, right.idx, left.buf[i]);
    similarity_score += (left.buf[i] * count);
  }

  printf("Total similarity score: %d\n", similarity_score);

  fclose(f);
  dynamic_buf_cleanup(&left);
  dynamic_buf_cleanup(&right);

  return EXIT_SUCCESS;
}

int dynamic_buf_init(struct dynamic_buf *dbuf, int size)
{
  dbuf->buf = (int *)malloc(sizeof(int) * size);
  dbuf->idx = 0;
  dbuf->max = size;

  return (dbuf->buf == NULL);
}

int dynamic_buf_insert(struct dynamic_buf *dbuf, int elem)
{
  int ret = 0;

  if (dbuf->idx == dbuf->max) {
    // Reached end of the buffer so resize it
    ret = dynamic_buf_resize(dbuf);
  }

  if (dbuf->buf == NULL) {
    ret = 1;
  }
  else {
    dbuf->buf[dbuf->idx++] = elem;
  }

  return ret;
}

int dynamic_buf_resize(struct dynamic_buf *dbuf)
{
  dbuf->buf = realloc(dbuf->buf, sizeof(int) * dbuf->max * 2);

  return (dbuf->buf == NULL);
}

void dynamic_buf_cleanup(struct dynamic_buf *dbuf)
{
  dbuf->idx = 0;
  dbuf->max = 0;
  free(dbuf->buf);
}

int cmp(const void *a, const void *b)
{
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;

    if (arg1 < arg2) return -1;
  if (arg1 > arg2) return 1;
  return 0;
}

int binary_search(int *list, int size, int target)
{
  // Indices for the left and right elements used for
  //  bisecting the array.
  int L = 0;
  int R = size;

  // Midpoint
  int m = 0;

  // Number of occurrences of target
  int count = 0;

  while (L < R) {
    // The midpoint is the floor between the left and right
    //  endpoints.
    m = (int)((L + R) / 2);
    
    if (list[m] < target) {
      L = m + 1;
    }
    else {
      R = m;
    }
  }

  while ((list[L] == target) && (L < size)) {
    count++;
    L++;
  }

  return count;
}
