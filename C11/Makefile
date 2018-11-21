LDLIBS = -lpthread
CFLAG = -std=c++11 -Wall
threadpool11: main.cpp threadpool.cpp threadpool.h
	g++ -g -o $@ $^ ${LDLIBS} ${CFLAG}
clean:
	rm threadpool11