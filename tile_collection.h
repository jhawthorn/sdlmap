#ifndef TILE_COLLECTION_H
#define TILE_COLLECTION_H TILE_COLLECTION_H

#include <stack>
#include <list>
#include <assert.h>
#include "tile.h"
#include "tile_downloader.h"

struct Range{
	int minx, miny, maxx, maxy, depth;
	bool contains(int x, int y, int d) const;
};

class Quad{
	Tile *tile;
	Quad *children[4];
	int x, y, depth;
	public:
		Quad(int x, int y, int depth);
		void queryRange(std::list<Tile *> &l, const Range &r);
		void buildChildren();
};

class TileCollection{
	Quad quad;
	Range range;
	public:
	TileCollection(): quad(0,0,0){}
	TileDownloader transfers;
	void set_bounds(int _minx, int _miny, int _maxx, int _maxy, int _zoom);
	bool work();
	void render(SDL_Surface *screen, int offsetx, int offsety);
};

#endif
