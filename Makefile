all: texteditor

texteditor: c/texteditor.c
	mkdir -p ~/.iterami
	mkdir -p ~/.iterami/cfg
	mkdir -p ~/.iterami/css
	touch ~/.iterami/cfg/texteditor.cfg
	cp ../common/gtk/gtk.css ~/.iterami/css
	gcc c/texteditor.c `pkg-config --cflags --libs gtk+-3.0` -o ~/.iterami/texteditor

clean:
	rm ~/.iterami/texteditor
	rm ~/.iterami/cfg/texteditor.cfg
	rm ~/.iterami/css/texteditor.css
