SDL_CONFIG=sdl2-config
CXXFLAGS=`$(SDL_CONFIG) --cflags` -g
LIBS=`$(SDL_CONFIG) --libs` -lcurl -lSDL2_image
TARGET=sdlmap

sdlmap: sdlmap.o tile_downloader.o tile_collection.o map_view.o tile.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(TARGET) *.o
