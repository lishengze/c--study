cc = g++
prom = main
obj = main.o test_func.o test_static.o
extend_compile_str = -std=c++11
extend_link_str = -std=c++11 -pthread

$(prom): $(obj)
	$(cc) $(extend_link_str) -o $(prom) $(obj)

main.o: main.cpp test_func.h
	$(cc) -c  main.cpp  $(extend_compile_str)

test_func.o: test_func.cpp test_static.h
	$(cc) -c  test_func.cpp  $(extend_compile_str)

test_static.o: test_static.cpp test_static.h
	$(cc) -c  test_static.cpp  $(extend_compile_str)

# .PHONY: clean
clean:	
	rm -rf *.o

# ./$(prom)
