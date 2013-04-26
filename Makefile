CXXFLAGS=`sdl-config --cflags` -g
LIBS=`sdl-config --libs` -lcurl -lSDL_image
TARGET=sdlmap

$(TARGET): sdlmap.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(TARGET) *.o
