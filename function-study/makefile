CC = g++
TARGET = a.out
OBJECTS = main.o print.o testOverloaded.o

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

main.o: main.cpp testOverloaded.h
	$(CC) -c main.cpp

print.o: print.cpp print.h
	$(CC) -c print.cpp

testOverloaded.o: testOverloaded.cpp print.h testOverloaded.h 
	$(CC) -c testOverloaded.cpp