GXX = g++
TARGET = ascii-plague-shooter
SOURCES = ascii-plague-shooter.cpp src/*.cpp
FLAGS = -o $(TARGET) -Iinclude -lncurses -lSDL2

$(TARGET): $(SOURCES)
	$(GXX) $(FLAGS) $(SOURCES)

clean:
	rm $(TARGET)

