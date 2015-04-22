PROGNAME = test
CC_PROG = g++-4.9
CC_OPTS = -std=c++11 -lpthread
CC = $(CC_PROG) $(CC_OPTS)
INCLUDES = -I ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_PATH = ./bin
CPPFLAGS = $(INCLUDES)

SRCS = main.cpp Server.cpp cipher.cpp

.PHONY: clean

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
			$(CC) $(CPPFLAGS) -c $< -o $@

$(BIN_PATH)/$(PROGNAME) : $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
			$(CC) $(CPPFLAGS) -o $@ $^

clean:
			rm $(OBJ_DIR)/*.o $(BIN_PATH)/$(PROGNAME)
			@echo Clean done!
