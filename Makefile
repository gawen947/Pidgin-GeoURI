TARGET = geouri.so

CFLAGS  := -fPIC -fomit-frame-pointer -Wall -Wextra -MMD -pipe -std=c99 \
	$(shell pkg-config --cflags purple gtk+-2.0 gobject-2.0)
LDFLAGS := -shared $(shell pkg-config --libs purple gtk+-2.0 gobject-2.0)

SRC  = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)

PLUGINDIR ?= $(shell pkg-config --variable=plugindir purple)
DATADIR   ?= $(shell pkg-config --variable=datadir purple)

commit = $(shell ./hash.sh)
ifneq ($(commit), UNKNOWN)
	CFLAGS += -DCOMMIT="\"$(commit)\""
	CFLAGS += -DPARTIAL_COMMIT="\"$(shell echo $(commit) | cut -c1-8)\""
endif

ifndef DISABLE_DEBUG
	CFLAGS += -ggdb
else
	CFLAGS += -DNDEBUG=1
endif

ifdef VERBOSE
	Q :=
else
	Q := @
endif

.PHONY: all clean install uninstall

%.o: %.c
	@echo "===> CC $<"
	$(Q)$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJS)
	@echo "===> LD $@"
	$(Q)$(CC) $(OBJS) $(LDFLAGS) -o $@

install:
	install -s -m 444 $(TARGET) $(DESTDIR)$(PLUGINDIR)

clean:
	@echo "===> CLEAN"
	$(Q)rm -f *.o
	$(Q)rm -f *.d
	$(Q)rm -f $(TARGET)

-include $(DEPS)
