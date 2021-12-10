#include "stm32f0xx.h"
#include "gameObjects.h"
#include <math.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
unsigned char nonCollidables[11] = {0x00, 0x22, 0x20, 0x1f, 0xfe, 0x21, 0x02, 0x1e, 0x27, 0x26, 0x25};
char swap = 0;
int frame = 0;
short blockState = 0;
short dir = 1;

extern const Map overworld1;
extern const Picture font;
extern const Picture title_screen;
extern unsigned short look_up[123][2];
extern const titleScreen;
extern sBlock spriteBlocks;
extern int activeSBlocks;
extern int activeMushroom;
extern Mushroom currShroom;
extern int numOfGoombas;
extern bigMario;
u16 block0[256]; //TILE BUFFER
u16 block1[256]; //TILE BUFFER

char scoreString[100] = "score ";
char time[4];
char highScoreString[100];

void DMA1_CH4_5_6_7_DMA2_CH3_4_5_IRQHandler(){
    DMA1->IFCR |= DMA_IFCR_CTCIF5;
    LCD_WriteData16_End();
    lcddev.select(0);
    return;
}

int checkNonCollisions(char one, char two) {
    //IF ONE IS IN NON COLLIDE AND TWO IS IN NON COLLIDE
    int check = 0;
    for(int i = 0; i < 11; i++) {
        if(one == nonCollidables[i]) {
            check++;
            break;
        }
    }

   for(int i = 0; i < 11; i++) {
       if(two == nonCollidables[i]) {
           check++;
           break;
       }
   }

   if(check == 2) {
       return 0;
   }

   return 1;
}

block* createMap(int* numOfObjects){
    *numOfObjects = overworld1.width * overworld1.height;
    block* tiles = (block*)malloc(sizeof(block) * *numOfObjects);
    int index = 0;
    int x = 0;
    int y = 0;
    for(int i = overworld1.height - 1; i >= 0; i--) {
        for(int j = 0; j < overworld1.width; j++) {
            char curr = overworld1.map_data[overworld1.width * i + j];
                tiles[index].type = curr;
                tiles[index].pos[0] = x;
                tiles[index].pos[1] = y;
                tiles[index].state = 0;
                index++;
            x += 16;
           }
        x = 0;
        y+=16;
    }
    return tiles;
}


block getBlock(block* scene, int x, int y) {
    if(x >= 0 && x < overworld1.width && y >= 0 && overworld1.height) {
        return scene[y * overworld1.width + x];
    } else {
        block none;
        none.type = -1;
    }
}

