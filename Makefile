CXXFLAGS=`sdl-config --cflags` -g
LIBS=`sdl-config --libs` -lcurl -lSDL_image
TARGET=sdlmap

sdlmap: sdlmap.o tile_downloader.o tile_collection.o map_view.o tile.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

kobomap: kobomap.o tile_downloader.o tile_collection.o map_view.o tile.o ink.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) -lts

clean:
	$(RM) $(TARGET) *.o
