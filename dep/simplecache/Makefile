CC = g++ -O3 -Wall -fPIC -Wno-strict-overflow -Wno-invalid-offsetof
INCLUDE = -I.

OUTPUT = libsimplecache.a

all: $(OUTPUT)

.cpp.o:
	$(CC) $(C_ARGS) $(INCLUDE) -c $^

.c.o:
	$(CC) $(C_ARGS) $(INCLUDE) -c $^

$(OUTPUT): *.o
	ar -r $@ $^

clean:
	rm -f *.o $(OUTPUT)
