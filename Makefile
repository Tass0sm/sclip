TARGET = sclip
SRC = main.c buffer.c
OBJ = $(SRC:.c=.o)
CFLAGS = -Wall -pedantic
LDFLAGS = -ljack

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall $< -c -o $@ 

clean:
	rm -f $(OBJ) $(TARGET)

options:
	@echo $(TARGET) build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC      = $(CC)"
