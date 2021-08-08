%target Makefile

# Emerald - A POSIX client for BerylDB.
# http://www.beryldb.com
#
# Copyright (C) 2021 Carlos F. Ferry <cferry@beryldb.com>
# 
# This file is part of BerylDB. BerylDB is free software: you can
# redistribute it and/or modify it under the terms of the BSD License
# version 3.
#
# More information about our licensing can be found at https://docs.beryl.dev

VERBOSE=1
CXX = @CXX@
COMPILER = @COMPILER_NAME@
SYSTEM = @SYSTEM_NAME@
BUILDPATH ?= $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/build/@COMPILER_NAME@-@COMPILER_VERSION@
CORECXXFLAGS = -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -pipe -Iinclude -Wall -Wextra -Wfatal-errors -Wno-write-strings -Wno-missing-field-initializers -Wshadow -Wno-switch
LDLIBS = -lstdc++
CORELDFLAGS = -rdynamic -L.
PICLDFLAGS = -fPIC -shared -rdynamic

DESTDIR := $(if $(DESTDIR),$(DESTDIR),"@DESTDIR|@")
CONPATH = "$(DESTDIR)@CONFIG_DIR@"
DATPATH = "$(DESTDIR)@DATA_DIR@"
EXAPATH = "$(DESTDIR)@EXAMPLE_DIR@"
LOGPATH = "$(DESTDIR)@LOG_DIR@"
MANPATH = "$(DESTDIR)@MANUAL_DIR@"
SCRPATH = "$(DESTDIR)@SCRIPT_DIR@"

INSTALL ?= install
INSTMODE_DIR ?= 0755
INSTMODE_BIN ?= 0755
INSTMODE_TXT ?= 0644
INSTMODE_PRV ?= 0640


ifndef DEBUG
  DEBUG=0
endif

DBGOK=0
ifeq ($(DEBUG), 0)
  CORECXXFLAGS +=  -O3
ifeq ($(COMPILER), GCC)
    CORECXXFLAGS += -g1
endif
  HEADER = std-header
  DBGOK=1
endif
ifeq ($(DEBUG), 1)
  CORECXXFLAGS += -O0 -g3 -Werror
  HEADER = debug-header
  DBGOK=1
endif
ifeq ($(DEBUG), 2)
  CORECXXFLAGS += -O2 -g3
  HEADER = debug-header
  DBGOK=1
endif
ifeq ($(DEBUG), 3)
  CORECXXFLAGS += -O0 -g0 -Werror
  HEADER = std-header
  DBGOK=1
endif

MAKEFLAGS += --no-print-directory

SOURCEPATH = $(shell pwd)

ifndef VERBOSE
  MAKEFLAGS += --silent
endif

ifndef CXXFLAGS
   CXXFLAGS = -std=c++14
endif

CORECXXFLAGS += $(CPPFLAGS) $(CXXFLAGS)
CORELDFLAGS += $(LDFLAGS)
PICLDFLAGS += $(LDFLAGS)

export BUILDPATH
export CORECXXFLAGS
export CORELDFLAGS
export CXX
export VERBOSE
export LDLIBS
export PICLDFLAGS
export SOURCEPATH

TARGET = all

ifdef BERYLDB_TARGET
    HEADER = mod-header
    TARGET = $(BERYLDB_TARGET)
endif

ifeq ($(DBGOK), 0)
  HEADER = unknown-debug-level
endif

all: finishmessage

target: $(HEADER)
	$(MAKEENV) perl make/checkenv.pl
	cd "$(BUILDPATH)"; $(MAKEENV) $(MAKE) -f real.mk $(TARGET)

debug:
	@${MAKE} DEBUG=1 all

debug-header:
	@echo " "
	@echo "· Building with debug symbols   "
	@echo "				       "

mod-header:
	@echo 'Building specific targets:'

std-header:
	@echo " "
	@echo "· Building Beryl-cli.          "
	@echo " "

finishmessage: target
	@echo " "
	@echo "* Beryl-cli compiled."
	@echo "* To install Beryl-cli, type: 'make install'"

