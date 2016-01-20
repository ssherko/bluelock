COMP = gcc
COMP_FLAGS = -Wall
INC = -I/usr/include/dbus-1.0/ -I/usr/lib/i386-linux-gnu/dbus-1.0/include/
OUT = bluelock
RFLAGS? =
LIBS = -lbluetooth -ldbus-1
SRC_FOLD = source/
HDR_FOLD = headers/
CONF_FOLD = config/
KEY_STORE = keystore
LOGS_PATH = logs
SRC = $(wildcard $(SRC_FOLD)*.c)
HDR = $(wildcard $(HDR_FOLD)*.h)

run: compile
	@./$(OUT) $(RFLAGS)

compile:
	@$(COMP) $(COMP_FLAGS) $(INC) $(SRC) $(HDR) -o $(OUT) $(LIBS)

clean:
	@rm $(OUT) -f 
	@rm $(CONF_FOLD)$(KEY_STORE) -f
	@rm $(CONF_FOLD)$(LOGS_PATH) -f 