void render(block* scene, int numOfObjects, float fCameraPosX, float fCameraPosY, float fPlayerPosX, float fPlayerPosY, const Picture* pic, char animState, char dirX, Goomba* goombs) {
    //SETUP VARIABLES FOR CAMERA OFFSET AND SETTING CAMERA BOUNDERIES
    int nTileWidth= 16;
    int nTileHeight = 16;
    int nVisibleTilesX = SCREENWIDTH / nTileWidth;
    int nVisibleTilesY = SCREENHEIGHT / nTileHeight;

    float fOffsetX = fCameraPosX - (float) nVisibleTilesX / 2.0f;
    float fOffsetY = fCameraPosY - (float) nVisibleTilesY / 2.0f;

    if(fOffsetX < 0) fOffsetX = 0;
    if (fOffsetY < 0) fOffsetY = 0;
    if (fOffsetX > overworld1.width - nVisibleTilesX) fOffsetX = overworld1.width - nVisibleTilesX;
    if (fOffsetY > overworld1.height - nVisibleTilesY) fOffsetY = overworld1.height - nVisibleTilesY;

    float fTileOffsetX = (fOffsetX - (int)fOffsetX) * nTileWidth;
    float fTileOffsetY = (fOffsetY - (int)fOffsetY) * nTileHeight;


    //FILL AND SEND EACH TILE BUFFER TO THE DMA
    for(int x = 0; x < nVisibleTilesX; x++) {
        for(int y = 0; y < nVisibleTilesY; y++) {
            block fourSet[4];
            fourSet[0] = getBlock(scene, x + fOffsetX, y + fOffsetY); //BOTTOM LEFT
            fourSet[0].pos[0] = x * nTileWidth - fTileOffsetX;
            fourSet[0].pos[1] = y * nTileHeight - fTileOffsetY;
            fourSet[1] = getBlock(scene, x + fOffsetX + 1, y + fOffsetY); //BOTTOM RIGHT
            fourSet[1].pos[0] = (x + 1) * nTileWidth - fTileOffsetX;
            fourSet[1].pos[1] = y * nTileHeight - fTileOffsetY;
            fourSet[2] = getBlock(scene, x + fOffsetX, y + fOffsetY + 1); //TOP LEFT
            fourSet[2].pos[0] = x * nTileWidth - fTileOffsetX;
            fourSet[2].pos[1] = (y+1) * nTileHeight - fTileOffsetY;
            fourSet[3] = getBlock(scene, x + fOffsetX + 1, y + fOffsetY + 1); //TOP RIGHT
            fourSet[3].pos[0] = (x+1) * nTileWidth - fTileOffsetX;
            fourSet[3].pos[1] = (y+1) * nTileHeight - fTileOffsetY;

            //CHECK FOR BLOCKS FITTING IN BUFFER. RASTERIZE IN BOUND BLOCKS WITH CLIPPING
            u16 *buffer = (swap == 0)? block0 : block1; //-->SELECTING THE BUFFER NOT BEING DRAWN CURRENTLY
            swap = (swap == 0)? 1 : 0; //SWAP BUFFERS
            mergeFour(buffer, fourSet, x*16, y*16, pic);

            //RASTERIZE SPRITES AND PLAYER
            drawMushroom(buffer, x*16, y*16, pic, fOffsetX, fOffsetY);
            drawSBlocks(buffer, x*16, y*16, pic, fOffsetX, fOffsetY);
            drawMobs(buffer, x*16, y*16, pic, goombs, fOffsetX, fOffsetY);
            drawPlayer(buffer, x*16, y*16, (fPlayerPosX - fOffsetX) * nTileWidth, (fPlayerPosY - fOffsetY) * nTileWidth, pic, animState, dirX);

            //DRAW CHARACTERS
            if(!titleScreen)
            drawString(buffer, x*16, y*16, scoreString, (1)*16, (7)*16);
            drawString(buffer, x*16, y*16, time, (17)*16, (7)*16);

            //DRAWING TITLESCREEN
            if(titleScreen){
                drawTitleScreen(buffer, x*16, y*16, title_screen, (6)*16, (3)*16);
                drawString(buffer, x*16, y*16, highScoreString, (7)*16, (2.5)*16);
            }

            //SEND BUFFER TO DMA FOR DRAWING
            lcddev.select(1);
            LCD_SetWindow(y*16, x*16, y*16 + 15, x*16 + 15);
            LCD_WriteData16_Prepare();
            setDMABuffer(buffer);
        }
    }

    frame++; //INCREMENT FRAME
    if(frame % 1 == 0) {
        blockState += dir;
        if(blockState >= 2) {
            dir = -1;
        } else if(blockState <= -1) {
            dir = 1;
        }
    }
}

void mergeFour(u16* buffer, block* neighbors, int xb, int yb, const Picture* pic) {
    for(int i = 0; i < 4; i++) {
        block curr = neighbors[i];
        if(curr.type == 0xfc) {
            curr.type = 0x1B;
        }
        //RASTERIZE CURRENT BLOCK
        u16 mX0 = MAX(curr.pos[0], xb);
        u16 mX1 = MIN(curr.pos[0] + 16, xb + 16);
        u16 mY0 = MAX(curr.pos[1], yb);
        u16 mY1 = MIN(curr.pos[1] + 16, yb + 16);

        u16 *data = (u16 *)pic->pixel_data; //INDEX TEXTURE FROM SPRITE SHEET
        if((curr.type == 0x1B || curr.type == 0x1e) && blockState >= 0) {
            curr.state = blockState;
        }

        for(int x = mX0; x < mX1; x++) {
            for(int y = mY0; y < mY1; y++) {
                int xCoord = (x - xb) > 0? x - xb : 0; //INDEX ON THE BUFFER
                int yCoord = (y - yb) > 0? y - yb : 0; //OFFSET TEXTURE FOR WHEN TEXTURE IS BETWEEN BUFERS
                int texDisplacement = xb - curr.pos[0];
                int teyDisplacement = yb - curr.pos[1];
                int xTemp = 15 - xCoord;
                u16 color;
                if(curr.type == 0xfe){
                    color = data[(((0x22 - curr.state) * 16) + yCoord + teyDisplacement) * pic->width + xTemp - texDisplacement];
                } else if(curr.type == 0x01){
                    color = 0x00;
                } else {
                    color = data[(((curr.type - curr.state) * 16) + yCoord + teyDisplacement) * pic->width + xCoord + texDisplacement];
                }

                buffer[xCoord * 16 + yCoord] = color;
            }
        }
    }
}

