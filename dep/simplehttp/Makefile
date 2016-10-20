CC = gcc -O3 -Wall -fPIC -Wno-strict-overflow
INCLUDE = -I.

OUTPUT = libsimplehttp.a

all: $(OUTPUT)

.cpp.o:
	$(CC) $(C_ARGS) $(INCLUDE) -c $^

.c.o:
	$(CC) $(C_ARGS) $(INCLUDE) -c $^

$(OUTPUT): *.o
	ar -r $@ $^

clean:
	rm -f *.o $(OUTPUT)
