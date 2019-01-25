cc = g++
prom = ThreadStudy
obj = ThreadStudy.o thread_func.o test.o func_math.o condition_study.o
extend_compile_str = -std=c++11
extend_link_str = -std=c++11 -pthread

$(prom): $(obj)
	$(cc) $(extend_link_str) -o $(prom) $(obj)

ThreadStudy.o: ThreadStudy.cpp thread_func.h test.h
	$(cc) -c  ThreadStudy.cpp  $(extend_compile_str)

thread_func.o: thread_func.cpp thread_func.h
	$(cc) -c  thread_func.cpp  $(extend_compile_str)

test.o: test.cpp test.h func_math.h condition_study.h
	$(cc) -c  test.cpp  $(extend_compile_str)

func_math.o: func_math.cpp func_math.h
	$(cc) -c  func_math.cpp  $(extend_compile_str)

condition_study.o: condition_study.cpp condition_study.h func_math.h
	$(cc) -c  condition_study.cpp  $(extend_compile_str)


# .PHONY: clean
clean:	
	rm -rf *.o

# ./$(prom)
