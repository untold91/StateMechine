# Application Name
APP = STATE_MACHINE

# Target files
TARGETS = \
StateMachine.c \

LIB_ARUG = \
-pthread

# Compiler Options
CC_CPP = g++
CC_C = gcc

# Build Path
BUILD_PATH = ./build/

# Build Arguments
# CFLAGS = -Wall -g -std=$(STD_VAR) -Werror
CFLAGS = -Wall -g -std=$(STD_VAR)
STD_VAR = c99

# Make commands
all: clean $(APP)

# Build Application
$(APP):
	$(CC_C) $(CFLAGS) $(TARGETS) -o $(BUILD_PATH)$(APP)_APP $(LIB_ARUG)

# Run the Application
run:
	$(BUILD_PATH)$(APP)_APP

# Clean Build
clean:
	rm -rf $(BUILD_PATH)*