TARGET = sobel_filter
CC = g++

PTHREADFLAGS = -lpthread
OPENCVFLAGS = `pkg-config --cflags --libs opencv4`

SRC = main.cc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $^ -o $@ $(PTHREADFLAGS) $(OPENCVFLAGS)

run: $(TARGET)
	./$< photo.jpg

clean:
	rm -rf $(TARGET)

format:
	clang-format -i $(SRC)