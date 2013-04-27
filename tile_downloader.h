#ifndef TILE_DOWNLOADER_H
#define TILE_DOWNLOADER_H TILE_DOWNLOADER_H

#include <list>
#include <string>
#include <curl/curl.h>

#include "tile.h"

struct MemoryStruct {
	char *memory;
	size_t size;
	MemoryStruct();
	~MemoryStruct();
	int append(void *contents, int len);
	static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
};

class TileDownloader{
	struct Transfer{
		CURL *curl;
		Tile *tile;
		MemoryStruct chunk;
		Transfer(CURL *curl, Tile *tile);
		void finish();
	};

	CURLM *multi_handle;
	std::list<Transfer *> transfers;
	int still_running;
	public:
	TileDownloader();
	int active();
	void queue(Tile *tile);
	int work();
};
#endif
