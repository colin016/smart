CC = gcc
LFLAG = -fpic -shared -lpthread
CFLAG = -c -g -Wall -fpic -shared

DIR_OBJ = ../../../obj
DIR_LIB = ../../../../lib
DIR_BIN = ../../../../bin

DLLTARGET = ${DIR_BIN}/smartdll.so
LIBTARGET = ${DIR_BIN}/libsmartdll.a

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRCS}))

.PHONY:all
all:${DLLTARGET}

.PHONY:dll
dll:${DLLTARGET}

.PHONY:lib
lib:${LIBTARGET}

${DLLTARGET}:${OBJS}
	@echo 'building target $@'
	g++ ${LFLAG} $^ -o $@
	@echo 'finished building target $@'
	
${LIBTARGET}:${OBJS}
	@echo 'building target $@'
	ar -r $@ $^
	@echo 'finished building target $@'	
	
${DIR_OBJ}/%.o: %.cpp
	${CC} ${CFLAG} -c $< -o $@
