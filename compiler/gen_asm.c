#ifndef GEN_ASM_C
#define GEN_ASM_C

#include "global.h"
#include "defs.h"
#include "misc.c"

// List of available registers
// and their names
static int freereg[4];
static char *reglist[4]= { "%r8", "%r9", "%r10", "%r11" };
static char *breglist[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };

// Set all registers as available
void freeall_registers(void)
{
  freereg[0]= freereg[1]= freereg[2]= freereg[3]= 1;
}

// Allocate a free register. Return the number of
// the register. Die if no available registers.
static int alloc_register(void)
{
  for (int i=0; i<4; i++) {
    if (freereg[i]) {
      freereg[i]= 0;
      return(i);
    }
  }
  fprintf(stderr, "Out of registers!\n");
  exit(1);
}

// Return a register to the list of available registers.
// Check to see if it's not already there.
static void free_register(int reg)
{
  if (freereg[reg] != 0) {
    fprintf(stderr, "Error trying to free register %d\n", reg);
    exit(1);
  }
  freereg[reg]= 1;
}

// Print out the assembly preamble
void cgpreamble() {
  freeall_registers();
  fputs("\t.text\n"
	".LC0:\n"
	"\t.string\t\"%d\\n\"\n"
	"printint:\n"
	"\tpushq\t%rbp\n"
	"\tmovq\t%rsp, %rbp\n"
	"\tsubq\t$16, %rsp\n"
	"\tmovl\t%edi, -4(%rbp)\n"
	"\tmovl\t-4(%rbp), %eax\n"
	"\tmovl\t%eax, %esi\n"
	"\tleaq	.LC0(%rip), %rdi\n"
	"\tmovl	$0, %eax\n"
	"\tcall	printf@PLT\n" "\tnop\n" "\tleave\n" "\tret\n" "\n", Outfile);
}

// Print out a function preamble
void cgfuncpreamble(char *name) {
  fprintf(Outfile,
	  "\t.text\n"
	  "\t.globl\t%s\n"
	  "\t.type\t%s, @function\n"
	  "%s:\n" "\tpushq\t%%rbp\n"
	  "\tmovq\t%%rsp, %%rbp\n", name, name, name);
}

// Print out a function postamble
void cgfuncpostamble() {
  fputs("\tmovl	$0, %eax\n" "\tpopq	%rbp\n" "\tret\n", Outfile);
}

