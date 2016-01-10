EXEC=pp
OBJECTS=$(EXEC:=.o) pile.o
CFLAGS=-Wall

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $< -o $(EXEC) 

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJECTS)

mrproper: clean
	$(RM) $(EXEC)