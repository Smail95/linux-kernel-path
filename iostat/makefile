CC  = cc
CFLAGS = -Wall -pthread
LDFLAGS= -pthread

ODIR = obj

_OBJ = main.o util.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

DEPS = util.h

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)  
	
	
		
clean:
	rm -f $(ODIR)/*.o  *~ main 
