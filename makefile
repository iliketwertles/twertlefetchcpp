CC = g++
TARGET = twertlefetchcpp

$(TARGET): main.o
	$(CC) $^ -o $@

main.o: main.cpp
	$(CC) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

run: clean $(TARGET)
	./$(TARGET)

.PHONY: clean run