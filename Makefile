CC = gcc
CFLAGS = -g

TARGET = simplec
BUILD_DIR = build
OUTPUT = $(BUILD_DIR)/$(TARGET)

TMP_DIR = tmp

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(OUTPUT) $(TARGET).c

clean:
	$(RM) $(OUTPUT) TMP_DIR/* *.ll *.out *.err
