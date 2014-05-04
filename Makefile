SRC_DIR := ./src
INCLUDE_DIR := ./include
BUILD_DIR := ./build

CPP := g++
CC := gcc

CFLAG := -O2
LFLAG := -O2

TARGET := $(BUILD_DIR)/parser
CHECK := $(BUILD_DIR)/check

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(BUILD_DIR)/parser.o $(BUILD_DIR)/node.o $(BUILD_DIR)/main.o


.PHONY:all $(BUILD_DIR) $(CHECK)
all:$(BUILD_DIR) $(TARGET) $(CHECK)

$(BUILD_DIR):
	mkdir -p $@

$(CHECK):$(BUILD_DIR)/check.o
	$(CPP) -o $@ $< $(LFLAG)

$(TARGET):$(OBJ)
	$(CPP) -o $(TARGET) $(OBJ) $(CFLAG) $(LFLAG)


$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CPP) -c $< $(CFLAG) -o $@ -I$(INCLUDE_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
