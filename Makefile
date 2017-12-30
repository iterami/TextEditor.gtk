all: texteditor

texteditor: c/texteditor.c
	gcc c/texteditor.c `pkg-config --cflags --libs gtk+-3.0` -o texteditor

clean:
	rm texteditor
