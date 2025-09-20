# A minimalist makefile for set_rtlimits.  It gets the job done in most
# cases.

# C compiler to use
CC=$(CROSS_COMPILE)gcc

# Installation locations.  If SYSCONFDIR is changed, the CONFIG_FILE define
# in set_rtlimits.c will need changing to match.
PREFIX=$(CROSS_ROOT)/usr/local
SYSCONFDIR=/etc

all:	set_rlimits

set_rlimits:	set_rlimits.c
	$(CC) -Wall -g -o set_rlimits set_rlimits.c

install:	set_rlimits
	test -d $(PREFIX)/bin || mkdir -p $(PREFIX)/bin
	test -d $(PREFIX)/man/man8 || mkdir -p $(PREFIX)/man/man8
	test -d $(SYSCONFDIR) || mkdir -p $(SYSCONFDIR)
	cp -p set_rlimits $(PREFIX)/bin/
	chown 0.0 $(PREFIX)/bin/set_rlimits
	chmod u+s $(PREFIX)/bin/set_rlimits
	test -e $(SYSCONFDIR)/set_rlimits.conf || cp set_rlimits.conf $(SYSCONFDIR)/
	chown 0.0 $(SYSCONFDIR)/set_rlimits.conf
	cp -p set_rlimits.8 $(PREFIX)/man/man8/
	chown 0.0 $(PREFIX)/man/man8/set_rlimits.8

.PHONY:	clean tidy

clean:	
	rm -f set_rlimits *.o *~ core

tidy:
	rm -f core *~
