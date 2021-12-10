#include "stm32f0xx.h"
#include "lcd.h"
#include "sd.h"
#include "gameObjects.h"
#include <math.h>
#include "midi.h"
#include "gpio.h"

extern const Picture game_tileset;
extern char scoreString[100];
extern char highScoreString[100];
extern char time[3];
extern Map overworld1;

Goomba goombs[8];
sBlock spriteBlocks;
Mushroom currShroom;

int gameOverMusic = 0;
int flagWinMusic = 0;
int gameTime = 0;
int scoreTime = 400;
int musicFlip = 0;
int musicFlipWin = 0;
int titleScreen = 1;
int numOfSBlocks = 3;
int activeSBlocks = 0;
int activeMushroom = 0;
int numOfGoombas = 8;
int bigMario = 0;

int main(void)
{
    // Set up
    enable_gpio();
    initDMA();

    // High-score
    int highscore = read_highscore("hs.txt");
    int score = 0;

    // Init Variables
    float fElapsedTime = .01;
    float fPlayerPosX = 4.0;
    float fPlayerPosY = 2.0;
    float fPlayerVelX = 0.0;
    float fPlayerVelY = 0.0;

    float fCameraPosX = 10.0;
    float fCameraPosY = 0.0;

    char gameOver = 0;
    char win = 0;
    int nDirModX = 0;
    char animState = 0;
    int numOfObjects = 0;
    int frame = 0;
    char bPlayerOnGround = 1;

    LCD_Setup();
    LCD_Clear(0x94BF);
    block* scene = createMap(&numOfObjects);

    //SET GOOMBAS AND POSITIONS
    Goomba* goombs = (Goomba*) malloc(sizeof(Goomba) * 8);
    goombs[0].pos[0] = 49.0;
    goombs[0].pos[1] = 2.0;
    goombs[0].vel[0] = 20.0f;
    goombs[0].vel[1] = 0.0f;
    goombs[0].dir = 0;

    goombs[1].pos[0] = 54.0;
    goombs[1].pos[1] = 2;
    goombs[1].vel[0] = -20.0f;
    goombs[1].vel[1] = 0;
    goombs[1].dir = 0;

    goombs[2].pos[0] = 40.0;
    goombs[2].pos[1] = 2;
    goombs[2].vel[0] = -20.0f;
    goombs[2].vel[1] = 0.0f;
    goombs[2].dir = 0;

    goombs[3].pos[0] = 81.0f;
    goombs[3].pos[1] = 10.0f;
    goombs[3].vel[0] = -20.0f;
    goombs[3].vel[1] = 0;
    goombs[3].dir = 0;

    goombs[4].pos[0] = 84.0f;
    goombs[4].pos[1] = 10.0f;
    goombs[4].vel[0] = -20.0f;
    goombs[4].vel[1] = 0;
    goombs[4].dir = 0;

    goombs[5].pos[0] = 131.0f;
    goombs[5].pos[1] = 2.0f;
    goombs[5].vel[0] = -20.0f;
    goombs[5].vel[1] = 0;
    goombs[5].dir = 0;

    goombs[6].pos[0] = 129.0f;
    goombs[6].pos[1] = 2.0f;
    goombs[6].vel[0] = -20.0f;
    goombs[6].vel[1] = 0;
    goombs[6].dir = 0;

    goombs[7].pos[0] = 127.0f;
    goombs[7].pos[1] = 2.0f;
    goombs[7].vel[0] = -20.0f;
    goombs[7].vel[1] = 0;
    goombs[7].dir = 0;

    /// MUSIC SETUP ///
    init_wavetable();
    enableDAC1Ports();
    setup_DAC();
    init_tim6();
    init_tim2();
    midiPlayerMusic();
    ///////////////////

    sprintf(highScoreString, "highscore %d", highscore);

    while(titleScreen) { //TITLE SCREEN LOOP
        if((GPIOC->IDR & GPIO_ODR_0) != 0 && !gameOver) {
            titleScreen = 0;
        }

        render(scene, numOfObjects, fCameraPosX, fCameraPosY, fPlayerPosX, fPlayerPosY, &game_tileset, animState, nDirModX, goombs);
    }

    while(1) { //GAMELOOP
            //READ PLAYER CONTROLS
        if((GPIOC->IDR & GPIO_ODR_0) != 0 && !gameOver) {
                    if(bPlayerOnGround) {
                        animState = 3;
                        fPlayerVelY = 88.0f;
                    }
                }
                else if(fPlayerVelY > 0){
                    fPlayerVelY += -700.0f * fElapsedTime;
                }

                if((GPIOC->IDR & GPIO_ODR_1) != 0 && !gameOver) {
                    nDirModX = 0;
                    fPlayerVelX += 800 * fElapsedTime;
                    switch(animState) {
                    case 0:
                        animState = 1;
                        if(fPlayerVelY < 0) {
                            animState = 3;
                        }
                        break;
                    case 1:
                        animState = 2;
                        if(fPlayerVelY < 0) {
                            animState = 3;
                        }
                        break;
                    case 2:
                        animState = 1;
                        break;
                    case 3:
                        if(fPlayerVelY == 0) {
                            animState = 1;
                        }
                        break;
                    }
                }

                if((GPIOC->IDR & GPIO_ODR_2) != 0 && !gameOver) {
                    nDirModX = 1;
                    fPlayerVelX += -800 * fElapsedTime;
                    switch(animState) {
                    case 0:
                        animState = 1;
                        if(fPlayerVelY < 0) {
                            animState = 3;
                        }
                        break;
                    case 1:
                        animState = 2;
                        if(fPlayerVelY < 0) {
                            animState = 3;
                        }
                        break;
                    case 2:
                        animState = 1;
                        break;
                    case 3:
                        if(fPlayerVelY == 0) {
                            animState = 1;
                        }
                        break;
                    }
                }


            if((GPIOB->IDR & GPIO_ODR_9) != 0)
            {
                clear_highscore("hs.txt");

            }

            if((GPIOC->IDR & GPIO_ODR_3) != 0) {
                score++;
            }
            //else {GPIOC->BSRR |= GPIO_BSRR_BR_9; }

            if(win != 1){
                        fPlayerVelY += -850.0f * fElapsedTime;
                    } else {
                        fPlayerVelY = 0;
                        fPlayerVelX = 0;
                    }

            if(bPlayerOnGround) {
                fPlayerVelX += -18.0f * fPlayerVelX * fElapsedTime;
                if(fabs(fPlayerVelX) < .05f) {
                    fPlayerVelX = 0.0f;
                }
            } else if(!bPlayerOnGround) {
                fPlayerVelX += -10.0f * fPlayerVelX * fElapsedTime;
                if(fabs(fPlayerVelX) < .05f) {
                    fPlayerVelX = 0.0f;
                }
            }

            float fNewPlayerPosX = fPlayerPosX + fPlayerVelX * fElapsedTime;
            float fNewPlayerPosY = fPlayerPosY + fPlayerVelY * fElapsedTime;


            //Check Collisions
            if(fPlayerVelX <= 0 && !gameOver) {
               block one = getBlock(scene, fNewPlayerPosX + 0.0f, fPlayerPosY + 0.0f);
               block two = getBlock(scene, fNewPlayerPosX + 0.0f, fPlayerPosY + 0.9f);
               if(checkNonCollisions(one.type, two.type)) {
                   fNewPlayerPosX = (int)fNewPlayerPosX + 1;
                   fPlayerVelX = 0;
               } else {
                   if(one.type == 0x1e){
                       scene[(int)(fNewPlayerPosY + 0.9f) * overworld1.width + (int)(fNewPlayerPosX)].type = 0x00;
                       score+=10;
                   }

                   if(one.type == 0x2){
                       win = 1;
                       gameOver = 2;
                   } else if(one.type == 0x3) {
                       win = 1;
                       gameOver = 2;
                       score+=10;
                   }

                   if(two.type == 0x2){
                       win = 1;
                       gameOver = 2;
                   } else if(two.type == 0x3) {
                       win = 1;
                       gameOver = 2;
                       score+=10;
                   }
               }
            } else if(fPlayerVelX > 0 && !gameOver) {
                block one = getBlock(scene, fNewPlayerPosX + 1.0f, fPlayerPosY + 0.0f);
                block two = getBlock(scene, fNewPlayerPosX + 1.0f, fPlayerPosY + 0.9f);
                if(checkNonCollisions(one.type, two.type)) {
                    fNewPlayerPosX = (int)fNewPlayerPosX;
                    fPlayerVelX = 0;
                } else {
                    if(two.type == 0x1e) {
                        scene[(int)(fNewPlayerPosY + 0.9) * overworld1.width + (int)(fNewPlayerPosX + 1)].type = 0x00;
                        score+=10;
                    }
                }

                if(one.type == 0x2){
                    win = 1;
                    gameOver = 2;
                } else if(one.type == 0x3) {
                    win = 1;
                    gameOver = 2;
                    score+=10;
                }

                if(two.type == 0x2){
                    win = 1;
                    gameOver = 2;
                } else if(two.type == 0x3) {
                    win = 1;
                    gameOver = 2;
                    score+=10;
                }
            }

            if(fPlayerVelY < 0 && !bPlayerOnGround) {
                animState = 3;
            }

            bPlayerOnGround = 0;
            if(fPlayerVelY >= 0 && !gameOver){
                block one = getBlock(scene, fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f);
                block two = getBlock(scene,  fNewPlayerPosX + 0.9f, fNewPlayerPosY + 1.0f);

                if(checkNonCollisions(one.type, two.type)) {
                    fNewPlayerPosY = (int)fNewPlayerPosY;
                    fPlayerVelY = 0;
                    if(one.type == 0x1B || one.type == 0xFC){
                        scene[(int)(fNewPlayerPosY + 1) * overworld1.width + (int)(fNewPlayerPosX)].type = 0x00;
                        spriteBlocks.state = 1;
                        spriteBlocks.pos[0] = (int)fNewPlayerPosX;
                        spriteBlocks.pos[1] = (int)(fNewPlayerPosY + 1);
                        spriteBlocks.prev[0] = (int)fNewPlayerPosX;
                        spriteBlocks.prev[1] = (int)(fNewPlayerPosY + 1);
                        spriteBlocks.vel[0] = 0;
                        spriteBlocks.vel[1] = 25;
                        activeSBlocks = 1;
                        score++;
                        if(one.type == 0xFC){
                            activeMushroom = 1;
                            currShroom.pos[0] = (int)(fNewPlayerPosX);
                            currShroom.pos[1] = (int)(fNewPlayerPosY + 1);
                            currShroom.vel[0] = 20;
                            currShroom.vel[1] = 30;
                        }
                    } else if(two.type == 0x1B || two.type == 0xFC) {
                        scene[(int)(fNewPlayerPosY + 1) * overworld1.width + (int)(fNewPlayerPosX + 0.9f)].type = 0x00;
                        spriteBlocks.state = 1;
                        spriteBlocks.pos[0] = (int)(fNewPlayerPosX + 0.9f);
                        spriteBlocks.pos[1] = (int)(fNewPlayerPosY + 1);
                        spriteBlocks.prev[0] = (int)(fNewPlayerPosX + 0.9f);
                        spriteBlocks.prev[1] = (int)(fNewPlayerPosY + 1);
                        spriteBlocks.vel[0] = 0;
                        spriteBlocks.vel[1] = 25;
                        activeSBlocks = 1;
                        score++;
                        if(two.type == 0xFC){
                            activeMushroom = 1;
                            currShroom.pos[0] = (int)(fNewPlayerPosX + 0.9f);
                            currShroom.pos[1] = (int)(fNewPlayerPosY + 1);
                            currShroom.vel[0] = 20;
                            currShroom.vel[1] = 30;
                        }
                    }
                } else {
                    if(one.type == 0x1e){
                        scene[(int)(fNewPlayerPosY + 1) * overworld1.width + (int)(fNewPlayerPosX + .9)].type = 0x00;
                        score+=10;
                    }

                    if(one.type == 0x2){
                        win = 1;
                        gameOver = 2;
                    } else if(one.type == 0x3) {
                        win = 1;
                        gameOver = 2;
                        score+=10;
                    }

                    if(two.type == 0x2){
                        win = 1;
                        gameOver = 2;
                    } else if(two.type == 0x3) {
                        win = 1;
                        gameOver = 2;
                        score+=10;
                    }
                }
            } else if(fPlayerVelY < 0 && !gameOver){
                block one = getBlock(scene, fNewPlayerPosX + 0.0, fNewPlayerPosY);
                block two = getBlock(scene, fNewPlayerPosX + 0.9f, fNewPlayerPosY);

                if(checkNonCollisions(one.type, two.type)) {
                    fNewPlayerPosY = (int)fNewPlayerPosY + 1;
                    fPlayerVelY = 0;
                    bPlayerOnGround = 1; // Player has a solid surface underfoot
                    if((GPIOC->IDR & GPIO_ODR_1) == 0 && (GPIOC->IDR & GPIO_ODR_2) == 0) {
                        animState = 0;
                    }
                } else {
                    if(one.type == 0x1e){
                        scene[(int)(fNewPlayerPosY) * overworld1.width + (int)(fNewPlayerPosX + .9)].type = 0x00;
                        score += 10;
                    }

                    if(one.type == 0x2){
                        win = 1;
                        gameOver = 2;
                    } else if(one.type == 0x3) {
                        win = 1;
                        gameOver = 2;
                        score+=10;
                    }

                    if(two.type == 0x2){
                        win = 1;
                        gameOver = 2;
                    } else if(two.type == 0x3) {
                        win = 1;
                        gameOver = 2;
                        score+=10;
                    }
                }
            }

            fPlayerPosX = fNewPlayerPosX;
            fPlayerPosY = fNewPlayerPosY;

            fCameraPosX = fPlayerPosX;
            fCameraPosY = fPlayerPosY;

            if(fPlayerPosY <= 0.25 && gameOver == 0) {
                fPlayerPosY = .25;
                fPlayerVelY = 80.0;
                fPlayerVelX = 0;
                gameOver = 1;
            }

            //GOOMBA LOGIC
            if(!gameOver) {
                for(int i = 0; i < numOfGoombas; i++) {
                    if((goombs[i].dir & 0xFF) != 0x2 && (goombs[i].dir & 0xFF) != 0x4 && (goombs[i].dir & 0xFF) != 0x8 && abs(fCameraPosX - goombs[i].pos[0]) < SCREENWIDTH / 16){
                        goombs[i].vel[1] += -850.0f * fElapsedTime;
                        float fNewGoombPosX = goombs[i].pos[0] + goombs[i].vel[0] * fElapsedTime;
                        float fNewGoombPosY = goombs[i].pos[1] + goombs[i].vel[1] * fElapsedTime;

                        //X VEL
                        if(goombs[i].vel[0] <= 0) {
                            block one = getBlock(scene, fNewGoombPosX + 0.0f, goombs[i].pos[1] + 0.0f);
                            block two = getBlock(scene, fNewGoombPosX + 0.0f, goombs[i].pos[1] + 0.9f);
                           if(checkNonCollisions(one.type, two.type)) {
                               fNewGoombPosX = (int)fNewGoombPosX + 1;
                               goombs[i].vel[0] = -goombs[i].vel[0];
                           }
                        } else {
                            block one = getBlock(scene, fNewGoombPosX + 1.0f, goombs[i].pos[1] + 0.0f);
                            block two = getBlock(scene, fNewGoombPosX + 1.0f, goombs[i].pos[1] + 0.9f);
                            if(checkNonCollisions(one.type, two.type)) {
                                fNewGoombPosX = (int)fNewGoombPosX;
                                goombs[i].vel[0] = -goombs[i].vel[0];
                            }
                        }

                        //Y VEL
                        if(goombs[i].vel[1] >= 0) {
                            block one = getBlock(scene, fNewGoombPosX + 0.0f, fNewGoombPosY + 1.0f);
                            block two = getBlock(scene, fNewGoombPosX + 0.9f, fNewGoombPosY + 1.0f);
                            if(checkNonCollisions(one.type, two.type)) {
                                 fNewGoombPosY = (int)fNewGoombPosY;
                                 goombs[i].vel[1] = 0;
                              }
                        } else {
                            block one = getBlock(scene, fNewGoombPosX + 0.0f, fNewGoombPosY);
                            block two = getBlock(scene, fNewGoombPosX + 0.9f, fNewGoombPosY + 0.0f);
                            if(checkNonCollisions(one.type, two.type)) {
                                 fNewGoombPosY = (int)fNewGoombPosY + 1;
                                 goombs[i].vel[1] = 0;
                             }
                        }

                        //PLAYER GOOMBA COLLISION DETECTION
                        if(fNewGoombPosX < fPlayerPosX + 1 && fNewGoombPosX + 1 > fPlayerPosX && fNewGoombPosY < fPlayerPosY + 1 && 1 + fNewGoombPosY > fPlayerPosY) {
                            //GET VECTOR FROM PLAYER TO GOOMBA
                            float PGx = fNewGoombPosX - fPlayerPosX;
                            float PGy = fNewGoombPosY - fPlayerPosY;
                            PGx = PGx / sqrt(pow(PGx, 2) + pow(PGy, 2));
                            PGy = PGy / sqrt(pow(PGx, 2) + pow(PGy, 2));
                            int best = -1;
                            float dirs[8]= {0.0, 1.0, 1.0, 0.0, 0.0, -1.0, -1.0, 0.0};
                            float max = 0.0;
                            for(int i = 0; i < 8; i+=2) {
                               float dot = PGx * dirs[i] + PGy * dirs[i + 1];
                               if(dot > max) {
                                   max = dot;
                                   best = i;
                               }
                            }
                            if(best == 4) { //LANDED ON GOOMBA
                                goombs[i].dir = 0x2;
                                fPlayerVelY = 50.0f;
                                //fNewGoombPosY = -fNewGoombPosY;
                                score++;
                            } else {
                               //GAME OVER!
                               gameOver = 1;
                               fPlayerVelY = 80.0;
                               fPlayerVelX = 0;
                            }
                        }

                        //GOOMBA TO GOOMBA COLLISION
                        for(int j = 0; j < numOfGoombas; j++) {
                            if(&goombs[i] != &goombs[j]) {
                                if(fNewGoombPosX < goombs[j].pos[0] + 1 && fNewGoombPosX + 1 > goombs[j].pos[0] && fNewGoombPosY < goombs[j].pos[1] + 1 && 1 + fNewGoombPosY > goombs[j].pos[1] && (goombs[i].dir != 0x8 && goombs[j].dir != 0x8)) {
                                    goombs[i].vel[0] = -goombs[i].vel[0];
                                    goombs[j].vel[0] = -goombs[j].vel[0];
                                }
                            }
                        }

                        goombs[i].pos[0] = fNewGoombPosX;
                        goombs[i].pos[1] = fNewGoombPosY;
                        if(frame % 2 == 0) {
                            goombs[i].dir ^= 0x1;
                        }
                    } else if((goombs[i].dir & 0xFF) == 0x2 || (goombs[i].dir & 0xFF) == 0x4 || (goombs[i].dir & 0xFF) == 0x8){
                        //STOP GOOMBA RENDER
                        goombs[i].dir = (goombs[i].dir == 0x8)? 0x8 : goombs[i].dir << 1;
                    }

                }
            }

            //SBLOCK ANIMATIONS
            for(int i = 0; i < activeSBlocks; i++) {
                //HANDLE GRAVITY FOR ACTIVE BLOCKS
                if(spriteBlocks.state == 1) {
                    spriteBlocks.vel[1] += -850.0f * fElapsedTime;
                    spriteBlocks.pos[1] = spriteBlocks.pos[1] + spriteBlocks.vel[1] * fElapsedTime;

                    if(spriteBlocks.pos[1] <= spriteBlocks.prev[1]) {
                        spriteBlocks.pos[1] = spriteBlocks.prev[1];
                        spriteBlocks.state = 0;
                        scene[(int)(spriteBlocks.prev[1]) * overworld1.width + (int)(spriteBlocks.prev[0])].type = 0x07;
                        activeSBlocks = 0;
                    }
                }
            }

            //MUSHROOM LOGIC
            if(activeMushroom) {
                float fNewShroomPosX = 0;
                float fNewShroomPosY = 0;

                if(!activeSBlocks){
                    currShroom.vel[1] += -850.0f * fElapsedTime;
                }

                fNewShroomPosX += currShroom.pos[0] + currShroom.vel[0] * fElapsedTime;
                fNewShroomPosY += currShroom.pos[1] + currShroom.vel[1] * fElapsedTime;

                //X VEL
                if(!activeSBlocks){
                    if(currShroom.vel[0] <= 0) {
                        block one = getBlock(scene, fNewShroomPosX + 0.0f, currShroom.pos[1] + 0.0f);
                        block two = getBlock(scene, fNewShroomPosX + 0.0f, currShroom.pos[1] + 0.9f);
                        if(checkNonCollisions(one.type, two.type)) {
                            fNewShroomPosX = (int)fNewShroomPosX + 1;
                            currShroom.vel[0] = -currShroom.vel[0];
                        }
                    } else {
                        block one = getBlock(scene, fNewShroomPosX + 1.0f, currShroom.pos[1] + 0.0f);
                        block two = getBlock(scene, fNewShroomPosX + 1.0f, currShroom.pos[1] + 0.9f);
                        if(checkNonCollisions(one.type, two.type)) {
                            fNewShroomPosX = (int)fNewShroomPosX;
                            currShroom.vel[0] = -currShroom.vel[0];
                        }
                    }

                    //Y VEL
                    if(currShroom.vel[1] >= 0) {
                        block one = getBlock(scene, fNewShroomPosX + 0.0f, fNewShroomPosY + 1.0f);
                        block two = getBlock(scene, fNewShroomPosX + 0.9f, fNewShroomPosY + 1.0f);
                        if(checkNonCollisions(one.type, two.type)) {
                            fNewShroomPosY = (int)fNewShroomPosY;
                            currShroom.vel[1] = 0;
                        }
                    } else {
                        block one = getBlock(scene, fNewShroomPosX + 0.0f, fNewShroomPosY);
                        block two = getBlock(scene, fNewShroomPosX + 0.9f, fNewShroomPosY + 0.0f);
                        if(checkNonCollisions(one.type, two.type)) {
                            fNewShroomPosY = (int)fNewShroomPosY + 1;
                            currShroom.vel[1] = 0;
                        }
                    }
                }

                if(fNewShroomPosX < fPlayerPosX + 1 && fNewShroomPosX + 1 > fPlayerPosX && fNewShroomPosY < fPlayerPosY + 1 && 1 + fNewShroomPosY > fPlayerPosY) {
                    score+=10;
                    activeMushroom = 0;
                }

                if(!activeSBlocks){
                    currShroom.pos[0] = fNewShroomPosX;
                }
                currShroom.pos[1] = fNewShroomPosY;
            }

            if(gameOver == 1 && musicFlip == 0)
            {
                animState = 4;
                musicFlip = 1;
                gameOverMusic = 1;
            } else if(musicFlip == 1) {
                animState = 4;
                gameTime++;
                if(gameTime == 20) {
                    NVIC_SystemReset();
                }
            }

            if(win == 1 && musicFlipWin == 0)
            {
                musicFlipWin = 1;
                flagWinMusic = 1;
                score += scoreTime;
                //Check and store highscore
                if (score > highscore)
                {
                    write_highscore("hs.txt", score);
                }

            } else if(musicFlipWin == 1) {
                gameTime++;
                if(gameTime == 35) {
                    NVIC_SystemReset();
                }
            }

            if(scoreTime <= 0 && gameOver == 0) {
                gameOver = 1;
                fPlayerVelY = 80.0;
                fPlayerVelX = 0;
            }

            sprintf(scoreString, "score %d", score);
            sprintf(time, "%d", scoreTime);
            render(scene, numOfObjects, fCameraPosX, fCameraPosY, fPlayerPosX, fPlayerPosY, &game_tileset, animState, nDirModX, goombs);
            frame++;
            if(frame % 5 == 0 && scoreTime > 0 && !gameOver) {
                scoreTime-=1;
            }
        }

        free(scene);
    }


