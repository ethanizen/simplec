CC = gcc
CFLAGS = -g

TARGET = simplec
BUILD_DIR = build

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET) *.ll *.out *.err
