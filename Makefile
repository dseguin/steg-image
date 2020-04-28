#
# steg-image Makefile
#

CC ?= gcc
SRCDIR := src
INCDIR := $(SRCDIR)/libsteg
LIBSTEG := lib/libsteg.so
BUILDDIR := build
TARGETDIR := bin
TARGET := $(TARGETDIR)/steg-image

SOURCE := $(SRCDIR)/main.c
OBJECT := $(BUILDDIR)/main.o

DEBUGFLAGS := -Wall -Wextra -pedantic -Wformat=2 -Werror -Wfatal-errors \
        -Wno-unused-function -Wswitch-enum -Wcast-align -Wpointer-arith \
        -Wbad-function-cast -Wno-strict-aliasing -Wstrict-overflow=4 \
        -Wfloat-conversion -Wstrict-prototypes -Winline -Wundef \
        -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wlogical-op \
        -Wfloat-equal -Wredundant-decls -Wold-style-definition -ggdb3 -O0 \
        -fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing
RELEASEFLAGS := -O3 -Wall -Wl,--strip-all
LIB := -Llib -lsteg
INC := -I $(INCDIR)

all: | $(LIBSTEG) c89 debug-flag makedirs $(OBJECT) $(TARGET) cp-runner

debug: | $(LIBSTEG) c89 debug-flag makedirs $(OBJECT) $(TARGET) cp-runner

release: | $(LIBSTEG) c89 release-flag makedirs $(OBJECT) $(TARGET) cp-runner

$(TARGET): $(OBJECT)
	@echo ""
	@echo " Linking..."
	@echo " $(CC) $(CFLAGS) $^ -o $(TARGET) $(LIB)"; \
		$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; \
		$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(LIBSTEG): $(SRCDIR)/libsteg/libsteg.mk $(SRCDIR)/libsteg/*.c $(SRCDIR)/libsteg/*.h
	@echo ""
	@echo " Making libsteg.so..."
	@echo " make -f $(SRCDIR)/libsteg/libsteg.mk"; \
		make -f $(SRCDIR)/libsteg/libsteg.mk

cp-runner: runner
	@cp $< $(TARGETDIR)/steg-run
	@chmod +x $(TARGETDIR)/steg-run

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
	@echo " $(RM) -r $(BUILDDIR) $(TARGETDIR) $(LIBSTEG)" ; \
		$(RM) -r $(BUILDDIR) $(TARGETDIR) $(LIBSTEG)

.PHONY: all makedirs debug release clean tests