install: target
	@-$(INSTALL) -d -g @GID@ -o @UID@ -m $(INSTMODE_DIR) $(CONPATH)
	@-$(INSTALL) -d -g @GID@ -o @UID@ -m $(INSTMODE_DIR) $(DATPATH)
	@-$(INSTALL) -d -g @GID@ -o @UID@ -m $(INSTMODE_DIR) $(LOGPATH)
	@-$(INSTALL) -d -g @GID@ -o @UID@ -m $(INSTMODE_DIR) $(MANPATH)
	@-$(INSTALL) -d -g @GID@ -o @UID@ -m $(INSTMODE_DIR) $(SCRPATH)
	-$(INSTALL) -g @GID@ -o @UID@ -m $(INSTMODE_BIN) "$(BUILDPATH)/bin/beryl-cli" .
	-$(INSTALL) -g @GID@ -o @UID@ -m $(INSTMODE_TXT) @CONFIGURE_DIRECTORY@/beryl-cli.1 $(MANPATH) 2>/dev/null
	-$(INSTALL) -g @GID@ -o @UID@ -m $(INSTMODE_TXT) docs/conf/*.example $(EXAPATH)
	@echo ""
	@echo '· Paths:'
	@echo ' '
	@echo '    Configuration:' $(CONPATH)
	@echo ""
	@echo "· Beryl-cli is now installed."
	@echo ""
	@echo 'You need to create a configuration file:' $(CONPATH)/beryl-cli.conf
	@echo 'Feel free to check our config examples:' $(EXAPATH)
	@echo "" 	
	@echo '· To start the client, run:'  ./beryl-cli
	@echo ""

Makefile: make/models/main.mk src/version.sh configure @CONF_CACHEFILE@
	./configure --update

clean:
	@echo "Cleaning           "
	-rm -f "$(BUILDPATH)/bin/beryl-cli" "$(BUILDPATH)/include" "$(BUILDPATH)/real.mk"
	-rm -rf "$(BUILDPATH)" 2>/dev/null
	-rm -rf "$(BUILDPATH)" ".configure"
	-rm -rf build
	-rm -f Makefile
	-rm -rf beryl-cli
	-rm -f include/config.h
	-rm -f core
	rm -rf @CONFIGURE_DIRECTORY@
	@echo Completed.

deinstall:
	-rm -rf $(EXAPATH)
	-rm -f $(MANPATH)/beryl-cli.1

configureclean:
	-rm -f Makefile
	rm -f include/config.h
	rm -f include/constants.h
	rm -rf @CONFIGURE_DIRECTORY@

distclean: clean configureclean
	-rm -rf "$(SOURCEPATH)/run"

help:
	@echo 'BERYLDB Makefile'
	@echo ''
	@echo 'Use: ${MAKE} [flags] [targets]'
	@echo ''
	@echo 'Settings:'
	@echo ' '
	@echo ' VERBOSE=1  Displays full command compiling.'
	@echo ' DEBUG=1    Enable debugging, for module development.'
	@echo ' DEBUG=2    Build with optimizations, for detailed traces.'
	@echo ' DEBUG=3    Fast build without improvements. '
	@echo ' DESTDIR=           Specifies destination root.'
	@echo ' -j <N>             Parallel building.'
	@echo ''
	@echo 'Targets:'
	@echo ' all       Complete build of BerylDB.'
	@echo ' install   Build and install BerylDB.'
	@echo ' debug     Compile a debug build. '
	@echo ''
	@echo ' BERYLDB_TARGET=target  Builds a user-specified target, such as	"beryldb" or "core_destroy"'
	@echo '                         Multiple targets may be separated by a space'
	@echo ''
	@echo ' clean     Cleans object files.'
	@echo ' distclean Cleans all generated files.'
	@echo ' deinstall Removes the files generated by "make install"'
	@echo

.NOTPARALLEL:

.PHONY: all target debug debug-header mod-header mod-footer std-header finishmessage install clean deinstall configureclean help
