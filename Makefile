CXXFLAGS=`sdl-config --cflags` -g
LIBS=`sdl-config --libs` -lcurl -lSDL_image
TARGET=sdlmap

sdlmap: sdlmap.o tile_downloader.o tile_collection.o map_view.o tile.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(TARGET) *.o
