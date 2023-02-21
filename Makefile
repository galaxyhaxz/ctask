CC = gcc

TARGET = coexample
OBJS = coroutine.o example.o

example: $(OBJS)
	$(CC) -o $@ $^

all: $(TARGET)

clean:
	$(RM) $(OBJS)
