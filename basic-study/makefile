CC = g++
TARGET = a.out
OBJECTS = main.o print.o testStaticCast.o testVolatile.o

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

main.o: main.cpp testStaticCast.h
	$(CC) -c main.cpp

print.o: print.cpp print.h
	$(CC) -c print.cpp

testStaticCast.o: testStaticCast.cpp print.h testStaticCast.h 
	$(CC) -c testStaticCast.cpp

testVolatile.o: testVolatile.cpp print.h testVolatile.h 
	$(CC) -c testVolatile.cpp	