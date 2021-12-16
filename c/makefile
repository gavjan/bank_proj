CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS	= -lpam -lpam_misc

.PHONY: all clean
all: auth app

#----------AUTH-----------
auth: auth.o
	$(CC) -o $@ $^ $(LDFLAGS)
auth.o: auth.c
	$(CC) $(CFLAGS) -c $<
#----------AUTH-----------




#----------APP------------
app: app.o safe.o menu.o list.o
	$(CC) -o $@ $^ $(LDFLAGS)
list.o: list.c list.h
	$(CC) $(CFLAGS) -c $<
safe.o: safe.c safe.h list.h
	$(CC) $(CFLAGS) -c $<
menu.o: menu.c menu.h safe.h list.h
	$(CC) $(CFLAGS) -c $<
app.o: app.c menu.h safe.h list.h
	$(CC) $(CFLAGS) -c $<
#----------APP------------

run: app
	
valgrind: app
	valgrind -q --tool=memcheck  --leak-check=full --leak-resolution=med  ./$<

clean:
	-@$(RM) app auth *.o vgcore.*