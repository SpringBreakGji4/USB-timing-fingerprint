CC=gcc
CFLAGS=-lusb-1.0 -lpcap -ludev

main: authenticator.o fingerprint.o
	$(CC) -o authenticator authenticator.o $(CFLAGS)
	$(CC) -o fingerprint fingerprint.o $(CFLAGS)

clean:
	rm authenticator
	rm fingerprint
	rm *.o
