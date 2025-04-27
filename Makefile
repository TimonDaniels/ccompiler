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

test11n: $(EXE) tests\input11 compiler\lib\printint.c
	$(EXE) tests\input11
	gcc -c -o out.o assembly.s
	gcc -o out.exe out.o compiler\lib\printint.c
	.\out.exe

.PHONY: all clean