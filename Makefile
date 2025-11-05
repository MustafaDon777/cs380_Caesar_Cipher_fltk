# Makefile for Cipher Studio Pro
CXX      = g++
CXXFLAGS = -Os -std=c++17 -Wall
LIBS     = -lfltk
TARGET   = cipher_pro
SRC      = main.cpp

# Windows (MinGW)
ifeq ($(OS),Windows_NT)
    LIBS += -lole32 -lcomctl32
    TARGET := $(TARGET).exe
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run