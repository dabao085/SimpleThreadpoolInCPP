LDLIBS = -lpthread
CFLAG = -std=c++0x -Wall
threadpool03: main.cpp threadpool.cpp threadpool.h
	g++ -g -o $@ $^ ${LDLIBS} ${CFLAG}
clean:
	rm threadpool03