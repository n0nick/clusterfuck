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
	rm cluster.o kcluster main.o files.o node.o reduction.o queue.o diameter.o xgmml_output.o

kcluster: main.o cluster.o node.o files.o reduction.o queue.o diameter.o xgmml_output.o
	gcc -o kcluster main.o cluster.o node.o files.o reduction.o queue.o diameter.o xgmml_output.o $(CFLAGS) $(LDFLAGS)

main.o: main.c
	gcc -c main.c $(CFLAGS)

cluster.o: cluster.h cluster.c
	gcc -c cluster.c $(CFLAGS)
	
node.o: node.h node.c
	gcc -c node.c $(CFLAGS)
	
files.o: files.h files.c
	gcc -c files.c $(CFLAGS)
	
reduction.o: reduction.h reduction.c
	gcc -c reduction.c $(CFLAGS)
	
queue.o: queue.h queue.c
	gcc -c queue.c $(CFLAGS)
	
diameter.o: diameter.h diameter.c
	gcc -c diameter.c $(CFLAGS)
	
xgmml_output.o: xgmml_output.h xgmml_output.c
	gcc -c xgmml_output.c $(CFLAGS)