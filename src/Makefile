hockeyboard_OBJECTS = \
	ClockSocket.o \
	FreeType.o \
	FreeTypeMem.o \
	GLBitmap.o \
	GLCairoSurface.o \
	GLCairoTextSurface.o \
	HockeyData.o \
	HockeyData_2.o \
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
	serialsync_unix.o \
	settings.o \
	SyncSocket.o \
	timegettime.o \
	UDPSocket.o

hockeyboard_LIBS = -lGL -lGLU -lglut
hockeyboard_LIBS += `pkg-config --libs freetype2`
hockeyboard_LIBS += `pkg-config --libs cairo`
hockeyboard_LIBS += `pkg-config --libs pango`
hockeyboard_LIBS += `pkg-config --libs pangocairo`

CXX=g++
CFLAGS=-g -O2

CFLAGS += `pkg-config --cflags freetype2`
CFLAGS += `pkg-config --cflags pango`
CFLAGS += `pkg-config --cflags cairo`


hockeyboard: $(hockeyboard_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(hockeyboard_LIBS)

%.o : %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^

