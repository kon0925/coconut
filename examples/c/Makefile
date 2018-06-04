C=gcc
CFLAGS=-I/usr/local/BerkeleyDB.5.2/include/ -I$(BDB_ROOT)/src
LDFLAGS=-ldb-6.2 -L/usr/local/BerkeleyDB.6.2/lib/
OBJS=rep_base.o rep_msg.o rep_net.o rep_common.o

all: ex_reg_mgr

ex_reg_mgr: $(OBJS)
	${CC} ${CFLAGS} ${LDFLAGS} $^ -o $@

rep_base.o: base/rep_base.c
	${CC} ${CFLAGS} -c base/rep_base.c

rep_msg.o: base/rep_msg.c
	${CC} ${CFLAGS} -c base/rep_msg.c

rep_net.o: base/rep_net.c
	${CC} ${CFLAGS} -c base/rep_net.c

rep_common.o: common/rep_common.c
	${CC} ${CFLAGS} -c common/rep_common.c
