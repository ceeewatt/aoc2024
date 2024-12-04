/*
*   Advent of Code Day 2
*
*   Link: https://adventofcode.com/2024/day/2
*
*   Part One:
*   The input file consists of many reports. Each row of data
*    is a report. Each report contains a list of numbers called
*    levels.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEVELS  (10)
#define NUM_REPORTS_INIT  (1000)

struct report {
  int level[MAX_LEVELS];
  int num_levels;
};

bool report_is_safe(const struct report *report);

// Given input array `in` containing `size` elements,
//  calculate the diff array `out`. Each element of
//  the diff array is the difference between adjacent
//  elements of the input array. If the input array
//  contains elements [A0,A1,A2,A3], the diff array
//  contains elements [A1-A0,A2-A1,A3-A2]. The diff
//  array will contain one less element than the input
//  array.
void diff(const int *in, int size, int * const out);

void remove_level(struct report *in, struct report *out, int idx);

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Missing file name in second argument position ");
    return EXIT_FAILURE;
  }

  char *filename = argv[1];
  FILE *f = fopen(filename, "r");

  if (f == NULL) {
    printf("Zoinks\n");
    return EXIT_FAILURE;
  }

  struct report report[NUM_REPORTS_INIT];
  memset(report, 0, sizeof(report));

  char line[100];
  char *token;
  int num_reports = 0;
  int level_idx = 0;
  while (fgets(line, sizeof(line), f)) {
    token = strtok(line, " ");
    while (token) {
      level_idx = report[num_reports].num_levels++;
      report[num_reports].level[level_idx] = atoi(token);
      token = strtok(NULL, " ");
    }

    num_reports++;
  }

  int safe_count = 0;
  bool enable_dampener = true;
  struct report temp;
  for (int i = 0; i < num_reports; i++) {
    if (report_is_safe(&report[i])) {
      safe_count++;
      //printf("Report #%d: safe\n", i);
    }
    else if (enable_dampener) {
      for (int l = 0; l < report[i].num_levels; l++) {
        remove_level(&report[i], &temp, l);

        if (report_is_safe(&temp)) {
          safe_count++;
          //printf("Report #%d: safe with dampener\n", i);
          break;
        }
      }


    }
  }

  printf("%d reports are safe\n", safe_count);

  return EXIT_SUCCESS;
}

bool report_is_safe(const struct report *report)
{
  int d[MAX_LEVELS];
  diff(report->level, report->num_levels, d);

  int elem = 0;
  int positive_count = 0;
  int negative_count = 0;
  for (int i = 0; i < (report->num_levels - 1); i++) {
    elem = d[i];

    // A report is safe if all adjacent levels differ by at
    //  least one and at most three.
    if ((elem == 0) || (abs(elem) > 3)) {
      return false;
    }

    if (elem < 0) {
      positive_count++;
    }
    else if (elem > 0) {
      negative_count++;
    }
  }

  // If the diff values contain a mix of positive and negative
  //  numbers, the corresponding levels are neither in ascending
  //  or descending order and thus are unsafe.
  return !((positive_count > 0) && (negative_count > 0));
}

void diff(const int *in, int size, int * const out)
{
  for (int i = 0; i < (size - 1); i++) {
    out[i] = in[i + 1] - in[i];
  }
}

void remove_level(struct report *in, struct report *out, int idx)
{
  int idx_new = 0;

  for (int i = 0; i < in->num_levels; i++) {
    if (i != idx) {
      out->level[idx_new++] = in->level[i];
    }
  }

  out->num_levels = in->num_levels - 1;
}
