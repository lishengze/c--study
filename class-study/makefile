CC = g++
TARGET = a.out
OBJECTS = main.o print.o testVirtualBase.o data-sema.o testDataPointer.o testPureClass.o

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

testDataPointer.o: testDataPointer.cpp testDataPointer.h print.h
	$(CC) -c testDataPointer.cpp

testPureClass.o: testPureClass.cpp testPureClass.h print.h
	$(CC) -c testPureClass.cpp