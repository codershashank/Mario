#include <stdio.h>
#include <allegro.h>
//#include "mappyal.h"
#include "mappyal.c"

#define MODE GFX_AUTODETECT_FULLSCREEN
#define WIDTH 640
#define HEIGHT 480
#define JUMPIT 1600
#define BAT1_BMP      0
#define CONTROLS_BMP  5  
#define CREDITS_BMP   6
#define NEWGUY_BMP    7
#define START_BMP     8
DATAFILE *data;
//define the sprite structure
typedef struct SPRITE
{
    int dir, alive;
    int x,y;
    int width,height;
    int xspeed,yspeed;
    int xdelay,ydelay;
    int xcount,ycount;
    int curframe,maxframe,animdir;
    int framecount,framedelay;
}SPRITE;

//declare the bitmaps and sprites
BITMAP *player_image[4];
SPRITE *player;
BITMAP *buffer;	
BITMAP *temp;
int level = 1;
int lifes = 3;
int score = 0;
int health = 100;
BITMAP *bat_img[2];
SPRITE *bat;
BITMAP *temp2;
BITMAP *strt;
BITMAP *controls;
BITMAP *credits;


BITMAP *test_img;
SPRITE *test;

int oldpx, oldpy;
int facing = 0;
int jump = JUMPIT;
//tile grabber
BITMAP *grabframe(BITMAP *source, 
                  int width, int height, 
                  int startx, int starty, 
                  int columns, int frame)
{
    BITMAP *temp = create_bitmap(width,height);
    int x = startx + (frame % columns) * width;
    int y = starty + (frame / columns) * height;
    blit(source,temp,x,y,0,0,width,height);
    return temp;
}

//collition control
int collided(int x, int y)
{
    BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->tl;
  
}

int collidedkill(int x, int y)
{
    BLKSTR *blockdata;
    blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
    return blockdata->bl;
}

int collidedtop(int x, int y)
{
    BLKSTR *blockdata;
    blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
    return blockdata->br;
}


int collidedobj(int x, int y)
{
    BLKSTR *blockdata;
    blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
    return blockdata->tr;
}
   
//enemy control
void scnleft(int x, int num, SPRITE *spr)
{
     if (x < num)
     spr->dir = 1;
}
void scnright(int x, int num, SPRITE *spr)
{
     if (x > num)
     spr->dir = 0;
}     

void updateEnemy(int numL, int numR, SPRITE *spr)
{if (++spr->framecount > spr->framedelay)
            {
                spr->framecount=0;
                if (++spr->curframe > spr->maxframe)
                    spr->curframe=0;
            }            
if (spr->dir ==1)
{spr->x +=1;
scnright(spr->x, numR, spr);}
else
{spr->x -=1;
scnleft(spr->x, numL, spr);}
}
//end of enemy control
int enemycoll(int x, int y, SPRITE *spr)
{extern int health;
    if (spr->y+spr->height > y && spr->y < y+80 && spr->x < x && spr->x > x-50)   
     {health -= 1;} 
}
  
void loadlevel()
{extern int level;
if (level == 1)
   { //load the map
	if (MapLoad("reallevel.fmp")) exit(0);	

    //create the double buffer
	buffer = create_bitmap (WIDTH, HEIGHT);
	clear(buffer);}
if (level == 2)
    {//load the map
	if (MapLoad("tst.fmp")) exit(0);	

    //create the double buffer
	buffer = create_bitmap (WIDTH, HEIGHT);
	clear(buffer);}     
}



void ClearCell (int x, int y)
{
	MapSetBlock (x/mapblockwidth, y/mapblockheight, 1);
}


void AddScore()
{
     score += 20;
if (score == 100)
     {lifes +=1;
     ++score;}
}



