cc = g++
prom = time_study
obj = time_study.o time_func.o
extend_compile_str = -std=c++11
extend_link_str = -std=c++11 -pthread

$(prom): $(obj)
	$(cc) $(extend_link_str) -o $(prom) $(obj)

time_study.o: time_study.cpp time_func.h
	$(cc) -c  time_study.cpp  $(extend_compile_str)

time_func.o: time_func.cpp time_func.h
	$(cc) -c  time_func.cpp  $(extend_compile_str)



# .PHONY: clean
clean:	
	rm -rf *.o

# ./$(prom)
