ALL: clean lanmai
clean:
	rm lanmai xbox -f
lanmai:
	g++ src/lanmai.cpp lib/common.cpp -o lanmai -ludev -levdev -I/usr/include/libevdev-1.0 -O3
xbox:
	g++ src/xbox.cpp lib/common.cpp -o xbox -ludev -levdev -I/usr/include/libevdev-1.0 -std=c++17 -O3
install:
	cp lanmai $(HOME)/.local/bin/
