LDLIBS = -lpthread
threadpool98: main.cpp threadpool.cpp threadpool.h
	g++ -g -o $@ $^ ${LDLIBS}
clean:
	rm threadpool98