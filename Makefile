CC = gcc
CFLAGS = -Wall -Wextra -O3 -march=native -ffast-math -funroll-loops -Xpreprocessor -fopenmp
INCLUDES = -I/opt/homebrew/opt/fftw/include -I/opt/homebrew/opt/giflib/include -I/opt/homebrew/Cellar/libomp/20.1.6/include
LDFLAGS = -L/opt/homebrew/opt/fftw/lib -L/opt/homebrew/opt/giflib/lib -L/opt/homebrew/Cellar/libomp/20.1.6/lib
LDLIBS = -lfftw3 -lgif -lm -lomp

TARGET = gray_scott

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean 