hockeyboard_OBJECTS = \
	ClockSocket.o \
	FreeType.o \
	FreeTypeMem.o \
	GLBitmap.o \
	GLCairoSurface.o \
	GLCairoTextSurface.o \
	HockeyData.o \
	HockeyDraw.o \
	HockeyDrop.o \
	HockeyLogic.o \
	HockeyRoster.o \
	IniParser.o \
	keyboard.o \
	Keybuffer.o \
	main.o \
	Mclock.o \
	mouse.o \
	serialsync.o \
	settings.o \
	timegettime.o \

hockeyboard_LIBS = -lcairo -lpango -lGL -lGLUT
CXX=gcc
CFLAGS=-g -O2

CFLAGS += `pkg-config --cflags freetype2`
CFLAGS += `pkg-config --cflags pango`
CFLAGS += `pkg-config --cflags cairo`


hockeyboard: $(hockeyboard_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(hockeyboard_LIBS)

%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^

