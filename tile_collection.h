#ifndef TILE_COLLECTION_H
#define TILE_COLLECTION_H TILE_COLLECTION_H

#include <list>
#include "tile.h"
#include "tile_downloader.h"

class TileCollection{
	std::list<Tile *> tiles;
	int minx, miny, maxx, maxy;
	int zoom;
	TileDownloader transfers;
	public:
	void set_bounds(int _minx, int _miny, int _maxx, int _maxy, int _zoom);
	void create_tiles();
	bool work();
	void render(int offsetx, int offsety);
	bool bounded(Tile &t);
};

#endif
