omxgtk: omxgtk.c
	gcc -O2 omxgtk.c -o omxgtk `pkg-config --cflags gtk+-2.0 --libs gtk+-2.0` -std=c99

install: omxgtk
	sudo install -g video omxgtk /usr/bin
	sudo install -g video omxgtkplayer /usr/bin
	sudo install -g video omxpipe.sh /usr/local/bin
	sudo install -g video omxpipeonce.sh /usr/local/bin
	sudo cp ./omxgtk.1.gz /usr/share/man/man1

remove:
	sudo rm /usr/bin/omxgtk
	sudo rm /usr/bin/omxgtkplayer
	sudo rm /usr/local/bin/omxpipe.sh
	sudo rm /usr/local/bin/omxpipeonce.sh
	sudo rm /usr/share/man/man1/omxgtk.1.gz

clean:
	rm ./omxgtk

tar: omxgtk
	cd .. && tar -czvf omxgtk_0.1.orig.tar.gz ./omxgtk/Makefile ./omxgtk/omxgtk.c ./omxgtk/omxpipe.sh ./omxgtk/omxpipeonce.sh ./omxgtk/omxgtkplayer ./omxgtk/omxgtk.1.gz ./omxgtk/COPYING ./omxgtk/INSTALL
edit: omxgtk.c
	vim omxgtk.c

deb: omxgtk
	debuild -us -uc
