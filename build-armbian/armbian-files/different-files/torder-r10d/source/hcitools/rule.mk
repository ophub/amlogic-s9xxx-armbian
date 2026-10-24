# define output directory

OUTPUT_BIN := $(shell (mkdir -p output); cd ./output; pwd)
LIBDIR := $(shell cd $(pwd))/lib
BINDIR := $(shell cd $(pwd))/$(OUTPUT_BIN)

# define compiler
CC := gcc
CXX := g++

CPPFLAGS := $(CPPFLAGS) -I$(shell (cd $(pwd)))/include

CFLAGS := -Wall -O3 -Os -pipe \
		 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
		 -D_GNU_SOURCE -D_REENTRANT \
		 $(CFLAGS)

# uses '=' let the variable parse when it's used
# COMPILE: compiles the source file specified
# COMPILEX: compiles the source file
# COMPILE_MSG: print compiling message
COMPILE = @$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		  $(CPPFLAGS) $(CFLAGS)

COMPILEX = @$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		  $(CPPFLAGS) $(CFLAGS) -c $< -o $@

COMPILECPP = @$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		  $(CPPFLAGS) $(CFLAGS)

COMPILECPPX = @$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		  $(CPPFLAGS) $(CFLAGS) -c $< -o $@

COMPILE_MSG = @echo "  CC\t$<"

LDFLAGS := -L$(SYSROOT)/lib -L$(SYSROOT)/usr/lib -L$(LIBDIR) \
	$(LDFLAGS)

# uses '=' let the variable parse when it's used
# LINK: link the objects specified
# LINKX: compiles the objects
# LINK_MSG: print linking message
LINK = @$(CC) $(CFLAGS) $(LDFLAGS)

LINKX = @$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LDFLAGS)

LINKCPP = @$(CXX) $(CFLAGS) $(LDFLAGS)

LINKCPPX = @$(CXX) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LDFLAGS)

LINK_MSG = @echo "  LN\t$^ -> $@"

AR = @arm-linux-gnueabi-ar

ARFLAGS := -rc

