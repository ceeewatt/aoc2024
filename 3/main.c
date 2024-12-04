#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct element {
  // An element is a character or characters that appear
  //  at least once in the token or up to `repeat_count`
  //  times.
  char *c;
  int range;
  int repeat_count;
  int match_count;
  char buf[10];
  int buf_idx;
  bool finished;
};

struct token {
  // An token represents a grouping of characters that we
  //  want to search for in a string. Each element must
  //  occur in order.
  struct element *element;
  int curr_element;
  int num_elements;
};

bool match_mul(struct token *t, char c, int *product);
bool match_instruction_token(struct token *t, char c);
void token_reset(struct token *t)
{
  //printf("TOKEN RESET\n");
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
      else {
        //printf("Ignoring prior multiplication instruction\n");
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

  //printf("(Element #%d) ", t->curr_element);
  //printf("(Repeat: %d) ", repeat);
  //printf("(Match count: %d) ", e->match_count);
  //printf("Analyzing char %c\n", c);

  for (int i = 0; i < e->range; i++) {
    if (c == e->c[i]) {
      match = true;
      //printf("\tMatch %c against one of [%s]\n", c, e->c);
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

      //printf("\tMatch count: %d\n", e->match_count);
    }
    else {
      e->buf[e->buf_idx++] = c;
      e->finished = true;

      //printf("\tElement %d buf: %s\n", t->curr_element, e->buf);

      t->curr_element =
        (t->curr_element < (t->num_elements - 1)) ?
        t->curr_element + 1 :
        0;

      //printf("\tMoving to next element %d\n", t->curr_element);
    }
  }
  else {
    if (repeat) {
      if ((e->match_count >= 1) && (e->match_count <= e->repeat_count)) {
        e->finished = true;
        //printf("\tElement %d buf: %s\n", t->curr_element, e->buf);

        // next element and reprocess this same character
        t->curr_element =
          (t->curr_element < (t->num_elements - 1)) ?
          t->curr_element + 1 :
          0;

        //printf("\tMoving to next element %d\n", t->curr_element);
        //printf("\tMaking recursive call to match_element\n");

        match_element(t, c);
      }
      else {
        //printf("\t(Match count: %d) Match count too high or too low for this element, reset token\n", e->match_count);
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
    //printf("Parsed: ");
    //for (int e = 0; e < t->num_elements; e++) {
    //  printf("%s", t->element[e].buf);
    //}

    *product = atoi(t->element[4].buf) * atoi(t->element[6].buf);

    //printf(" = %d\n", *product);
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
