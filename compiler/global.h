#ifndef extern_
#define extern_ extern
#endif

#include <stdio.h>  // Include this to define the FILE type
#include "defs.h"

#define TEXTLEN		512	    // Length of symbols in input
#define NSYMBOLS	1024	// Number of symbols in symbol table

extern_ int Putback;
extern_ int Line;
extern_ FILE *Outfile;
extern_ int Functionid;                 // Symbol id of the current function
extern_ char Text[TEXTLEN + 1];		    // Last identifier scanned
extern_ struct symtable Gsym[NSYMBOLS];	// Global symbol table
