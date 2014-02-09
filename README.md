# sdlmap

sdlmap is a dead simple ["Slippy map"](http://wiki.openstreetmap.org/wiki/Slippy_Map) using SDL 2.0 and libcurl.
It may be able to be run on systems which could not handle more complex browser and javascript based map viewers.

sdlmap is meant to be a starting point for map experiments, being minimalist (*read: incomplete*) and portable (*read: I used SDL*).

![preview](https://raw.github.com/jhawthorn/sdlmap/master/preview.png)
(Map tiles courtesy of [OpenStreetMap](http://www.openstreetmap.org))

## Usage

```
$ make
$ ./sdlmap
```

## Dependencies

* SDL 2.0
* SDL\_image 2
* libcurl

### Arch linux

```
# pacman -S sdl2 sdl2_image
```

### Ubuntu

```
# apt-get install libsdl2-image-dev libcurl4-gnutls-dev
```




