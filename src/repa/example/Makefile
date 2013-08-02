#
# src/Makefile
#
# 	This library is free software; you can redistribute it and/or
#	modify it under the terms of the GNU Lesser General Public
#	License as published by the Free Software Foundation version 2.1
#	of the License.
#
#

ifeq ($(shell [ ! -r ../../Makefile.opts ] && echo 1),)
    include ../../Makefile.opts
endif

all: chat

# Compilação do programa e passos das ligações de dependências
chat:
	@echo "\tCreating chat..." && \
		$(CC) $(CFLAGS) $(LDLIBS) -m32 -o ../../apps/32bits/chat chat.c ../../apps/32bits/librepa.a && \
		$(CC) $(CFLAGS) $(LDLIBS) -m64 -o ../../apps/64bits/chat chat.c ../../apps/64bits/librepa.a

clean:
	@echo "\tCLEAN lib/example" && rm -f *.o chat
