all: texteditor

texteditor: c/texteditor.c
	mkdir -p ~/.iterami
	gcc c/texteditor.c `pkg-config --cflags --libs gtk+-3.0` -o ~/.iterami/texteditor
	mkdir -p ~/.iterami/css
	cp css/texteditor.css ~/.iterami/css

clean:
	rm ~/.iterami/texteditor
	rm ~/.iterami/css/texteditor.css
