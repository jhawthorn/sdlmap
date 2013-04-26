
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <set>
#include <string>
#include <vector>

#include "SDL.h"
#include "SDL_image.h"

#define TILESIZE 256

#include "sdlmap.h"
#include "tile.h"
#include "tile_collection.h"
#include "map_view.h"

struct Coordinate{
	double lat, lng;
};

struct Point{
	double x, y;
	Point(Coordinate &c):
		x((c.lng + 180.0f) / 360.0f),
		y((1.0f - log(tan(c.lat * M_PI / 180.0f) + 1.0f / cos(c.lat * M_PI / 180.0f)) / M_PI) / 2.0f) {}
	Point &operator<<=(int zoom){
		x *= (1 << zoom);
		y *= (1 << zoom);
		return *this;
	}
};

MapView::MapView(int width, int height, Coordinate &center, int zoom): zoom(zoom){
		resize(width, height);
		Point centerpt = center;
		centerpt <<= zoom;
		offsetx = centerpt.x * TILESIZE - surface->w / 2;
		offsety = centerpt.y * TILESIZE - surface->h / 2;
		update_bounds();
	}

/* Create tile objects for current focus */
void TileCollection::create_tiles(){
	std::set< std::pair<int, int> > existing;
	for(std::list<Tile *>::iterator it = tiles.begin(); it != tiles.end(); ++it){
		if((*it)->zoom == zoom){
			existing.insert(std::pair<int, int>((*it)->x, (*it)->y));
		}
	}

	for(int y = miny; y <= maxy; y++){
		for(int x = minx; x <= maxx; x++){
			if(existing.find(std::pair<int, int>(x, y)) == existing.end()){
				tiles.push_back(new Tile(x, y, zoom));
			}
		}
	}
};

bool TileCollection::bounded(Tile &t){
	return t.zoom == zoom && t.x >= minx && t.x <= maxx && t.y >= miny && t.y <= maxy;
}
bool TileCollection::work(){
	bool working = !transfers.empty();
	transfers.work();
	for(std::list<Tile *>::iterator it = tiles.begin(); transfers.queueable() && it != tiles.end(); ++it){
		Tile *t = *it;
		if(!bounded(*t))
			continue;
		if(t->state != Tile::EMPTY)
			continue;
		transfers.queue(t);
		working = true;
	}
	return working;
}

void TileCollection::render(int offsetx, int offsety){
	for(std::list<Tile *>::iterator it = tiles.begin(); it != tiles.end(); ++it){
		if(bounded(**it))
			(*it)->render(offsetx, offsety);
	}
	SDL_Flip(SDL_GetVideoSurface());
}

void MapView::render(){
	tiles.render(offsetx, offsety);
}

void TileDownloader::queue(Tile *tile){
	CURL *curl = curl_easy_init();
	transfers.push_back(new Transfer(curl, tile));
	curl_multi_add_handle(multi_handle, curl);
	tile->queue();
}

TileDownloader::Transfer::Transfer(CURL *curl, Tile *tile): curl(curl), tile(tile){
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "sdlmap/1.0");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, MemoryStruct::write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_URL, tile->url().c_str());
}

void TileDownloader::Transfer::finish(){
	SDL_Surface *surface = IMG_Load_RW(SDL_RWFromMem(chunk.memory, chunk.size), 1);
	tile->set_surface(surface);
}

void runloop(MapView &view){
	bool mousedown = false;
	bool dirty = true;
	for(;;){
		SDL_Event event;
		while((dirty ? SDL_PollEvent : SDL_WaitEvent)(&event)){
			switch (event.type) {
				case SDL_MOUSEBUTTONUP:
					mousedown = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					mousedown = true;
					break;
				case SDL_MOUSEMOTION:
					if(mousedown){
						view.offsetx -= event.motion.xrel;
						view.offsety -= event.motion.yrel;
						dirty = true;
					}
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_ESCAPE:
							exit(0);
							break;
						case SDLK_PLUS:
						case SDLK_EQUALS:
							view.zoom_in();
							dirty = true;
							break;
						case SDLK_MINUS:
							view.zoom_out();
							dirty = true;
							break;
						default:
							printf("The %s key was pressed!\n", SDL_GetKeyName(event.key.keysym.sym));
							break;
					}
					break;
				case SDL_VIDEORESIZE:
					view.resize(event.resize.w, event.resize.h);
					dirty = true;
					break;
				case SDL_QUIT:
					exit(0);
			}
		}
		if(dirty){
			view.update_bounds();
			dirty = view.tiles.work();
			view.render();
		}
	}
}

int main(int argc, char *argv[]){
	if(SDL_Init(SDL_INIT_VIDEO)){
		fprintf(stderr, "SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	Coordinate center = {48.4284, -123.3656};
	int zoom = 14;
	MapView view(800, 600, center, zoom);

	runloop(view);
	return 0;
}

