#ifndef __GAME_OBS
#define __GAME_OBS
#include <stdint.h>
#include "lcd.h"
#define BACKGROUND 0x94BF
#define SCREENWIDTH 320 //320
#define SCREENHEIGHT 128
#define TRANSPARENT 0xf01f

//CERTAIN CHAR TYPES DENOTE ENEMY TYPES THAT HAVE FUNCTIONALITY
typedef struct {
    short pos[2];
    char type;
    char state;
} block;

typedef const struct {
    unsigned int width;
    unsigned int height;
    unsigned char map_data[0];
} Map;

typedef struct {
    float pos[2];
    float vel[2];
    char dir;
} Goomba;

typedef struct {
    float pos[2];
    float prev[2];
    float vel[2];
    char state;
} sBlock;

typedef struct {
    float pos[2];
    float vel[2];
    char state;
} Mushroom;

void gameLogic(void);
void drawSBlocks(u16* buffer, int xb, int yb, const Picture *pic, float fOffsetX, float fOffsetY);
void render(block*, int, float, float, float, float, const Picture*, char animState, char dir, Goomba* goombs);
block* createMap(int*);
block getBlock(block*, int, int);
void clearBuffer(u16 *buffer, u16 color);
void drawPlayer(u16 *buffer, int xb, int yb, u16 posX, u16 posY, const Picture*, char animState, char dir);
void drawMobs(u16 *buffer, int xb, int yb, const Picture* pic, Goomba* goombs, float offSetX, float offSetY);
void mergeFour(u16* buffer, block* neighbors, int xb, int yb, const Picture*);
void setDMABuffer(u16 *buffer);
void drawChar(u16 *buffer, int xb, int yb, char c, u16 xs, u16 ys);
void drawString(u16 *buffer, int xb, int yb, char* string, u16 xs, u16 ys);
int checkNonCollisions(char one, char two);
void drawMushroom(u16* buffer, int xb, int yb, const Picture *pic, float fOffsetX, float fOffsetY);
#endif
