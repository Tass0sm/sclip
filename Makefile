TARGET = sclip
SRC = sclip.c
OBJ = $(SRC:.c=.o)
CFLAGS = -Wall -pedantic
LDFLAGS = -ljack

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(LDFLAGS) -Wall $< -c -o $@ 

clean:
	rm -f $(OBJ) $(TARGET)

options:
	@echo $(TARGET) build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC      = $(CC)"
