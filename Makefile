# define the name of application
APPNAME := noon

# define the cpp compiler to use
CXX := g++

# define the FLAGS
FLAGS := -Wall -llua -std=c++20 -g

# define bin directory
BIN := build

# define include directory
INC := include

# define lib directory
LIB := lib

# define source directory
SRC := src

# define object directory
OBJ := obj

# define resource directory
RES := res

VPATH := $(SRC) $(INC) $(LIB)

# Generate extension depending on the operating system
ifeq ($(OS),Windows_NT)
	TARGET := $(BIN)/$(APPNAME).exe
else
	TARGET := $(BIN)/$(APPNAME)
endif

# define the color of the console text
ifeq ($(OS),Windows_NT)
	RED    := $(tput setaf "1")
	GREEN  := $(tput setaf "2")
	YELLOW := $(tput setaf "3")
	BLUE   := $(tput setaf "4")
	PURPLE := $(tput setaf "5")
	CYAN   := $(tput setaf "6")
	WHITE  := $(tput setaf "7")
	RESET  := $(tput sgr0)
else
	RED    := $(shell echo -e "\033[031m")
	GREEN  := $(shell echo -e "\033[032m")
	YELLOW := $(shell echo -e "\033[033m")
	BLUE   := $(shell echo -e "\033[034m")
	PURPLE := $(shell echo -e "\033[035m")
	CYAN   := $(shell echo -e "\033[036m")
	WHITE  := $(shell echo -e "\033[037m")
	RESET  := $(shell echo -e "\033[0m")
endif

OBJS := $(patsubst %.cpp, $(OBJ)/%.o, $(notdir $(wildcard $(SRC)/*.cpp) $(wildcard $(LIB)/*.cpp)))

# Generate object files
$(OBJ)/%.o: %.cpp
ifeq ($(OS),Windows_NT)
	@IF NOT EXIST $(subst /,\,$(OBJ)) mkdir $(subst /,\,$(OBJ))
else
	@mkdir -p $(OBJ)
endif
	@echo $(CYAN)Compiling: $(RED)$(@F)$(RESET)
	@$(CXX) -g -std=c++20 -I $(INC) -c $< -o $@

# Generate executable
$(TARGET): $(OBJS)
ifeq ($(OS),Windows_NT)
	@IF NOT EXIST $(subst /,\,$(BIN)) mkdir $(subst /,\,$(BIN))
else
	@mkdir -p $(BIN)
endif
	@echo $(CYAN)Creating executable: $(GREEN)$(@F)$(RESET)
	@$(CXX) $(OBJS) -o $@ $(FLAGS)

.PHONY: run
run:
# @echo $(CYAN)Copying resources
# @cp -R $(RES)/* $(BIN) 
	@$(TARGET)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@if exist $(OBJ)\*.o @del /Q $(OBJ)\*.o
	@if exist $(OBJ)\*.exe @del /Q $(BIN)\*.exe
else
	@rm -f $(OBJ)/*.o
	@rm -f $(BIN)/*
endif

.PHONY:	delete
delete:
ifeq ($(OS),Windows_NT)
	@if exist $(OBJ) @rd /Q /S $(OBJ)
	@if exist $(BIN) @rd /Q /S $(BIN)
else
	@rm -rf $(OBJ)
	@rm -rf $(BIN)
endif