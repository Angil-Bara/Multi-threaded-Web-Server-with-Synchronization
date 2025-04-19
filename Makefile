# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g -gdwarf-4
LDFLAGS = -pthread

# Source files
SRCS = webserver_multi.c net.c
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = webserver_multi

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean

