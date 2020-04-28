#
# libsteg Makefile
#

CC ?= gcc
PARENTDIR := .
SRCDIR := $(PARENTDIR)/src/libsteg
BUILDDIR := $(PARENTDIR)/build/libsteg
TARGETDIR := $(PARENTDIR)/lib
TARGET := $(TARGETDIR)/libsteg.so

SOURCE := $(SRCDIR)/steganography.c
OBJECT := $(BUILDDIR)/steganography.o

CPPCOMMENTDIR := $(PARENTDIR)/ext/stb
CPPCOMMENT := $(CPPCOMMENTDIR)/stb_image.h $(CPPCOMMENTDIR)/stb_image_write.h

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

all: | rm-comments c89 debug-flag makedirs $(OBJECT) $(TARGET)

debug: | rm-comments c89 debug-flag makedirs $(OBJECT) $(TARGET)

release: | rm-comments c89 release-flag makedirs $(OBJECT) $(TARGET)

$(TARGET): $(OBJECT)
	@echo ""
	@echo " Linking..."
	@echo " $(CC) -fPIC -shared $(CFLAGS) $^ -o $(TARGET) $(LIB)"; \
		$(CC) -fPIC -shared $(CFLAGS) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo " $(CC) -fPIC $(CFLAGS) $(INC) -c -o $@ $<"; \
		$(CC) -fPIC $(CFLAGS) $(INC) -c -o $@ $<

rm-comments: $(CPPCOMMENT)
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
