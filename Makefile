SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

cc: $(OBJECTS)
	gcc $^ -o $@ -g

%.o: %.c
	gcc -c $< -o $@ -g