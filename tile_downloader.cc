
#include "tile_downloader.h"

MemoryStruct::MemoryStruct(): size(0), memory(NULL){
}
MemoryStruct::~MemoryStruct(){
	free(memory);
}
int MemoryStruct::append(void *contents, int len){
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
size_t MemoryStruct::write_callback(void *contents, size_t size, size_t nmemb, void *userp){
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;
	return mem->append(contents, size * nmemb);
}

TileDownloader::TileDownloader(): multi_handle(curl_multi_init()){
}
int TileDownloader::active(){
	return transfers.size();
}
void TileDownloader::queue(Tile *tile){
	CURL *curl = curl_easy_init();
	transfers.push_back(new Transfer(curl, tile));
	curl_multi_add_handle(multi_handle, curl);
	tile->queue();
}

int TileDownloader::work(){
	int msgs_left;
	CURLMsg *msg;
	curl_multi_perform(multi_handle, &still_running);
	while((msg = curl_multi_info_read(multi_handle, &msgs_left))){
		if(msg->msg == CURLMSG_DONE){
			for(std::list<Transfer *>::iterator it = transfers.begin(); it != transfers.end(); ++it){
				if((*it)->curl == msg->easy_handle){
					if(msg->data.result){
						fprintf(stderr, "Error loading '%s': %s\n", (*it)->tile->url().c_str(), curl_easy_strerror(msg->data.result));
					}else{
						(*it)->finish();
					}
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

TileDownloader::Transfer::Transfer(CURL *curl, Tile *tile): curl(curl), tile(tile){
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "sdlmap/1.0");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, MemoryStruct::write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_URL, tile->url().c_str());
}

void TileDownloader::Transfer::finish(){
	tile->load(chunk.memory, chunk.size);
}