void keyinput()
{extern int jump;
if (key[KEY_RIGHT]) 
        { 
            facing = 1; 
            player->x+=2; 
            if (++player->framecount > player->framedelay)
            {
                player->framecount=0;
                if (++player->curframe > player->maxframe)
                    player->curframe=1;
            }
        }
        else if (key[KEY_LEFT]) 
        { 
            facing = 0; 
            player->x-=2;
            if (++player->framecount > player->framedelay)
            {
                player->framecount=0;
                if (++player->curframe > player->maxframe)
                    player->curframe=1;
            }
        }
        else player->curframe=0;

        //handle jumping
        if (jump==JUMPIT)
        {                  //gravity code
            if (!collided(player->x + player->width/2, 
                player->y + player->height + 5))
                jump = 0; 

		    if (key[KEY_SPACE]) 
                jump = 30;
            
        }
        else
        {
            player->y -= jump/3; 
            jump--; 
   if (collidedtop(player->x, player->y))
            player->y = oldpy;     
        }

		if (jump<0) 
        { 
            
            
            if (collided(player->x + player->width/2, 
                player->y + player->height))
			{ 
                jump = JUMPIT; 
                while (collided(player->x + player->width/2, 
                    player->y + player->height))
                    player->y -= 2; 
            } 
        }     
}


void Start()
{strt = (BITMAP *)data[START_BMP].dat;
 controls = (BITMAP *)data[CONTROLS_BMP].dat;
 credits = (BITMAP *)data[CREDITS_BMP].dat;

   while (! key[KEY_ENTER])
     {
     blit(strt, screen,0,0,0,0,SCREEN_W,SCREEN_H);
       if (key[KEY_TAB])
          {
          while (!key[KEY_ESC])
          {blit(controls, screen,0,0,0,0,SCREEN_W,SCREEN_H);
                         rest(1);}}
       rest(1);
       if (key[KEY_SPACE])
          {
          while (!key[KEY_ESC])
          {blit(credits, screen,0,0,0,0,SCREEN_W,SCREEN_H);
                        rest(1);}}
       rest(1);
       if (key[KEY_Q])
       {return 0;}
     rest(1);
     }
destroy_bitmap(strt);
destroy_bitmap(controls);
destroy_bitmap(credits);     
}
int main (void)
{
// Golobal Integers
    extern int level;
    extern int lifes;
    extern int score;
    extern int health;
// Local Integers
   register int mapxoff, mapyoff;
   extern int oldpy, oldpx;
   extern int facing;
   extern int jump;
   int n, m;
  // DATAFILE *data;
//initilization    
	allegro_init();	
	install_timer();
	install_keyboard();
    set_color_depth(32);
	set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);

//load data
data = load_datafile("test.dat");
    temp = (BITMAP *)data[NEWGUY_BMP].dat;
    for (n=0; n<4; n++)
    player_image[n] = grabframe(temp,43,77,0,0,4,n);
    destroy_bitmap(temp);
    temp2 = (BITMAP *)data[BAT1_BMP].dat;
    for (m=0; m<2; m++)
    bat_img[m] = grabframe(temp2,87,72,0,0,2,m);
    destroy_bitmap(temp2);

    test_img = load_bitmap("test.bmp", NULL);
gameover:
Start();
first:
//initilize Player
    player = malloc(sizeof(SPRITE));
    player->x = 80;
    player->y = 200;
    player->curframe=0;
    player->framecount=0;
    player->framedelay=11;
    player->maxframe=3;
    player->width=player_image[0]->w;
    player->height=player_image[0]->h;
//initilize Bat
    bat = malloc(sizeof(SPRITE));
    bat->x = 170;
    bat->y = 100;
    bat->curframe=0;
    bat->framecount=0;
    bat->framedelay=11;
    bat->maxframe=1;
    bat->width=bat_img[0]->w;
    bat->height=bat_img[0]->h;
    bat->dir = 0;
    bat->xspeed = 0;
