#
# steg-image Makefile
#

CC ?= gcc
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
TARGETENC := $(TARGETDIR)/steg-encode
TARGETDEC := $(TARGETDIR)/steg-decode

SOURCEENC := $(SRCDIR)/steganography.c
SOURCEDEC := $(SRCDIR)/steg_decode.c
OBJECTENC := $(BUILDDIR)/steganography.o
OBJECTDEC := $(BUILDDIR)/steg_decode.o

CPPCOMMENTDIR := ext/stb

DEBUGFLAGS := -Wall -Wextra -pedantic -Wformat=2 \
        -Wno-unused-function -Wswitch-enum -Wcast-align -Wpointer-arith \
        -Wbad-function-cast -Wno-strict-aliasing -Wstrict-overflow=4 \
        -Wfloat-conversion -Wstrict-prototypes -Winline -Wundef \
        -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op \
        -Wfloat-equal -Wredundant-decls -Wold-style-definition -ggdb3 -O0 \
        -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing
RELEASEFLAGS := -O3 -Wall -Wl,--strip-all
LIB := -lm
INC := -I ext/stb

all: | rm-comments c89 debug-flag makedirs $(OBJECTENC) $(OBJECTDEC) $(TARGETENC) $(TARGETDEC)

encoder-debug: | rm-comments c89 debug-flag makedirs $(OBJECTENC) $(TARGETENC)

encoder-release: | rm-comments c89 release-flag makedirs $(OBJECTENC) $(TARGETENC)

decoder-debug: | rm-comments c89 debug-flag makedirs $(OBJECTDEC) $(TARGETDEC)

decoder-release: | rm-comments c89 release-flag makedirs $(OBJECTDEC) $(TARGETDEC)

$(TARGETENC): $(OBJECTENC)
	@echo ""
	@echo " Linking..."
	@echo " $(CC) $(CFLAGS) $^ -o $(TARGETENC) $(LIB)"; \
		$(CC) $(CFLAGS) $^ -o $(TARGETENC) $(LIB)

$(TARGETDEC): $(OBJECTDEC)
	@echo ""
	@echo " Linking..."
	@echo " $(CC) $(CFLAGS) $^ -o $(TARGETDEC) $(LIB)"; \
		$(CC) $(CFLAGS) $^ -o $(TARGETDEC) $(LIB)


$(OBJECTENC): $(SOURCEENC)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; \
		$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(OBJECTDEC): $(SOURCEDEC)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; \
		$(CC) $(CFLAGS) $(INC) -c -o $@ $<

rm-comments: $(CPPCOMMENTDIR)/stb_image.h $(CPPCOMMENTDIR)/stb_image_write.h
	@echo " Stripping C++ style comments from $^..."
	@echo " sed -i.orig 's|[[:blank:]]*//.*||' $^"
	@sed -i.orig 's|[[:blank:]]*//.*||' $^
	@echo ""

makedirs:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(TARGETDIR)

debug-flag:
	$(eval CFLAGS += $(DEBUGFLAGS))

release-flag:
	$(eval CFLAGS += $(RELEASEFLAGS))

c89:
	$(eval DEBUGFLAGS="-std=c89" $(DEBUGFLAGS))
	$(eval RELEASEFLAGS="-std=c89" $(RELEASEFLAGS))

gnu89:
	$(eval DEBUGFLAGS="-std=gnu89" "-D_GNU_SOURCE" $(DEBUGFLAGS))
	$(eval RELEASEFLAGS="-std=gnu89" "-D_GNU_SOURCE" $(RELEASEFLAGS))

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGETDIR)" ; \
		$(RM) -r $(BUILDDIR) $(TARGETDIR)

.PHONY: all makedirs debug release clean tests
