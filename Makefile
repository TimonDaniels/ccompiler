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

.PHONY: all clean