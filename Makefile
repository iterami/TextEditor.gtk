all: setup

setup: c/main.c
	mkdir -p ~/.iterami
	mkdir -p ~/.iterami/config
	mkdir -p ~/.iterami/css
	touch ~/.iterami/config/texteditor.cfg
	cp ../common/css/gtk.css ~/.iterami/css
	gcc c/main.c `pkg-config --cflags --libs gtk+-3.0` -o3 ~/.iterami/texteditor

clean:
	rm ~/.iterami/texteditor
	rm ~/.iterami/config/texteditor.cfg
