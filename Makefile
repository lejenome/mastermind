# important directories
prefix?=/usr/local
mandir?=$(prefix)/share/man
datadir?=$(prefix)/share
localedir?=$(datadir)/locale
docdir?=$(datadir)/doc/$(PACKAGE)

PACKAGE = mastermind
locales = fr

DEFINES=-DLOCALEDIR=\"$(localedir)\"
CFLAGS += $(DEFINES) -Wall

MKDIR = mkdir -p
ifdef DEBUG
	CFLAGS += -DDEBUG
endif
ifeq (, $(filter Linux GNU GNU/%, $(shell uname -s)))
	LDFLAGS +=-liconv -lintl
	LDFLAGS+=-liconv -lintl
endif

all: mastermindcli intl
mastermindcli: core.o cli.o
	$(CC) -o $@ $^ $(LDFLAGS)
	-@chmod +x $@
%.o: src/%.c
	$(CC) -c $^ $(CFLAGS)
intl: $(locales)
$(locales): % : mastermind.pot po/%.po
	msgmerge -U po/$@.po mastermind.pot
	$(MKDIR) intl/$@/LC_MESSAGES
	msgfmt po/$@.po -c --statistics -o intl/$@/LC_MESSAGES/mastermind.mo
mastermind.pot:
	xgettext -c/ -k_ -d mastermind -o mastermind.pot src/*.c \
		--package-name=$(PACKAGE) -l C
po/%.po:
	$(MKDIR) po
	msginit --no-translator -l $(shell echo $@ |sed "s/po\/\(.*\).po/\1/") \
		-i mastermind.pot -o $@


.PHONY: clean
clean:
	-$(RM) -rf mastermindcli *.o intl mastermind.pot po/*~
