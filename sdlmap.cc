
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>

#include <curl/curl.h>

#include "SDL.h"
#include "SDL_image.h"

void die(const char* format, ...){
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);
	exit(-1);
}

#define TILESIZE 256

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


struct MemoryStruct {
	char *memory;
	size_t size;
	public:
	MemoryStruct(){
		size = 0;
		memory = (char *)malloc(1);
	}
	~MemoryStruct(){
		free(memory);
	}
};


static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

class Tile;
class MapView;
class TileCollection{
	std::list<Tile *> tiles;
	int minx, miny, maxx, maxy;
	int zoom;
	public:
	void set_bounds(int _minx, int _miny, int _maxx, int _maxy, int _zoom){
		minx = _minx;
		maxx = _maxx;
		miny = _miny;
		maxy = _maxy;
		zoom = _zoom;
		create_tiles();
	}
	void create_tiles();
	int load_some(int count);
	void render(int offsetx, int offsety);
	bool bounded(Tile &t);
};

class MapView{
	public:
		TileCollection tiles;
		Coordinate center;
		int zoom;
		int offsetx, offsety;
		SDL_Surface *surface;
		MapView(Coordinate &center, int zoom): center(center), zoom(zoom){
			surface = SDL_GetVideoSurface();
			Point centerpt = center;
			centerpt <<= zoom;
			offsetx = centerpt.x * TILESIZE - surface->w / 2;
			offsety = centerpt.y * TILESIZE - surface->h / 2;
			update_bounds();
		}
		void zoom_in(){
			zoom++;
			offsetx = offsetx * 2 + surface->w / 2;
			offsety = offsety * 2 + surface->h / 2;
		}
		void zoom_out(){
			zoom--;
			offsetx = (offsetx - surface->w / 2) / 2;
			offsety = (offsety - surface->h / 2) / 2;
		}
		void update_bounds(){
			tiles.set_bounds(offsetx / TILESIZE, offsety / TILESIZE, (offsetx + surface->w) / TILESIZE, (offsety + surface->h) / TILESIZE, zoom);
		}
		void render();
};

class Tile{
	public:
		int zoom, x, y;
		SDL_Surface *surface;
		Tile(int x, int y, int zoom): zoom(zoom), x(x), y(y), surface(NULL){};
		bool loaded(){
			return !!surface;
		}
		void load(){
			printf("loading: (%i,%i,%i)\n", x, y, zoom);
			MemoryStruct chunk;
			CURL *curl = curl_easy_init();
			CURLcode res;
			if(curl){
				char url[4096];
				//snprintf(url, sizeof url, "http://a.tile.openstreetmap.org/%i/%i/%i.png", zoom, x, y);
				snprintf(url, sizeof url, "http://a.tile.stamen.com/toner/%i/%i/%i.png", zoom, x, y);
				curl_easy_setopt(curl, CURLOPT_URL, url);
				curl_easy_setopt(curl, CURLOPT_USERAGENT, "sdlmap/1.0");
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
				res = curl_easy_perform(curl);
				if(res != CURLE_OK)
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				curl_easy_cleanup(curl);
			}
			surface = IMG_Load_RW(SDL_RWFromMem(chunk.memory, chunk.size), 1);
		}
		void render(int offsetx, int offsety){
			SDL_Rect dest = {x * TILESIZE - offsetx, y * TILESIZE - offsety, TILESIZE, TILESIZE};
			SDL_Surface *screen = SDL_GetVideoSurface();
			if(surface){
				printf("render: (%i, %i, %i) => (%i, %i)\n", x, y, zoom, dest.x, dest.y);
				SDL_BlitSurface(surface, NULL, screen, &dest);
			}else{
				SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 255, 255, 255));
			}
		}

};

/* Create tile objects for current focus */
void TileCollection::create_tiles(){
	/* TODO: make this NOT O(N^3)*/
	for(int y = miny; y <= maxy; y++){
		for(int x = minx; x <= maxx; x++){
			bool found = false;
			for(std::list<Tile *>::iterator it = tiles.begin(); it != tiles.end(); ++it){
				Tile *t = *it;
				if(t->zoom == zoom && t->x == x && t->y == y){
					found = true;
					break;
				}
			}
			if(!found){
				printf("creating: (%i,%i,%i)\n", x, y, zoom);
				tiles.push_front(new Tile(x, y, zoom));
			}
		}
	}
};

bool TileCollection::bounded(Tile &t){
	return t.zoom == zoom && t.x >= minx && t.x <= maxx && t.y >= miny && t.y <= maxy;
}
int TileCollection::load_some(int count){
	int loaded = 0;
	for(std::list<Tile *>::iterator it = tiles.begin(); loaded < count && it != tiles.end(); ++it){
		Tile *t = *it;
		if(!bounded(*t))
			continue;
		if(t->loaded())
			continue;
		t->load();
		loaded++;
	}
	return loaded;
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

void runloop(MapView &view){
	bool mousedown = false;
	bool dirty = true;
	for(;;){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
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
				case SDL_QUIT:
					exit(0);
			}
		}
		if(dirty){
			view.update_bounds();
			int loaded = view.tiles.load_some(1);
			dirty = (loaded > 0);
			view.render();
		}
	}
}

int main(int argc, char *argv[]){
	if(SDL_Init(SDL_INIT_VIDEO))
		die("SDL_Init failed: %s", SDL_GetError());
	if(!SDL_SetVideoMode(800, 600, 0, SDL_SWSURFACE))
		die("Unable to set video mode: %s\n", SDL_GetError());
	Coordinate center = {48.4284, -123.3656};
	int zoom = 14;
	MapView view(center, zoom);

	runloop(view);
	return 0;
}

