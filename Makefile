# --- VARIABLES ---
CC = gcc

# Flags de compilación
# -O3            : Maximum optimization.
# -march=native  : Use native processor.
# -flto          : Link Time Optimization.
# -Wall -Wextra  : Maximize warnings.
CFLAGS = -O3 -march=native -flto -Wall -Wextra

# Linker flags
LDFLAGS = -flto

# Executable name
TARGET = cifras

# List of source files (only .c)
SRCS = main.c cifras_bt.c

# Automatically generate the list of object files (.o)
OBJS = $(SRCS:.c=.o)

# --- RULES ---

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Compilation complete!"

# Generic rule to compile .c to .o
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean-up rule (safe)
clean:
	@echo "Cleaning up compiled files..."
	rm -f $(OBJS) $(TARGET)

# Avoid potential conflicts with files named 'all' or 'clean'
.PHONY: all clean
