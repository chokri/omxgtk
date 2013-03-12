omxgtk: omxgtk.c
	gcc -O omxgtk.c -o omxgtk `pkg-config --cflags gtk+-2.0 --libs gtk+-2.0`

install: omxgtk
	sudo install -g video omxgtk /usr/bin
	sudo install -g video omxgtkplayer /usr/bin
	sudo install -g video omxpipe.sh /usr/local/bin
	
remove:
	sudo rm /usr/bin/omxgtk
	sudo rm /usr/bin/omxgtkplayer
	sudo rm /usr/local/bin/omxpipe.sh

tar: omxgtk
	cd .. && tar -czvf omxgtk_0.1.orig.tar.gz ./omxgtk/Makefile ./omxgtk/omxgtk.c ./omxgtk/omxpipe.sh ./omxgtk/omxgtkplayer

edit: omxgtk.c
	vim omxgtk.c
	
	
 
