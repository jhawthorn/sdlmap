#ifndef TILE_DOWNLOADER_H
#define TILE_DOWNLOADER_H TILE_DOWNLOADER_H

#include <curl/curl.h>

#include "tile.h"

struct MemoryStruct {
	char *memory;
	size_t size;
	public:
	MemoryStruct(): size(0), memory(NULL){}
	~MemoryStruct(){
		free(memory);
	}
	int append(void *contents, int len){
		memory = (char *)realloc(memory, size + len);
		if(memory == NULL) {
			/* out of memory! */
			fprintf(stderr, "not enough memory (realloc returned NULL)\n");
			exit(-1);
		}

		memcpy(&memory[size], contents, len);
		size += len;
		return len;
	}
	static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp){
		struct MemoryStruct *mem = (struct MemoryStruct *)userp;
		return mem->append(contents, size * nmemb);
	}
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
		TileDownloader(){
			multi_handle = curl_multi_init();
		}
		bool empty(){
			return transfers.empty();
		}
		bool queueable(){
			return transfers.size() < 2;
		}
		void queue(Tile *tile);
		int work(){
			int msgs_left;
			CURLMsg *msg;
			curl_multi_perform(multi_handle, &still_running);
			while((msg = curl_multi_info_read(multi_handle, &msgs_left))){
				if(msg->msg == CURLMSG_DONE){
					for(std::list<Transfer *>::iterator it = transfers.begin(); it != transfers.end(); ++it){
						if((*it)->curl == msg->easy_handle){
							(*it)->finish();
							curl_multi_remove_handle(multi_handle, msg->easy_handle);
							curl_easy_cleanup(msg->easy_handle);
							transfers.erase(it);
							delete *it;
							break;
						}
					}
				}
			}
			return still_running;
		}
};
#endif
