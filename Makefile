CXX = g++
CXXFLAGS = -g -std=c++11
LDFLAGS =
INCLUDE = -I ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
SOURCES = $(shell ls $(SRC_DIR)/*.cpp)
OBJS = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.o))
TARGET = target_name
DEPENDS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if [ ! -d $(OBJ_DIR) ]; \
    	then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
    	fi
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $< 

test:
	cd "$(PWD)/test" && make test

clean:
	$(RM) $(OBJS) $(DEPENDS)
	cd "$(PWD)/test" && make clean

-include $(DEPENDS)

.PHONY: all clean test