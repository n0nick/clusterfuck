CPLEX=cplex121
CONCERT=concert29
CPARCH=x86-64_debian4.0_4.1

CFLAGS+= -ansi -pedantic-errors -Wall -g
CFLAGS+= -I/usr/local/stow/${CPLEX}/lib/${CPLEX}/include
CFLAGS+= -I/usr/local/stow/${CONCERT}/lib/${CONCERT}/include/
CFLAGS+= -I/usr/include/libxml2

LDFLAGS+= -L/usr/local/stow/${CPLEX}/lib/${CPLEX}/lib/${CPARCH}/static_pic -lilocplex -lcplex 
LDFLAGS+= -lm -lpthread -lgraph
LDFLAGS+= -lxml2

all: kcluster

clean:
	rm cluster.o kcluster

kcluster: cluster.o 
	gcc -o kcluster cluster.o $(CFLAGS) $(LDFLAGS)

cluster.o: cluster.h cluster.c
	gcc -c cluster.c $(CFLAGS)
