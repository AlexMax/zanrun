VERSION =

ACC = acc/acc

ACCFLAGS = -i acc/

SHELL = /bin/sh

objects = \
	zanrun/acs/constant.o \
	zanrun/acs/luksys.o \
	zanrun/acs/zanrun.o

all: $(objects)
	cd zanrun && zip -r ../zanrun$(VERSION).pk3 *

zanrun/acs/:
	mkdir zanrun/acs/

zanrun/acs/constant.o: zanrun/acs/ zanrun/scripts/luk/constants.acs
	$(ACC) $(ACCFLAGS) zanrun/scripts/luk/constants.acs zanrun/acs/constant.o
zanrun/acs/luksys.o: zanrun/acs/ zanrun/scripts/luk/luk.acs
	$(ACC) $(ACCFLAGS) zanrun/scripts/luk/luk.acs zanrun/acs/luksys.o
zanrun/acs/zanrun.o: zanrun/acs/ zanrun/scripts/zanrun.acs
	$(ACC) $(ACCFLAGS) zanrun/scripts/zanrun.acs zanrun/acs/zanrun.o

clean:
	rm $(objects) zanrun$(VERSION).pk3
	rmdir zanrun/acs/
