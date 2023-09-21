/*
MINESWEEPER (SAPER) FOR CP/M-80 v0.3
Originally written in Pascal by Reshetnikov C.I.
Converted for Z88DK-C (originally HITECH-C) by Eric B.

Built using z88dk: "zcc +cpm -clib 8080 mines.c -o mines.com"
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <cpm.h>

#define ESC 27
#define CTRL_X 24
#define CTRL_C 3
#define RET 13

static unsigned char x,y,fx,fy,c;
static unsigned int i,fo,fm,tmines,time,tdiv,tdiv_max;
static unsigned char f[22][42];
static char tmp[12];
static FILE* fp;

/* 
Non-blocking getch(), used for running the timer while waiting for input
*/
static char getch_nobl() {
	return bdos(CPM_DCIO, 0xFF);
}

/* 
Move cursor to x,y (ANSI terminal)
*/
static void goto_xy(char x, char y) {
	/*sprintf(tmp, "%c[%d;%dH", ESC, y+1, x+1);
    for (i = 0; i < strlen(tmp); ++i) { putch(tmp[i]); }*/
	printf("%c[%d;%dH", ESC, y+1, x+1);
}

/*
Clear screen (switched to using just newlines)
*/
static void clearscr() { 
    /* printf("%c[2J", ESC); */
	for (i = 0; i < 40; ++i) { printf("\n"); }
}

/* 
Invert text (ANSI terminal)
*/
static void invert_t() {
	printf("%c[7m", ESC);
}

/* 
Normal text (ANSI terminal)
*/
static void normal_t() {
	printf("%c[m", ESC);
}

/*
Somewhat hacky random seed using non blocking input
*/
static void seed_random() {
	i = c = 0;
	while (!(c=getch_nobl())) {
		++i;
	}	
	srand(i*c);
}

static void win() {
	goto_xy(0,22);
	printf("\nCongratulations, you win!  ");
	getch();
	exit(1);
}

static void lose() {
    for (y=1;y<=fy;++y) {
        for (x=1;x<=fx;++x) {
			if ((f[y][x] & (128+64)) == 128) {
				goto_xy(x*2,y+1);
				putch('*');
			}
        }
    }
    goto_xy(0,22);
    printf("\nAlas, there was a mine...  "); 
	getch();
	exit(1);
}

static void open_cell(unsigned char cx, unsigned char cy) {
	if ((cx < 1) || (cy < 1) || (cx > fx) || (cy > fy) || ((f[cy][cx] & 64) != 0)) { return; }
	if ((f[cy][cx] & 128) != 0) { lose(); }
	goto_xy(cx*2,cy+1);
	if (f[cy][cx] == 0) {
		putch(' ');
		f[cy][cx] = 255;
		--fo;
		open_cell(cx-1, cy); open_cell(cx-1, cy-1); open_cell(cx-1, cy+1);
		open_cell(cx+1, cy); open_cell(cx+1, cy-1); open_cell(cx+1, cy+1);
		open_cell(cx, cy-1); open_cell(cx, cy+1);
	} else {
		putch('0'+f[cy][cx]);
		f[cy][cx] = 255;
		--fo;
	}
}

/*
Print the time, only doing necessary digits to be efficient
*/
static void print_time() {
	if (time%100 == 0) {
		goto_xy(28,24);
		sprintf(tmp, "%d", time);
		putch(tmp[0]);
		putch(tmp[1]);
		putch(tmp[2]);
	}
	else if (time%10 == 0) {
		goto_xy(29,24);
		sprintf(tmp, "%d", time%100);
		putch(tmp[0]);
		putch(tmp[1]);
	}
	else {
		goto_xy(30,24);
		sprintf(tmp, "%d", time%10);
		putch(tmp[0]);
	}
}

/*
Print cell and mine count, using putch to avoid held keys interfering
May not be required anymore in z88dk, need to test
*/
static void print_mines_cells() {
	goto_xy(7,24);
	sprintf(tmp, "%03d", fm);
	putch(tmp[0]);
	putch(tmp[1]);
	putch(tmp[2]);
	goto_xy(18,24);
	sprintf(tmp, "%03d", fo);
	putch(tmp[0]);
	putch(tmp[1]);
	putch(tmp[2]);
}

