COMP = gcc
COMP_FLAGS = -Wall
OUT = bluelock
RFLAGS? =
LIBS = -lbluetooth
SRC_FOLD = source/
HDR_FOLD = headers/
CONF_FOLD = config/
KEY_STORE = keystore
SRC = $(wildcard $(SRC_FOLD)*.c)
HDR = $(wildcard $(HDR_FOLD)*.h)

run: compile
	@./$(OUT) $(RFLAGS)

compile: 
	@$(COMP) $(COMP_FLAGS) $(SRC) $(HDR) -o $(OUT) $(LIBS)

clean:
	@rm $(OUT)
	@rm $(CONF_FOLD)$(KEY_STORE)