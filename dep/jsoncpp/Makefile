CC = gcc -O3 -Wall -fPIC -Wno-strict-overflow
INCLUDE = -Idist

OUTPUT = libjsoncpp.a

all: $(OUTPUT)

jsoncpp.o:dist/jsoncpp.cpp
	$(CC) $(C_ARGS) $(INCLUDE) -c $^

$(OUTPUT): jsoncpp.o
	ar -r $@ $^

clean:
	rm -f *.o $(OUTPUT)
