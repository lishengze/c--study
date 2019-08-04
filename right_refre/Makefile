# originating https://github.com/TheNetAdmin/Makefile-Templates
# tool marcros
CC := g++
CCFLAG := -std=c++11
DBGFLAG := -g
CCOBJFLAG := -c
LINKFLAG = -pthread

# path marcros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src ../public-files/public-func ../public-files/public-class
PUBFUNC_PATH := ../public-files/public-func
PUBCLASS_PATH := ../public-files/public-class
CURR_SRC_PATH := src
DBG_PATH := debug

HEADER_PATH = -I ../public-files/public-func/ ../public-files/public-class/
LIB_PATH = -L ../public-files/lib/

# compile marcros
TARGET_NAME := main
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif

TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)
MAIN_SRC := src/main.cpp

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

$(warning SRC is $(SRC)) 
$(warning OBJ is $(OBJ)) 
$(warning OBJ_DEBUG is $(OBJ_DEBUG)) 
$(warning TARGET is $(TARGET))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)

CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: all

# non-phony targets

$(TARGET): $(OBJ)
	$(CC) $(CCFLAG) $(LINKFLAG) -o $@ $?

$(OBJ_PATH)/%.o: $(CURR_SRC_PATH)/%.cpp
	$(CC) $(CCFLAG) $(CCOBJFLAG) -o $@ $<  $(HEADER_PATH)

$(OBJ_PATH)/%.o: $(PUBFUNC_PATH)/%.cpp
	$(CC)  $(CCFLAG) $(CCOBJFLAG) -o $@ $< $(HEADER_PATH)

$(OBJ_PATH)/%.o: $(PUBCLASS_PATH)/%.cpp
	$(CC) $(CCFLAG) $(CCOBJFLAG) -o $@ $<  $(HEADER_PATH)

# $(DBG_PATH)/%.o: $(SRC_PATH)/%.cpp
# 	$(CC) $(CCOBJFLAG) $(DBGFLAG) -o $@ $< $(CCFLAG)

$(DBG_PATH)/%.o: $(CURR_SRC_PATH)/%.cpp
	$(CC) $(CCFLAG) $(CCOBJFLAG) -o $@ $<  $(HEADER_PATH)

$(DBG_PATH)/%.o: $(PUBFUNC_PATH)/%.cpp
	$(CC) $(CCFLAG) $(CCOBJFLAG) -o $@ $<  $(HEADER_PATH)

$(DBG_PATH)/%.o: $(PUBCLASS_PATH)/%.cpp
	$(CC)  $(CCFLAG) $(CCOBJFLAG) -o $@ $< $(HEADER_PATH)

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CCFLAG) $(DBGFLAG) $(LINKFLAG)  $? -o $@

# phony rules
.PHONY: all
all: $(TARGET)

# .PHONY: debug
debug: $(TARGET_DEBUG)

# .PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

# .PHONY: distclean
distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
