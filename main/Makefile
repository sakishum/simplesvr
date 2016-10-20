# 开发环境的定义
version = prod

ifneq ($(version), prod)
C_FLAGS = -DDEBUG
endif

$(info current env: $(version))


INC = -I. -I../dep -I../dep/pdu -I../dep/pdu/mcall
LIB = -lrt -lpthread -lm -ldl -rdynamic -L../dep/simplehttp -lsimplehttp -L../dep/simplecache -lsimplecache -L../dep/jsoncpp -ljsoncpp

C_FLAGS += -Wall

BINARY = simplesvr

all:$(BINARY)

$(BINARY): main.cpp *.h
	g++ $< $(C_FLAGS) $(INC) $(LIB)  -o $@

clean:
	-rm -f *.o 
	-rm -f $(BINARY)
	
install: 
	mv $(BINARY) ../bin
	cp conf.json ../bin

