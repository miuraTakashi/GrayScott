CC = gcc
CFLAGS = -Wall -Wextra -O2
INCLUDES = -I/opt/homebrew/opt/fftw/include -I/opt/homebrew/opt/giflib/include
LDFLAGS = -L/opt/homebrew/opt/fftw/lib -L/opt/homebrew/opt/giflib/lib
LDLIBS = -lfftw3 -lgif -lm

TARGET = gray_scott

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean 