CFLAGS=-I. -Wall -Werror -O2
INCS=
OBJS=test.o
LIBS=-lfftw3 -lm
TARGET=test

%.o: %.c $(INCS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf $(TARGET) *.o
