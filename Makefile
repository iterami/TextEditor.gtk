all: setup

setup: c/main.c
	mkdir -p ~/.iterami
	mkdir -p ~/.iterami/config
	touch ~/.iterami/config/texteditor.cfg
	mkdir -p ~/.iterami/css
	cp ../common/css/gtk.css ~/.iterami/css
	mkdir -p ~/.iterami/icons
	cp icons/default.png ~/.iterami/icons
	gcc -O3 c/main.c `pkg-config --cflags --libs gtk+-3.0` -o ~/.iterami/texteditor

clean:
	rm ~/.iterami/texteditor
	rm ~/.iterami/config/texteditor.cfg
