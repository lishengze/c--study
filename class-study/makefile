CC = g++
TARGET = a.out
OBJECTS = main.o print.o testVirtualBase.o data-sema.o

$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

main.o: main.cpp testVirtualBase.h
	$(CC) -c main.cpp

print.o: print.cpp print.h
	$(CC) -c print.cpp

testVirtualBase.o: testVirtualBase.cpp testVirtualBase.h print.h
	$(CC) -c testVirtualBase.cpp

data-sema.o: data-sema.cpp data-sema.h print.h
	$(CC) -c data-sema.cpp

.PHONY: clean
clean:
	rm -f *.o