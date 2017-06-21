CXX = clang++

SDL_LIB = -L/usr/lib -lSDL -lSDL_gfx -lSDL_image -lSDL_mixer
SDL_INCLUDE = -I/usr/local/include `sdl-config --cflags `

CXXFLAGS = -std=c++0x -O2 -g  $(SDL_INCLUDE) -Iinclude -Inoudar-core/noudar-core/include -Istb
OBJS = src/CGame.o SDLVersion/main.o SDLVersion/CSDLRenderer.o src/NativeBitmap.o src/LoadPNG.o noudar-core/noudar-core/src/CPlainFileLoader.o noudar-core/noudar-core/src/Common.o
LIBS = $(SDL_LIB)
TARGET = game

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:   $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
