/*
*   Advent of Code Day 3
*
*   Link: https://adventofcode.com/2024/day/3
*
*   Part One:
*   Given a continuous string of garbled data, parse each
*    multiply instruction which takes the form `mul(X,Y)`
*    where X and Y are each 1-3 digit numbers. After having
*    parsed each multiply instruction, calculate the sum
*    of each product.
*   Without regex functionality, I'm left to manually parse
*    each incoming character from the file stream and match
*    it against a specified pattern. To do so, I've a multiply
*    "token", which consists of many "elements" that must
*    appear in order in the input stream. If a stream of
*    characters matches the token, I perform the multiply
*    operation on the two operands and accumulate the result
*    into a final sum.
*
*   Part Two:
*   This part introduces two new instructions:
*   - do(): enables future multiply instructions.
*   - don't(): disable future multiply instructions.
*   Using the same "token" construct from part one, I simply
*    created two new tokens for the new instructions and am
*    matching the input stream against these tokens. If there's
*    a match, I set a flag accordingly to enable/disable
*    future multiply operations.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// An element is a character or characters that appear
//  at least once in the token or upt to `repeat_count`
//  times.
struct element {
  // String of possible characters to match against
  char *c;

  // The number of character in `c`
  int range;

  // Determines how many times this element can be repeated
  int repeat_count;

  // Keep track of how many times we've made a match for
  //  this element.
  int match_count;

  // Store the matched characters here
  char buf[10];
  int buf_idx;

  // True if we've finished successfully matching this element
  bool finished;
};

// A token represents a grouping of characters that we want
//  to search for in a string. Each element must occur in
//  order.
struct token {
  // A list of elements comprising this token
  struct element *element;

  // Current index to the `element` array
  int curr_element;

  // Size of `element` array
  int num_elements;
};

// Attempt to match the character `c` against the multiply
//  instruction token.
// If there's a successful match, return true and place the
//  result of the multiplication operation in `product`.
// Call this function repeatedly with a stream of characters.
bool match_mul(struct token *t, char c, int *product);

// Same as `match_mul()` except match against either the
//  do() or don't() instruction. Return true if there's
//  a successful match.
bool match_instruction_token(struct token *t, char c);

// Reset the token/element state variables
void token_reset(struct token *t);

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

  // A long, ugly block of compound literals for initializing
  //  the tokens :)

  char c1[] = "m";
  struct element e1 = {
    .c = c1,
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  char c2[] = "u";
  struct element e2 = {
    .c = c2,
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  char c3[] = "l";
  struct element e3 = {
    .c = c3,
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  char c4[] = "(";
  struct element e4 = {
    .c = c4,
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  char c5[] = "0123456789";
  struct element e5 = {
    .c = c5,
    .repeat_count = 3,
    .range = 10,
    .match_count = 0
  };
  char c6[] = ",";
  struct element e6 = {
    .c = c6,
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  char c7[] = ")";
  struct element e7 = {
    .c = c7,
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };

  struct element mul_element_list[] = {e1, e2, e3, e4, e5, e6, e5, e7};
  struct token mul_token = {
    .element = mul_element_list,
    .curr_element = 0,
    .num_elements = 8
  };

  char do_instruction[] = "do()";
  struct element do_e1 = {
    .c = &do_instruction[0],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element do_e2 = {
    .c = &do_instruction[1],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element do_e3 = {
    .c = &do_instruction[2],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element do_e4 = {
    .c = &do_instruction[3],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element do_element_list[] = {do_e1, do_e2, do_e3, do_e4};
  struct token do_token = {
    .element = do_element_list,
    .curr_element = 0,
    .num_elements = 4
  };

  char dont_instruction[] = "don't()";
  struct element dont_e1 = {
    .c = &dont_instruction[0],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element dont_e2 = {
    .c = &dont_instruction[1],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element dont_e3 = {
    .c = &dont_instruction[2],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element dont_e4 = {
    .c = &dont_instruction[3],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element dont_e5 = {
    .c = &dont_instruction[4],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element dont_e6 = {
    .c = &dont_instruction[5],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element dont_e7 = {
    .c = &dont_instruction[6],
    .repeat_count = 1,
    .range = 1,
    .match_count = 0
  };
  struct element dont_element_list[] = {dont_e1, dont_e2, dont_e3, dont_e4, dont_e5, dont_e6, dont_e7};
  struct token dont_token = {
    .element = dont_element_list,
    .curr_element = 0,
    .num_elements = 7
  };

  int c = 0;
  int product = 0;
  int sum_of_products = 0;
  bool mul_enabled = true;

  // Read the input stream one character at a time. Check
  //  the character against the specified tokens to find
  //  any `mul()`, `do()`, or `don't()` instructions. Only
  //  process a multiply instruction if it follows a `do()`
  //  instruction (multiply instructions are enabled until
  //  a `don't()` instruction is encountered).
  while ((c = fgetc(f)) != EOF) {
    if (match_instruction_token(&do_token, c)) {
      mul_enabled = true;
    }

    if (match_instruction_token(&dont_token, c)) {
      mul_enabled = false;
    }

    if (match_mul(&mul_token, c, &product)) {
      if (mul_enabled) {
        sum_of_products += product;
      }
    }
  }

  printf("Sum of products: %d\n", sum_of_products);

  return EXIT_SUCCESS;
}

void match_element(struct token *t, char c) {
  struct element *e = &t->element[t->curr_element];
  bool match = false;
  bool repeat = (e->repeat_count > 1);

  for (int i = 0; i < e->range; i++) {
    if (c == e->c[i]) {
      match = true;
      break;
    }
  }

  if (match) {
    if (repeat) {
      if (++e->match_count > e->repeat_count) {
        token_reset(t);
      }
      else {
        e->buf[e->buf_idx++] = c;
      }
    }
    else {
      e->buf[e->buf_idx++] = c;
      e->finished = true;

      t->curr_element =
        (t->curr_element < (t->num_elements - 1)) ?
        t->curr_element + 1 :
        0;
    }
  }
  else {
    if (repeat) {
      if ((e->match_count >= 1) && (e->match_count <= e->repeat_count)) {
        e->finished = true;

        t->curr_element =
          (t->curr_element < (t->num_elements - 1)) ?
          t->curr_element + 1 :
          0;

        match_element(t, c);
      }
      else {
        token_reset(t);
      }
    }
    else {
      token_reset(t);
    }
  }
}

bool match_mul(struct token *t, char c, int *product)
{
  bool token_parsed = false;

  match_element(t, c);

  if (t->element[t->num_elements - 1].finished) {
    *product = atoi(t->element[4].buf) * atoi(t->element[6].buf);

    token_parsed = true;
    token_reset(t);
  }

  return token_parsed;
}

bool match_instruction_token(struct token *t, char c)
{
  bool token_parsed = false;

  match_element(t, c);

  if (t->element[t->num_elements - 1].finished) {
    token_parsed = true;
    token_reset(t);
  }

  return token_parsed;
}

void token_reset(struct token *t)
{
  struct element *e;
  for (int i = 0; i < t->num_elements; i++) {
    e = &t->element[i];
    e->match_count = 0;
    e->buf_idx = 0;
    memset(e->buf, 0, sizeof(e->buf));
    e->finished = false;
  }

  t->curr_element = 0;
}
