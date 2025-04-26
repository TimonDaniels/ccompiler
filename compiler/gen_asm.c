#ifndef GEN_ASM_C
#define GEN_ASM_C

#include "global.h"
#include "defs.h"
#include "misc.c"

// List of available registers and their names.
static int freereg[4];
static char *reglist[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *breglist[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char *dreglist[4] = { "%r8d", "%r9d", "%r10d", "%r11d" };

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

// // Linux version:
// // Print out the assembly preamble
// void cgpreamble() {
//   freeall_registers();
//   fputs("\t.text\n"
// 	".LC0:\n"
// 	"\t.string\t\"%d\\n\"\n"
// 	"printint:\n"
// 	"\tpushq\t%rbp\n"
// 	"\tmovq\t%rsp, %rbp\n"
// 	"\tsubq\t$16, %rsp\n"
// 	"\tmovl\t%edi, -4(%rbp)\n"
// 	"\tmovl\t-4(%rbp), %eax\n"
//   "\tmovl\t%eax, %esi\n"
//   	"\tleaq\t.LC0(%rip), %rdi\n"
// 	"\tmovl	$0, %eax\n"
// 	"\tcall	printf@PLT\n" "\tnop\n" "\tleave\n" "\tret\n" "\n", Outfile);
// }

// Windows version:
void cgpreamble() {
  freeall_registers();
  fputs("\t.text\n", Outfile);
}

// Print out a function preamble
void cgfuncpreamble(char *name) {
  // Linux version:
  // fprintf(Outfile,
	//   "\t.text\n"
	//   "\t.globl\t%s\n"
	//   "\t.type\t%s, @function\n"
	//   "%s:\n" "\tpushq\t%%rbp\n"
	//   "\tmovq\t%%rsp, %%rbp\n", name, name, name);

  // Windows version:
  fprintf(Outfile,
    "\t.text\n"
    "\t.globl\t%s\n"
    "%s:\n"
    "\tpushq\t%%rbp\n"
    "\tmovq\t%%rsp, %%rbp\n", name, name);
}

// Print out a function postamble
void cgfuncpostamble() {
  fputs("\tpopq	%rbp\n" "\tret\n", Outfile);
}

// Array of type sizes in P_XXX order.
// 0 means no size. P_NONE, P_VOID, P_CHAR, P_INT, P_LONG
static int psize[] = { 0,       0,      1,     4,     8 };

// Given a P_XXX type value, return the
// size of a primitive type in bytes.
int cgprimsize(int type) {
  // Check the type is valid
  if (type < P_NONE || type > P_LONG)
    fatal("Bad type in cgprimsize()");
  return (psize[type]);
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

  switch (Gsym[id].type) {
    case P_INT:
      fprintf(Outfile, "\tmovzbl\t%s(\%%rip), %s\n", Gsym[id].name, dreglist[r]);
      break;
    case P_CHAR:
      fprintf(Outfile, "\tmovzbq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
      break;
    case P_LONG:
      fprintf(Outfile, "\tmovq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
      break;
    default:
      fatald("Bad type in cgstorglob()", Gsym[id].type);
  }
  return (r);
}

// Generate a global symbol
void cgglobsym(int id) {
  int typesize;
  typesize = cgprimsize(Gsym[id].type);

  fprintf(Outfile, "\t.comm\t%s,%d,%d\n", Gsym[id].name, typesize, typesize);
}

// Store a register's value into a variable
int cgstorglob(int r, int id) {
  switch (Gsym[id].type) {
    case P_CHAR:
      fprintf(Outfile, "\tmovb\t%s, %s(\%%rip)\n", breglist[r],
              Gsym[id].name);
      break;
    case P_INT:
      fprintf(Outfile, "\tmovl\t%s, %s(\%%rip)\n", dreglist[r],
              Gsym[id].name);
      break;
    case P_LONG:
      fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], Gsym[id].name);
      break;
    default:
      fatald("Bad type in cgloadglob:", Gsym[id].type);
  }
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

// // Linux version:
// Call printint() with the given register
// void cgprintint(int r) {
//   fprintf(Outfile, "\tmovq\t%s, %%rcx\n", reglist[r]);
//   fprintf(Outfile, "\tcall\tprintint\n");
//   free_register(r);
// }

// Windows version:
void cgprintint(int r) {
  // Allocate shadow space before the call
  fprintf(Outfile, "\tsubq\t$32, %%rsp\n");          // Allocate shadow space
  fprintf(Outfile, "\tmovq\t%s, %%rcx\n", reglist[r]);
  fprintf(Outfile, "\tcall\tprintint\n");
  fprintf(Outfile, "\taddq\t$32, %%rsp\n");          // Clean up shadow space
  free_register(r);
}

// Call a function with one argument from the given register
// Return the register with the result
int cgcall(int r, int id) {
  // Get a new register
  int outr = alloc_register();
  fprintf(Outfile, "\tsubq\t$32, %%rsp\n");          // Allocate shadow space
  fprintf(Outfile, "\tmovq\t%s, %%rcx\n", reglist[r]);
  fprintf(Outfile, "\tcall\t%s\n", Gsym[id].name);
  fprintf(Outfile, "\tmovq\t%%rax, %s\n", reglist[outr]);
  fprintf(Outfile, "\taddq\t$32, %%rsp\n");          // Clean up shadow space
  free_register(r);
  return (outr);
}

// Generate code to return a value from a function
void cgreturn(int reg, int id) {
  // Generate code depending on the function's type
  switch (Gsym[id].type) {
    case P_CHAR:
      fprintf(Outfile, "\tmovzbl\t%s, %%eax\n", breglist[reg]);
      break;
    case P_INT:
      fprintf(Outfile, "\tmovl\t%s, %%eax\n", dreglist[reg]);
      break;
    case P_LONG:
      fprintf(Outfile, "\tmovq\t%s, %%rax\n", reglist[reg]);
      break;
    default:
      fatald("Bad function type in cgreturn:", Gsym[id].type);
  }
  cgjump(Gsym[id].endlabel);
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
      case A_WIDEN:
        // Widen the child's type to the parent's type
        return (cgwiden(leftreg, n->left->type, n->type));
      case A_RETURN:
        printf("generating return statement\n");
        cgreturn(leftreg, Functionid);
        return (NOREG);
      case A_FUNCCALL:
        printf("generating function call");
        return (cgcall(leftreg, n->v.id));
      default:
        fatald("Unknown AST operation", n->op);
    }
}

#endif // GEN_ASM_C
  