void drawPlayer(u16 *buffer, int xb, int yb, u16 posX, u16 posY, const Picture* pic, char animState, char dirX) {
    if(!bigMario){
        u16 mX0 = MAX(posX, xb);
        u16 mX1 = MIN(posX + 16, xb + 16);
        u16 mY0 = MAX(posY, yb);
        u16 mY1 = MIN(posY + 16, yb + 16);
        u16 *data = (u16 *)pic->pixel_data; //INDEX TEXTURE FROM SPRITE SHEET

        for(int x = mX0; x < mX1; x++) {
            for(int y = mY0; y < mY1; y++) {
                int xCoord = (x - xb) > 0? x - xb : 0; //INDEX ON THE BUFFER
                int yCoord = (y - yb) > 0? y - yb : 0;
                int texDisplacement = xb - posX; //OFFSET TEXTURE FOR WHEN TEXTURE IS BETWEEN BUFERS
                int teyDisplacement = yb - posY;
                int xTemp = 15 - xCoord;
                u16 color;
                if(dirX == 0 && animState != 4){
                    color = data[((16 * (53 - animState)) + yCoord + teyDisplacement) * pic->width + xCoord + texDisplacement];
                } else if(dirX != 0 && animState != 4){
                    color = data[((16 * (53 - animState)) + yCoord + teyDisplacement) * pic->width + xTemp - texDisplacement];
                } else {
                    color = data[((16 * (8)) + yCoord + teyDisplacement) * pic->width + xTemp - texDisplacement];
                }

                if(color != TRANSPARENT) {
                    buffer[xCoord * 16 + yCoord] = color;
                }
            }
        }
    } else {

    }
}


void drawMobs(u16 *buffer, int xb, int yb, const Picture* pic, Goomba* goombs, float fOffsetX, float fOffsetY){
    for(int i = 0; i < numOfGoombas; i++) {
        u16 gposX = (goombs[i].pos[0] - fOffsetX) * 16;
        u16 gposY = (goombs[i].pos[1] - fOffsetY) * 16;

        u16 mX0 = MAX(gposX, xb);
        u16 mX1 = MIN(gposX + 16, xb + 16);
        u16 mY0 = MAX(gposY, yb);
        u16 mY1 = MIN(gposY + 16, yb + 16);
        u16 *data = (u16 *)pic->pixel_data; //INDEX TEXTURE FROM SPRITE SHEET

        for(int x = mX0; x < mX1; x++) {
           for(int y = mY0; y < mY1; y++) {
               int xCoord = (x - xb) > 0? x - xb : 0; //INDEX ON THE BUFFER
               int yCoord = (y - yb) > 0? y - yb : 0;
               int texDisplacement = xb - gposX; //OFFSET TEXTURE FOR WHEN TEXTURE IS BETWEEN BUFFERS
               int teyDisplacement = yb - gposY;
               int xTemp = 15 - xCoord;
               u16 color;
               switch(goombs[i].dir & 0xFF){
               case 0:
                   color = data[((16 * (49)) + yCoord + teyDisplacement) * pic->width + xCoord + texDisplacement];
                   break;
               case 1:
                   color = data[((16 * (49)) + yCoord + teyDisplacement) * pic->width + xTemp - texDisplacement];
                   break;
               case 8:
                   color = TRANSPARENT;
                   break;
               default:
                  color = data[((16 * (48)) + yCoord + teyDisplacement) * pic->width + xCoord + texDisplacement];
                   break;
               }
               if(color != TRANSPARENT) {
                   buffer[xCoord * 16 + yCoord] = color;
               }
           }
        }
    }
}

void drawChar(u16 *buffer, int xb, int yb, char c, u16 xs, u16 ys) {
    u16 mX0 = MAX(xs, xb);
    u16 mX1 = MIN(xs + 16, xb + 16);
    u16 mY0 = MAX(ys, yb);
    u16 mY1 = MIN(ys + 16, yb + 16);
    u16 *data = (u16*)font.pixel_data; //INDEX TEXTURE FROM SPRITE SHEET

    for(int x = mX0; x < mX1; x++) {
            for(int y = mY0; y < mY1; y++) {
                int xCoord = (x - xb) > 0? x - xb : 0; //INDEX ON THE BUFFER
                int yCoord = (y - yb) > 0? y - yb : 0;
                int texDisplacement = xb - xs;
                int teyDisplacement = yb - ys;
                int xTemp = 15 - xCoord;
                u16 color = data[(yCoord + teyDisplacement + 16*(look_up[(int) c][1])) * font.width + (xTemp + (int)(16*14.5) - texDisplacement - 16*(look_up[(int) c][0]))];

                if(color != 0x00) {
                    buffer[xCoord * 16 + yCoord] = color;
                }
            }
    }
}

