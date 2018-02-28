DIR_OBJ = ./obj
DIR_BIN = ../bin
TARGET = ${DIR_BIN}/smart

CC = gcc
CFLAG = -c -g -Wall
LDFLAG = -lpthread -lm
# -lmxll -lm -ldl

LIBS = ${DIR_BIN}/smart.a
LIBS += ${DIR_BIN}/log.a
LIBS += ${DIR_BIN}/config.a
LIBS += ${DIR_BIN}/dev.a
LIBS += ${DIR_BIN}/handle.a
LIBS += ${DIR_BIN}/work.a
LIBS += ${DIR_BIN}/proto.a

.PHONY:all
all: ${TARGET}

${TARGET}:${LIBS}
	@echo 'Building target: $@'
	${CC}  -o ${TARGET} -L../bin    -lsmart  -lgo   -lwork -lconfig -llog  -ldev -lhandle  -lproto -ldl ${LDFLAG}
	#${CC}  -o ${TARGET} ${LDFLAG} -L../bin  -lwork  -lconfig -llog  -ldev -lhandle  -lproto  -lsmart  -lgo
	#-lgo -lsmart
	@echo 'Finished building target: $@'

${DIR_BIN}/smart.a:
	cd smart && make lib

${DIR_BIN}/log.a:
	cd log && make lib

${DIR_BIN}/config.a:
	cd config && make lib

${DIR_BIN}/handle.a:
	cd handle && make lib

${DIR_BIN}/work.a:
	cd work && make lib

${DIR_BIN}/proto.a:
	cd proto && make lib

${DIR_BIN}/dev.a:
	cd dev && make lib

clean:
	@echo 'Make $@'
	rm -rf ${DIR_BIN}/*.a $(TARGET)
	cd smart && make clean
	@echo 'Finished make $@'

