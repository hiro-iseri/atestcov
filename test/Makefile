CXX = g++
CXXFLAGS = -g -std=c++11
LDFLAGS =
INCLUDE =
SRC_DIR = .
OBJ_DIR = ./obj
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCES:.cpp=.o)))
TARGET = test
DEPENDS = $(OBJS:.o=.d)

LIBS = -lgtest
GTEST_INCLUDE =

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	./test

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $< 

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPENDS)

-include $(DEPENDS)

.PHONY: clean