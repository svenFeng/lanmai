ALL: clean lanmai
clean:
	rm lanmai -f
lanmai:
	g++ src/lanmai.cpp -o lanmai -ludev -levdev -I/usr/include/libevdev-1.0 -O3
