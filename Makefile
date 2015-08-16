OBJS = epoller.o eventloop.o wrapper.o buffer.o threads.o link.o
CFLAGS += -O2 --std=c++0x 
#CFLAGS += -O2 --std=c++0x -I/home/peitao/work/build_tools/third_party/include
LIB += -lrt -pthread

all: app
app:${OBJS} test_main.cc
	g++ ${OBJS} ${LIB} test_main.cc -o app
epoller.o:epoller.cc *.h
	g++ -c epoller.cc ${CFLAGS} 
eventloop.o:eventloop.cc *.h
	g++ -c eventloop.cc ${CFLAGS}
wrapper.o:wrapper.cc *.h
	g++ -c wrapper.cc ${CFLAGS}
buffer.o:buffer.cc *.h
	g++ -c buffer.cc ${CFLAGS}
threads.o:threads.cc *.h
	g++ -c threads.cc ${CFLAGS}
link.o:link.cc *.h
	g++ -c link.cc ${CFLAGS}
clean:
	rm -f *.o app *_test


test: buffer_test
buffer_test: buffer.cc buffer.h buffer_test.cc
	g++ buffer.cc buffer.h buffer_test.cc -o buffer_test
