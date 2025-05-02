# A C Compiler
I wanted to both learn C and understand what it is that compilers actually do. So I started this project and worked alongside a github repository that I found on [build your own x](https://github.com/codecrafters-io/build-your-own-x). I used kept this repository next to my project, and made some small changes at the start that prevented me from copying everything mindlessly, and make sure that I had to understand what was happening. This also happend naturally because the repository was made for running on a Ubuntu machine, where I build this project on a windows machine.

The project makes a start at making a C compiler that parses a file, and generates a binary tree combining all the statements on every line. This tree is then converted in to assembly code that can be executed using the gcc compiler.


## BNF grammer of the language 
This is the BNF grammer that this compiler so far supports.

### Tokens
```
token: 'int' | 'char' | 'void'
     | 'if' | 'else' | 'while' | 'for' | 'return'
     | identifier
     | integer_literal
     | string_literal
     | '=' | '+' | '-' | '*' | '/' | '=='
     | '!=' | '<' | '>' | '<=' | '>='
     | ';' | '(' | ')' | '{' | '}' | ',' | '&'
     | '[' | ']'
```
### Program structure
```
program: function_declaration*
```
### Types
```
type: 'void' | 'char' | 'int' | type '*'      # Last part added in Pointers pt1
```

### Declarations
```
function_declaration: type identifier '(' parameter_list ')' compound_statement

parameter_list: parameter?
              | parameter ',' parameter_list

parameter: type identifier
         | type identifier '[' ']'             # Array parameters

variable_declaration: type identifier ';'
                    | type identifier '[' integer_literal ']' ';'   # Array declarations

### Statements
statement: variable_declaration
         | expression_statement
         | compound_statement
         | if_statement
         | while_statement
         | for_statement
         | return_statement

compound_statement: '{' statement* '}'

if_statement: 'if' '(' expression ')' statement
            | 'if' '(' expression ')' statement 'else' statement

while_statement: 'while' '(' expression ')' statement

for_statement: 'for' '(' expression_statement expression_statement expression? ')' statement

return_statement: 'return' expression? ';'

expression_statement: expression? ';'
```

### Expressions
```
expression: assignment_expression

assignment_expression: identifier '=' expression
                     | identifier '[' expression ']' '=' expression
                     | conditional_expression

conditional_expression: logical_or_expression

logical_or_expression: logical_and_expression ('||' logical_and_expression)*

logical_and_expression: equality_expression ('&&' equality_expression)*

equality_expression: relational_expression (('==' | '!=') relational_expression)*

relational_expression: additive_expression (('<' | '>' | '<=' | '>=') additive_expression)*

additive_expression: multiplicative_expression (('+' | '-') multiplicative_expression)*

multiplicative_expression: unary_expression (('*' | '/') unary_expression)*

unary_expression: postfix_expression
                | '-' unary_expression
                | '*' unary_expression         # Pointer dereference (added in Pointers pt1)
                | '&' unary_expression         # Address-of operator (added in Pointers pt1)

postfix_expression: primary_expression
                  | identifier '(' argument_list? ')'
                  | identifier '[' expression ']'

primary_expression: identifier
                  | integer_literal
                  | string_literal
                  | '(' expression ')'

argument_list: expression
             | expression ',' argument_list
```

## Converting the assembly to support windows
Windows does not suppert .type directive in the assembly language so this was removed. Also Windows requires 32bits shadow space on the stack for functions so function calls are surrounded by 32 bit allocation and deallocations. The last thing I needed to do is change some register names because windows uses a different register for function arguments.