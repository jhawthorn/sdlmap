#ifndef SDLMAP_H
#define SDLMAP_H SDLMAP_H

#define TILESIZE 256

inline int mod(int x, int div){
	x %= div;
	if(x < 0)
		x += div;
	return x;
}

#endif
