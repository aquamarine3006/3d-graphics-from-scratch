CC      = gcc
CFLAGS  = -Werror -Wall -Wextra -Wpedantic -O3 -march=native \
          -I$(GUIX_ENVIRONMENT)/include
LDFLAGS = -L$(GUIX_ENVIRONMENT)/lib -lSDL3 -lm
TARGET  = out
OBJS    = win.o project.o render.o main.o

.PHONY: all clean

all: $(TARGET)

win.o: win.c win.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

project.o: project.c project.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

render.o: render.c render.h project.h vector_t.h phase_t.h models/model_nefertiti.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c win.h render.h config.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
