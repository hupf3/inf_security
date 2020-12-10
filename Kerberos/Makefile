C := gcc
INC_DIR := include
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE := -I./$(INC_DIR)
MATH := -lm

all: $(BIN_DIR)/AS $(BIN_DIR)/TGS $(BIN_DIR)/SS $(BIN_DIR)/Client

$(BIN_DIR)/AS : $(BUILD_DIR)/DES.o $(BUILD_DIR)/ServerAS.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(INCLUDE) $^ -o $@

$(BIN_DIR)/TGS : $(BUILD_DIR)/DES.o $(BUILD_DIR)/ServerTGS.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(INCLUDE) $^ -o $@

$(BIN_DIR)/SS : $(BUILD_DIR)/DES.o $(BUILD_DIR)/ServerSS.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(INCLUDE) $^ -o $@

$(BIN_DIR)/Client : $(BUILD_DIR)/DES.o $(BUILD_DIR)/Client.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(INCLUDE) $^ -o $@

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(INCLUDE) -c $^ -o $@

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BIN_DIR)