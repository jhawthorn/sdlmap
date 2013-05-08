
#define INK_UPDATE_PARTIAL 1
#define INK_UPDATE_FULL 0
#define INK_UPDATE_MERGE 2

void Ink_Init();
void Ink_SetVideoMode(int width, int height);
void Ink_UpdateRect(int x, int y, int w, int h, int flags);
void Ink_Update(int flags);
void Ink_Wait();

