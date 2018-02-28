OS                    ?= linux
CC                    ?= gcc
BUILD                 ?= .

ME_COM_COMPILER       ?= 1
ME_COM_LIB            ?= 1
ME_COM_MATRIXSSL      ?= 0
ME_COM_MBEDTLS        ?= 0
ME_COM_NANOSSL        ?= 0
ME_COM_OPENSSL        ?= 0
ME_COM_OSDEP          ?= 1
ME_COM_SSL            ?= 0
ME_COM_VXWORKS        ?= 0

CFLAGS                += -fPIC -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Wl,-z,relro,-z,now -Wl,--as-needed -Wl,--no-copy-dt-needed-entries -Wl,-z,noexecstatck -Wl,-z,noexecheap -w
DFLAGS                += -DME_GOAHEAD_JAVASCRIPT=1 -DME_DEBUG=1 -D_REENTRANT -DPIC -D_FORTIFY_SOURCE=2 $(patsubst %,-D%,$(filter ME_%,$(MAKEFLAGS))) -DME_COM_COMPILER=$(ME_COM_COMPILER) -DME_COM_LIB=$(ME_COM_LIB) -DME_COM_MATRIXSSL=$(ME_COM_MATRIXSSL) -DME_COM_MBEDTLS=$(ME_COM_MBEDTLS) -DME_COM_NANOSSL=$(ME_COM_NANOSSL) -DME_COM_OPENSSL=$(ME_COM_OPENSSL) -DME_COM_OSDEP=$(ME_COM_OSDEP) -DME_COM_SSL=$(ME_COM_SSL) -DME_COM_VXWORKS=$(ME_COM_VXWORKS) 
IFLAGS                += "-I$(BUILD)/inc"
LIBS                  += -lrt -ldl -lpthread -lm

DEBUG                 ?= debug
CFLAGS-debug          ?= -g
DFLAGS-debug          ?= -DME_DEBUG
LDFLAGS-debug         ?= -g
DFLAGS-release        ?= 
CFLAGS-release        ?= -O2
LDFLAGS-release       ?= 
CFLAGS                += $(CFLAGS-$(DEBUG))
DFLAGS                += $(DFLAGS-$(DEBUG))
LDFLAGS               += $(LDFLAGS-$(DEBUG))

LIBTARGET = ../../obj/libgo.a
DIR_OBJ = $(BUILD)/obj
LIBSRC = $(wildcard *.c)
LIBOBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${LIBSRC}))

.PHONY:all
all:${LIBTARGET}

${LIBTARGET}:${LIBOBJ}
	@echo 'Building target: $@'
	ar -r $@ $^
	@echo 'Finished building target: $@'
	
$(BUILD)/obj/%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 -D_FILE_OFFSET_BITS=64 $(IFLAGS)

clean:
	@echo 'Make $@'
	rm -rf $(BUILD)/obj/*.o $(LIBTARGET) 
	@echo 'Finished make $@'

