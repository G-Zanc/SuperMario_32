void init_spi1_slow(void);
void enable_sdcard(void);
void disable_sdcard(void);
void sdcard_io_high_speed(void);
void init_sdcard_io(void);
void draw_sprite(const char*, int, int , int , int , int);
int read_highscore(const char* file);
void write_highscore(const char* file, int score);
void clear_highscore(const char*);
