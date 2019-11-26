cc = g++
prom = ThreadStudy
obj = ThreadStudy.o thread_func.o
extend_compile_str = -std=c++11
extend_link_str = -std=c++11 -pthread

$(prom): $(obj)
	$(cc) $(extend_link_str) -o $(prom) $(obj)

ThreadStudy.o: ThreadStudy.cpp thread_func.h
	$(cc) -c  ThreadStudy.cpp  $(extend_compile_str)

thread_func.o: thread_func.cpp thread_func.h
	$(cc) -c  thread_func.cpp  $(extend_compile_str)



# .PHONY: clean
clean:	
	rm -rf *.o

# ./$(prom)
