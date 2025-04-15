#ifndef extern_
#define extern_ extern
#endif

#include <stdio.h>  // Include this to define the FILE type

#define TEXTLEN		512	// Length of symbols in input

extern_ int Putback;
extern_ int Line;
extern_ FILE *Outfile;
extern_ char Text[TEXTLEN + 1];		// Last identifier scanned
