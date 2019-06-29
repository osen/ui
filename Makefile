CFLAGS= \
  -g -pedantic -std=c89 \
  -I/usr/X11R6/include -Isrc -Isrc/stent \
  -Wall -Wno-variadic-macros

LFLAGS=-L/usr/X11R6/lib -lX11

UI_SRC= \
  src/stent/stent.c \
  src/ui/Platform.c \
  src/ui/Window.c \
  src/ui/Panel.c \
  src/ui/Graphics.c \
  src/ui/Rectangle.c \
  src/ui/LayoutProcessor.c \
  src/ui/Button.c \
  src/ui/Line.c \
  src/ui/Component.c

EXAMPLE_SRC= \
  src/example/main.c

SRC=$(UI_SRC) $(EXAMPLE_SRC)

all:
	cc -oexample $(CFLAGS) $(SRC) $(LFLAGS)
