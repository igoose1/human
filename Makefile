PREFIX:=/usr
MANPREFIX:=${PREFIX}/share/man

CC = cc
LD= ${CC}
RM = rm
GZ = gzip
CFLAGS = -Wall -pedantic
LDFLAGS =

.SUFFIXES: .c .o .gz
.PHONY : all clean install uninstall

.c.o:
	@echo -e "CC $<"
	@${CC} -c ${CFLAGS} $< -o $@

human: human.o
	@echo -e "LD human"
	@${LD} $^ -o $@ ${LDFLAGS}

human.1.gz: human.1
	@echo "GZ $<"
	@${GZ} -c $< > $@

all : human human.1.gz

clean :
	${RM} -f human *.o *.gz *~

install: human human.1.gz
	install -D -m 0755 human ${DESTDIR}${PREFIX}/bin/human
	install -D -m 0644 human.1.gz ${DESTDIR}${MANPREFIX}/man1/human.1.gz

uninstall:
	${RM} ${DESTDIR}${PREFIX}/bin/human
	${RM} ${DESTDIR}${MANPREFIX}/man1/human.1.gz
