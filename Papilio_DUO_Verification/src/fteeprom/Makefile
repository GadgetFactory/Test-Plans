CC            = gcc
DEFINES       =
CFLAGS        = -O2 -std=gnu99 -Wall $(DEFINES)
INCPATH       = 
LINK          = gcc
LFLAGS        = 
LIBS          = -L./ -lftd2xx


fteeprom:  fteeprom.o 
	$(LINK) $(LFLAGS) -o fteeprom fteeprom.o  $(LIBS)

fteeprom.o: fteeprom.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o fteeprom.o fteeprom.c

clean:
	rm -rf *.o