// Load an integer literal value into a register.
// Return the number of the register
int cgloadint(int value, int type) {

  // Get a new register
  int r= alloc_register();

  // Print out the code to initialise it
  fprintf(Outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
  return(r);
}

int cgloadglob(int id) {
  // Get a new register
  int r = alloc_register();

  // Print out the code to initialise it: P_CHAR or P_INT
  if (Gsym[id].type == P_INT)
    fprintf(Outfile, "\tmovq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
  else
    fprintf(Outfile, "\tmovzbq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
  return (r);
}

// Generate a global symbol
void cgglobsym(int id) {
  if (Gsym[id].type ==P_INT)
    fprintf(Outfile, "\t.comm\t%s,8,8\n", Gsym[id].name);
  else
    fprintf(Outfile, "\t.comm\t%s,1,1\n", Gsym[id].name);
}

// Store a register's value into a variable
int cgstorglob(int r, int id) {
  if (Gsym[id].type == P_INT)
    fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], Gsym[id].name);
  else
    fprintf(Outfile, "\tmovb\t%s, %s(\%%rip)\n", breglist[r], Gsym[id].name);
  return (r);
}

// Widen the value in the register from the old
// to the new type, and return a register with
// this new value
int cgwiden(int r, int oldtype, int newtype) {
  // Nothing to do
  return (r);
}

// Add two registers together and return
// the number of the register with the result
int cgadd(int r1, int r2) {
  fprintf(Outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r1);
  return(r2);
}

// Subtract the second register from the first and
// return the number of the register with the result
int cgsub(int r1, int r2) {
  fprintf(Outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
  free_register(r2);
  return(r1);
}

// Multiply two registers together and return
// the number of the register with the result
int cgmul(int r1, int r2) {
  fprintf(Outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
  free_register(r1);
  return(r2);
}

// Divide the first register by the second and
// return the number of the register with the result
int cgdiv(int r1, int r2) {
  fprintf(Outfile, "\tmovq\t%s,%%rax\n", reglist[r1]);
  fprintf(Outfile, "\tcqo\n");
  fprintf(Outfile, "\tidivq\t%s\n", reglist[r2]);
  fprintf(Outfile, "\tmovq\t%%rax,%s\n", reglist[r1]);
  free_register(r2);
  return(r1);
}

// Call printint() with the given register
void cgprintint(int r) {
  fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
  fprintf(Outfile, "\tcall\tprintint\n");
  free_register(r);
}

// Compare two registers.
static int cgcompare(int r1, int r2, char *how) {
  fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\t%s\n", how, breglist[r2]);
  fprintf(Outfile, "\tandq\t$255,%s\n", reglist[r2]);
  free_register(r1);
  return (r2);
}

int cgequal(int r1, int r2) { return(cgcompare(r1, r2, "sete")); }
int cgnotequal(int r1, int r2) { return(cgcompare(r1, r2, "setne")); }
int cglessthan(int r1, int r2) { return(cgcompare(r1, r2, "setl")); }
int cggreaterthan(int r1, int r2) { return(cgcompare(r1, r2, "setg")); }
int cglessequal(int r1, int r2) { return(cgcompare(r1, r2, "setle")); }
int cggreaterequal(int r1, int r2) { return(cgcompare(r1, r2, "setge")); }

// Generate a label
void cglabel(int l) {
  fprintf(Outfile, "L%d:\n", l);
}

// Generate a jump to a label
void cgjump(int l) {
  fprintf(Outfile, "\tjmp\tL%d\n", l);
}

// List of comparison instructions,
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *cmplist[] =
  { "sete", "setne", "setl", "setg", "setle", "setge" };

// Compare two registers and set if true.
int cgcompare_and_set(int ASTop, int r1, int r2) {

  // Check the range of the AST operation
  if (ASTop < A_EQ || ASTop > A_GE)
    fatal("Bad ASTop in cgcompare_and_set()");

  fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\t%s\n", cmplist[ASTop - A_EQ], breglist[r2]);
  fprintf(Outfile, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
  free_register(r1);
  return (r2);
}

// List of inverted jump instructions,
// in AST order: A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE
static char *invcmplist[] = { "jne", "je", "jge", "jle", "jg", "jl" };

// Compare two registers and jump if false.
int cgcompare_and_jump(int ASTop, int r1, int r2, int label) {

  // Check the range of the AST operation
  if (ASTop < A_EQ || ASTop > A_GE)
    fatal("Bad ASTop in cgcompare_and_set()");

  fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
  fprintf(Outfile, "\t%s\tL%d\n", invcmplist[ASTop - A_EQ], label);
  freeall_registers();
  return (NOREG);
}

static int label(void) {
  static int l = 1;
  return (l++);
}

static int genAST(struct ASTnode *n, int reg, int parentASTop);

// generate IF assembly code
static int genIF(struct ASTnode *n) {
  int Lfalse, Lend;

  Lfalse = label();
  if (n->right)
    Lend = label();
  
  genAST(n->left, Lfalse, n->op);
  freeall_registers();

  genAST(n->mid, NOREG, n->op);
  freeall_registers();

  if (n->right)
    cgjump(Lend);

  cglabel(Lfalse);

  if (n->right) {
    genAST(n->right, NOREG, n->op);
    freeall_registers();
    cglabel(Lend);
  }

  return (NOREG);  
}

static int genWHILE(struct ASTnode *n) {
  int Lstart, Lend;

  Lstart = label();
  Lend = label();

  // set start label and evaluate condition
  // genAST recognises the A_WHILE operator and generates the jump to end
  cglabel(Lstart);
  genAST(n->left, Lend, n->op);
  freeall_registers();

  // evaluate body of while loop
  genAST(n->right, NOREG, n->op);
  freeall_registers();

  // jump back to start of while loop and make end label
  // this is in the body of the assembly code for the while loop.
  cgjump(Lstart);
  cglabel(Lend);

  return (NOREG);  
}

// Given an AST, generate
// assembly code recursively
static int genAST(struct ASTnode *n, int reg, int parentASTop) {
    if (n == NULL) {
      printf("NULL AST node passed to genAST\n");
      exit(0);
    }

    int leftreg, rightreg;

    switch (n->op) {
      case A_IF:
        printf("generating IF assembly code\n");
        return (genIF(n));
      case A_WHILE:
        printf("generating WHILE assembly code\n");
        return (genWHILE(n));
      case A_GLUE:
        printf("generating for glued AST nodes\n");
        genAST(n->left, NOREG, n->op);
        freeall_registers();
        genAST(n->right, NOREG, n->op);
        freeall_registers();
        return (NOREG);
      case A_FUNCTION:
        // Generate the function's preamble before the code
        printf("generating for function %s\n", Gsym[n->v.id].name);
        cgfuncpreamble(Gsym[n->v.id].name);
        genAST(n->left, NOREG, n->op);
        cgfuncpostamble();
        return (NOREG);
    }

    // Get the left and right sub-tree values
    if (n->left)
      leftreg = genAST(n->left, NOREG, n->op);
    if (n->right)
      rightreg = genAST(n->right, leftreg, n->op);
  
    switch (n->op) {
      case A_ADD:
        return (cgadd(leftreg, rightreg));
      case A_SUBTRACT:
        return (cgsub(leftreg, rightreg));
      case A_MULTIPLY:
        return (cgmul(leftreg, rightreg));
      case A_DIVIDE:
        return (cgdiv(leftreg, rightreg));
      case A_EQ:
      case A_NE:
      case A_LT:
      case A_GT:
      case A_LE:
      case A_GE:
        if (parentASTop == A_IF || parentASTop == A_WHILE) {
          printf("generating comparison for IF or WHILE statement\n");
          return (cgcompare_and_jump(n->op, leftreg, rightreg, reg));
        }
        else {
          printf("generating comparison for assignment\n"); 
          return (cgcompare_and_set(n->op, leftreg, rightreg));
        }
      case A_INTLIT:
        printf("generating for integer literal %d\n", n->v.intvalue);
        return (cgloadint(n->v.intvalue, n->type));
      case A_IDENT:
        printf("generating for identifier %s\n", Gsym[n->v.id].name);
        return (cgloadglob(n->v.id));
      case A_LVIDENT:
        printf("generating for left-hand identifier %s\n", Gsym[n->v.id].name);
        return (cgstorglob(reg, n->v.id));
      case A_ASSIGN:
        // The work has already been done, return the result
        return (rightreg);
      case A_PRINT:
        // Print the left-child's value
        // and return no register
        printf("generating for print statement\n");
        cgprintint(leftreg);
        freeall_registers();
        return (NOREG);
      default:
        fprintf(stderr, "Unknown AST operator %d\n", n->op);
        exit(1);
    }
}

#endif // GEN_ASM_C
  