int main() {
	c = 0;
    fx = fm = 20;
    fy = 10;
	tdiv_max = 1800; 
    memset(f, 0, 40*24);

    clearscr();
	invert_t();
	printf("          MINESWEEPER (SAPER) FOR CP/M-80           \n\n");
	normal_t();
    printf("      Original programming by Reshetnikov C.I.      \n");
    printf("             Converted to C by Eric B.              \n");
    printf("        Now with timer and a better random!         \n\n");
	invert_t();
	printf("                   PRESS ANY KEY                    \n");
	normal_t();
	
	seed_random();
	while ((c != 'N')) {
		printf("\nWhat do you want to do?\n (N) New game, (H) Hiscores, (T) Calibrate timer, (X) Quit");
		c = toupper(getch());
		if (c == 'T') {
			printf("\n\n Enter CPU speed to nearest MHz (1-135): ");
			gets(tmp);
			tdiv_max = atoi(tmp); 
			if (tdiv_max > 0) {
				tdiv_max *= 900;
			} else { tdiv_max = 900; }
		}
		else if (c == 'H') {
			fp = fopen("MINES.DAT", "r");
			if (!fp) {
				printf("\n\n Can't open MINES.DAT. Are there no hiscores?");
			}
			else {
				i = 0;
				printf("\nNAME\t\t\tW/H/M\t\t\tTIME\n");
				while(fscanf(fp, "%s", tmp) != EOF) {
					if (i % 3 == 0) { putch('\n'); }
					printf("%s\t\t\t", tmp);
					++i;
				}
			}
			putch('\n');
		}
		else if ((c == 'X') || (c == CTRL_C) || (c == CTRL_X)) { return 0; }
	}
    
    printf("\nLevel? 1 = Beginner, 2 = Amateur, 3 = Pro, 4 = Custom: ");
    c = getch();

    if (c == '1') {
        fx = 9; fy = 9; fm = 10;
    } else if (c == '2') {
        fx = 16; fy = 16; fm = 40;
    } else if (c == '3') {
        fx = 30; fy = 16; fm = 99;
    } else {
        printf("\n\n Enter width: ");
		gets(tmp); fx = atoi(tmp);
        printf("\n Enter height: ");
		gets(tmp); fy = atoi(tmp);
        printf("\n Enter number of mines: ");
		gets(tmp); fm = atoi(tmp);
        
        if (fx > 39) { fx = 39; }
        else if (fx < 2) { fx = 2; }
        if (fy > 20) { fy = 20; }
        else if (fy < 2) { fy = 2; }
        if (fm > (fx*fy-1)) { fm = (fx*fy-1); }
        else if (fm < 1) { fm = 1; }
    }
	tmines = fm;
	printf("\n Generating mines...");
    i = 0;
    while (i < fm) {
        x = (rand() % fx)+1;
        y = (rand() % fy)+1;
        if (f[y][x] < 128) {
            f[y][x] += 128;
            f[y][x+1] += 1;
            f[y][x-1] += 1;
            f[y+1][x+1] += 1;
            f[y-1][x+1] += 1;
            f[y+1][x-1] += 1;
            f[y-1][x-1] += 1;
            f[y+1][x] += 1;
            f[y-1][x] += 1;
            ++i;
        }
    }
	
	time = tdiv = 0;
    clearscr();
	goto_xy(0,0);
    printf(" Minesweeper for CP/M-80, v0.3 (c) Eric B., Reshetnikov C.I.\n");  
	invert_t();
	printf("%*c", fx*2+1, ' ');
	for (i=0;i<fy;++i) {
		invert_t();
		printf(" \n ");
		normal_t();
		for (c = 0; c < fx; ++c) { printf(" ."); }
	}
	invert_t();
	printf(" \n ");
	printf("%*c", fx*2+1, ' ');
	normal_t();
	
	fo = fx*fy;
	goto_xy(0,24);
	printf("Mines:     Cells:     Time:     ");
	print_mines_cells();

	/* Print controls */
	goto_xy(36,24);
	invert_t();	printf("W A S D"); normal_t();
	printf(" Move ");
	invert_t();	printf("RET"); normal_t();
	printf(" Toggle Flag ");
	invert_t();	printf("SP"); normal_t();
	printf(" Open ");
	invert_t();	printf("^X"); normal_t();
	printf(" Quit");
	
	c = 0;
	x = fx/2;
	y = fy/2;
	goto_xy(x*2,y+1);
	
	while ((c != CTRL_X) && (c != CTRL_C)) {
		c = getch_nobl();
		
		if (c) {
			c = toupper(c);
			if ((c == 'W') && (y > 1)) { y -= 1; }
			else if ((c == 'S') && (y < fy)) { y += 1; }
			else if ((c == 'A') && (x > 1)) { x -= 1; }
			else if ((c == 'D') && (x < fx)) { x += 1; }
			else if ((c == RET)) {
				if ((fm>0) || ((f[y][x] & 64)!=0)) {
					f[y][x] ^= 64;
					if ((f[y][x] & 64)!=0) {
						fm -= 1;
						fo -= 1;
						putch('F');
					} else {
						fm += 1;
						fo += 1;
						putch('.');
					}
					print_mines_cells();
				} else {
					putch('?');
				}
			}
			else if (c == ' ') {			
				open_cell(x,y);
				print_mines_cells();
			}
			goto_xy(x*2,y+1);
		}
		if ((fm == 0) && (fo == 0)) { win(); }
		if ((time < 999) && (tdiv > tdiv_max)) {
			tdiv = 0;
			++time;
			print_time();
			goto_xy(x*2,y+1);
		}
		++tdiv;
	}
	normal_t();
	goto_xy(0,22);
	printf("\nMaybe next time...         ");
    return 0;
}