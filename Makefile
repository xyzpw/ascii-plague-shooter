GXX = g++
TARGET = plague-shooter
SOURCES = src/*.cpp src/utils/*.cpp src/entities/*.cpp src/core/*.cpp $(wildcard src/core/**/*.cpp)
INCLUDES = -Iinclude -Iinclude/utils
LINKERS = -lncurses -lSDL2

FLAGS = -o $(TARGET) $(INCLUDES) $(LINKERS)

$(TARGET): $(SOURCES)
	$(GXX) $(FLAGS) $(SOURCES)

clean:
	rm $(TARGET)

