# important directories
prefix?=/usr/local
mandir?=$(prefix)/share/man
datadir?=$(prefix)/share
localedir?=$(datadir)/locale
docdir?=$(datadir)/doc/$(PACKAGE)

PACKAGE = mastermind
locales = fr

DEFINES=-DLOCALEDIR=\"$(localedir)\"
CFLAGS += $(DEFINES) -Wall -I/usr/local/include -Wno-char-subscripts
LDFLAGS+= -L/usr/local/lib -lreadline

MKDIR = mkdir -p
ifdef DEBUG
	CFLAGS += -DDEBUG -g
endif
ifeq (, $(filter Linux GNU GNU/%, $(shell uname -s)))
	LDFLAGS +=-liconv -lintl
endif

all: $(PACKAGE)cli intl
$(PACKAGE)cli: cli.o cli-cmd.o core.o
	$(CC) -o $@ $^ $(LDFLAGS)
	-@chmod +x $@
%.o: src/%.c
	$(CC) -c $^ $(CFLAGS)
intl: $(locales)
.NOTPARALLEL: $(locales)
$(locales): % : $(PACKAGE).pot po/%.po
	msgmerge -U po/$@.po $(PACKAGE).pot
	$(MKDIR) intl/$@/LC_MESSAGES
	msgfmt po/$@.po -c --statistics -o intl/$@/LC_MESSAGES/$(PACKAGE).mo
$(PACKAGE).pot:
	xgettext -c/ -k_ -d $(PACKAGE) -o $(PACKAGE).pot src/*.c \
		--package-name=$(PACKAGE) -l C
po/%.po:
	$(MKDIR) po
	msginit --no-translator -l $(shell echo $@ |sed "s/po\/\(.*\).po/\1/") \
		-i $(PACKAGE).pot -o $@


.PHONY: clean
clean:
	-$(RM) -rf $(PACKAGE)cli *.o intl $(PACKAGE).pot po/*~
