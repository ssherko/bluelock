COMP = gcc
COMP_FLAGS = -Wall
OUT = bluelock
RFLAGS? =
LIBS = bluetooth
SRC_FOLD = source/
HDR_FOLD = headers/

SRC = $(wildcard $(SRC_FOLD)*.c)
HDR = $(wildcard $(HDR_FOLD)*.h)

run: compile
	@./$(OUT) $(RFLAGS)

compile: 
	@$(COMP) $(COMP_FLAGS) $(SRC) $(HDR) -o $(OUT) -l$(LIBS)

clean:
	@rm $(OUT)
