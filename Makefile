CXX = g++

CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g

TARGET = P4

SRCS = P4.cpp codeGeneration.cpp staticSemantics.cpp parser.cpp scanner.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
