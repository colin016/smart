CC = gcc
CFLAG = -c -g -Wall -I${DIR_INC} -I${DIR_GOINC}
LDFLAG = -lpthread -lm -L${DIR_OBJ} -lgo

DIR_INC = ../inc
DIR_GOINC = ./goahead/inc
DIR_OBJ = ../obj
DIR_BIN = ../../bin

EXECFILE=test
LIBFILE=.a
TARGET=$(shell pwd)
TARGET:=$(shell basename ${TARGET})

TESTTARGET:=${DIR_BIN}/${TARGET}${EXECFILE}
LIBTARGET:=${DIR_BIN}/lib${TARGET}${LIBFILE}

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRCS}))



.PHONY:lib
lib:${LIBTARGET}

${LIBTARGET}:${OBJS}
	@echo '***Begin to building target: $@'
	ar -r $@ $^
	cd ./goahead && make all
	cp ${DIR_OBJ}/libgo.a ${DIR_BIN}/libgo.a
	@echo '***Finished building target: $@'

${DIR_OBJ}/%.o: %.c
	${CC} ${CFLAG} -c $< -o $@

${DIR_OBJ}/testframe.o:./test/testframe.c
	${CC} ${CFLAG} -c $< -o $@

${DIR_OBJ}/libgo.a:
	@echo '***Begin to building goahead'
	cd ./goahead && make all
	@echo '***Finished to building goahead'

.PHONY:all
all: ${TESTTARGET}

${TESTTARGET}:${DIR_OBJ}/libgo.a ${OBJS} ${DIR_OBJ}/testframe.o
	@echo '***Begin to building target: $@'
	${CC}  $^ -o $@ ${LDFLAG}
	@echo '***Finished building target: $@'
clean:
	@echo 'Make $@'
	rm -rf ${DIR_OBJ}/*.o ${DIR_OBJ}/*.a
	cd ./goahead && make clean
	@echo 'Finished make $@'

cleango:
	cd ./goahead && make clean