void drawString(u16 *buffer, int xb, int yb, char* string, u16 xs, u16 ys) {
    int i = 0;
    while(string[i] != 0) {
        drawChar(buffer, xb, yb, string[i], xs + i*8, ys);
        i++;
    }
}

void drawTitleScreen(u16* buffer, int xb, int yb, const Picture pic, u16 xt, u16 yt) {
    for(int shiftX = 0; shiftX < 8; shiftX++) {
        for(int shiftY = 0; shiftY < 4; shiftY++) {
            u16 mX0 = MAX(xt + shiftX*16, xb);
            u16 mX1 = MIN(xt + shiftX*16 + 16, xb + 16);
            u16 mY0 = MAX(yt + shiftY*16, yb);
            u16 mY1 = MIN(yt + shiftY*16 + 16, yb + 16);
            u16 *data = (u16*)title_screen.pixel_data; //INDEX TEXTURE FROM SPRITE SHEET

            for(int x = mX0; x < mX1; x++) {
                for(int y = mY0; y < mY1; y++) {
                    int xCoord = (x - xb) > 0? x - xb : 0; //INDEX ON THE BUFFER
                    int yCoord = (y - yb) > 0? y - yb : 0;

                    u16 color = data[(yCoord + shiftY*16) * title_screen.width + xCoord+shiftX*16];

                    if(color != BACKGROUND) {
                        buffer[xCoord * 16 + yCoord] = color;
                    }
                }
            }
        }
    }
}

void drawSBlocks(u16* buffer, int xb, int yb, const Picture *pic, float fOffsetX, float fOffsetY) {
    if(activeSBlocks){
        u16 xPos = (spriteBlocks.pos[0] - fOffsetX) * 16;
        u16 yPos = (spriteBlocks.pos[1] - fOffsetY) * 16;

        u16 mX0 = MAX(xPos, xb);
        u16 mX1 = MIN(xPos + 16, xb + 16);
        u16 mY0 = MAX(yPos, yb);
        u16 mY1 = MIN(yPos + 16, yb + 16);
        u16 *data = (u16*)pic -> pixel_data; //INDEX TEXTURE FROM SPRITE SHEET

        for(int x = mX0; x < mX1; x++) {
            for(int y = mY0; y < mY1; y++) {
                int xCoord = (x - xb) > 0? x - xb : 0; //INDEX ON THE BUFFER
                int yCoord = (y - yb) > 0? y - yb : 0; //OFFSET TEXTURE FOR WHEN TEXTURE IS BETWEEN BUFERS
                int texDisplacement = xb - xPos;
                int teyDisplacement = yb - yPos;
                u16 color;

                color = data[((7 * 16) + yCoord + teyDisplacement) * pic->width + xCoord + texDisplacement];

                buffer[xCoord * 16 + yCoord] = color;
            }
        }
    }
}

void drawMushroom(u16* buffer, int xb, int yb, const Picture *pic, float fOffsetX, float fOffsetY) {
    if(activeMushroom) {
        u16 xPos = (currShroom.pos[0] - fOffsetX) * 16;
        u16 yPos = (currShroom.pos[1] - fOffsetY) * 16;

        u16 mX0 = MAX(xPos, xb);
        u16 mX1 = MIN(xPos + 16, xb + 16);
        u16 mY0 = MAX(yPos, yb);
        u16 mY1 = MIN(yPos + 16, yb + 16);
        u16 *data = (u16*)pic -> pixel_data; //INDEX TEXTURE FROM SPRITE SHEET

        for(int x = mX0; x < mX1; x++) {
            for(int y = mY0; y < mY1; y++) {
                int xCoord = (x - xb) > 0? x - xb : 0; //INDEX ON THE BUFFER
                int yCoord = (y - yb) > 0? y - yb : 0; //OFFSET TEXTURE FOR WHEN TEXTURE IS BETWEEN BUFERS
                int texDisplacement = xb - xPos;
                int teyDisplacement = yb - yPos;
                u16 color;

                color = data[((5 * 16) + yCoord + teyDisplacement) * pic->width + xCoord + texDisplacement];
                if(color != TRANSPARENT){
                    buffer[xCoord * 16 + yCoord] = color;
                }
            }
        }
    }
}


void setDMABuffer(u16 *buffer) {
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;
    DMA1_Channel5->CMAR = buffer;
    DMA1_Channel5->CNDTR = 257;
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}
