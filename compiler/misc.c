#ifndef MISC_C
#define MISC_C

#include <stdio.h>
#include "global.h"

// Print out fatal messages
void fatal(char *s) {
    fprintf(stderr, "%s on line %d\n", s, Line); exit(1);
  }
  
  void fatals(char *s1, char *s2) {
    fprintf(stderr, "%s:%s on line %d\n", s1, s2, Line); exit(1);
  }
  
  void fatald(char *s, int d) {
    fprintf(stderr, "%s:%d on line %d\n", s, d, Line); exit(1);
  }
  
  void fatalc(char *s, int c) {
    fprintf(stderr, "%s:%c on line %d\n", s, c, Line); exit(1);
  }

#endif // MISC_C