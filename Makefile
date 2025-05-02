SRC_DIR = compiler
EXE = compiler.exe

SRCS = $(SRC_DIR)/main.c

$(EXE): $(SRCS)
	gcc -o $(EXE) -g $(SRCS)
	@echo "Compiler built successfully: $(EXE)"

clean:
	del $(EXE)

test: $(EXE)
	cd tests && runtests.bat

test15: $(EXE) tests\input15.c compiler\lib\printint.c
	$(EXE) tests\input15.c
	gcc -c -o out.o assembly.s
	gcc -o out.exe out.o compiler\lib\printint.c
	.\out.exe

.PHONY: all clean