//ini test
test = malloc(sizeof(SPRITE));
    test->x = 270;
    test->y = 400;
    test->curframe=0;
    test->framecount=0;
    test->framedelay=11;
    test->maxframe=1;
    test->width=bat_img[0]->w;
    test->height=bat_img[0]->h;
    test->dir = 0;
    test->xspeed = 0;

loadlevel();

    //main loop
	while (!key[KEY_ESC])
	{
      oldpy = player->y; 
      oldpx = player->x;

if (player->x > 3000)
        {level = 2;
		goto first;}

if (lifes == 0)
{lifes = 3;
          goto gameover;}

if (health < 1)
{lifes -=1;
  health =100;
  goto first;}

  updateEnemy(150, 300, test);
  updateEnemy(150, 300, bat);       
  enemycoll(player->x, player->y, bat);       
  keyinput();
//collition control
	if (!facing) 
        { 
            if (collided(player->x, player->y + player->height)) 
                player->x = oldpx; 
                if (collidedobj(player->x, player->y + player->height))
                   {ClearCell(player->x, player->y + player->height); 
                   AddScore();}
                      if (collidedkill(player->x+9, player->y + player->height))
                         {health -= 25;}        
        }
		else 
        { 
            if (collided(player->x + player->width, player->y + player->height)) 
                player->x = oldpx; 
                if (collidedobj(player->x + player->width, player->y + player->height))
                   {ClearCell(player->x + player->width, player->y + player->height);
                   AddScore();}
                      if (collidedkill(player->x + player->width-15, player->y + player->height))
                         {health -= 25;}        
        }
		
        //update the map scroll position
		mapxoff = player->x + player->width/2 - WIDTH/2 + 10;
		mapyoff = player->y + player->height/2 - HEIGHT/2 + 10;
        //avoid moving beyond the map edge
		if (mapxoff < 0) mapxoff = 0;
		if (mapxoff > (mapwidth * mapblockwidth - WIDTH))
            mapxoff = mapwidth * mapblockwidth - WIDTH;
		if (mapyoff < 0) 
            mapyoff = 0;
		if (mapyoff > (mapheight * mapblockheight - HEIGHT)) 
            mapyoff = mapheight * mapblockheight - HEIGHT;

        //draw the background tiles
		MapDrawBG(buffer, mapxoff, mapyoff, 0, 0, WIDTH-1, HEIGHT-1);
        //draw foreground tiles
		MapDrawFG(buffer, mapxoff, mapyoff, 0, 0, WIDTH-1, HEIGHT-1, 0);
        //draw the player's sprite
		if (facing) 
            draw_sprite(buffer, player_image[player->curframe], 
                (player->x-mapxoff), (player->y-mapyoff+1));
		else 
            draw_sprite_h_flip(buffer, player_image[player->curframe], 
                (player->x-mapxoff), (player->y-mapyoff));

        //blit the double buffer 
		vsync();
        acquire_screen();
		if (bat->dir == 1)
           draw_sprite(buffer, bat_img[bat->curframe],(bat->x-mapxoff),(bat->y-mapyoff));
        else
           draw_sprite_h_flip(buffer, bat_img[bat->curframe],(bat->x-mapxoff),(bat->y-mapyoff)); 
        
        draw_sprite(buffer, test_img,(test->x-mapxoff),(test->y-mapyoff));
        textprintf(buffer,font,0,0,9999999,"lifes = %d  Score = %d  Health = %d",lifes,score,health);
        blit(buffer, screen, 0, 0, 0, 0, WIDTH-1, HEIGHT-1);
       	release_screen();

	} //End of game loop




    for (n=0; n<4; n++)
        destroy_bitmap(player_image[n]);
    for (m=0; m<2; m++)
        destroy_bitmap(bat_img[m]);

destroy_bitmap(test_img);

    free(player);
	free(bat);
    free(test);
    destroy_bitmap(buffer);
	MapFreeMem ();
	MapFreeMem ();
    unload_datafile(data);
    allegro_exit();
	return 0;
}

END_OF_MAIN();

