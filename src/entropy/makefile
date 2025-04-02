# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -mrdrnd
LDFLAGS = -lbcrypt -lcrypto

# Source Files
SRCS = system_entropy.c shake.c   # Ensure `shake.c` does NOT have `main()`
OBJS = $(SRCS:.c=.o)

# Output Executable
TARGET = entropy_mixer.exe

# Default Rule: Build the Executable
all: $(TARGET) 

# Linking Step: Create the final executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile Each .c File into .o File
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Build Files
clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)