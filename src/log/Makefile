CC = gcc
CFLAG = -c -g -Wall -I${DIR_INC}
LDFLAG = -lpthread -lm

DIR_INC = ../inc
DIR_OBJ = ../obj
DIR_BIN = ../../bin

EXECFILE=test
LIBFILE=.a
TARGET=$(shell pwd)
TARGET:=$(shell basename ${TARGET})
TESTTARGET:=${DIR_BIN}/${TARGET}${EXECFILE}
LIBTARGET:=${DIR_BIN}/lib${TARGET}${LIBFILE}

TESTSRC = $(wildcard *.c) $(wildcard ./test/*.c)
TESTOBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${TESTSRC}))

LIBSRC = $(wildcard *.c)
LIBOBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${LIBSRC}))

.PHONY:all
all: ${TESTTARGET}

${LIBTARGET}:${LIBOBJ}
	@echo '***Begin to building target: $@'
	ar -r $@ $^
	@echo '***Finished building target: $@'
	
.PHONY:lib
lib:${LIBTARGET}

${TESTTARGET}:${TESTOBJ}
	@echo '***Begin to building target: $@'
	${CC}  $^ -o $@ ${LDFLAG}
	@echo '***Finished building target: $@'

${DIR_OBJ}/%.o: %.c
	${CC} ${CFLAG} -c $< -o $@

${DIR_OBJ}/main.o:./test/main.c
	${CC} ${CFLAG} -c $< -o $@

clean:
	@echo 'Make $@'
	rm -rf ${DIR_OBJ}/*.o 
	@echo 'Finished make $@'

