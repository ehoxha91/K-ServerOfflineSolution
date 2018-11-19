/*	
   gcc -o wnd TwoRobots_001.c -lX11 -lm -L/usr/X11R6/lib	--compile line	
   
   Student:    Ejup Hoxha 
   Semester:   Fall 2018 - 
   Class:      Advanced Algorithms - Dr. Peter Brass
   University: City University of New York - Groove School of Engineering

*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "HelperFile.h"			/* Few linked lists used primarly for drawings and logging. */

//#define DEBUG				/* Forget that this egzists*/
//#define DRAWNODES			/* If we want to draw nodes of our graph. */
//#define PRINT				/* This one is not needed neither. */

#define XBORDER 700
#define YBORDER 485

Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Two Robots - Offline Optimization - K=3 Server Problem";
char *icon_name_string = "Icon For Window";

KeySym keyR;				/* a dealie-bob to handle KeyPress Events */	
char pb_txt[255];			/* a char buffer for KeyPress Events */

XEvent report;

/* ################################### Colors ############################################################# */
GC gc, red, green, white, yellow, white_2, blue, blue_2, orange_2, orange, black, wallcolor;

XGCValues gc_values, gc_orange_v, gc_orange_v2, gc_red_v, gc_blue_v, gc_blue_v2, gc_green_v,
	  gc_white_v, wallcolor_v, gc_yellow_v, gc_white_v2, blue_sv, gc_black_v;
Colormap color_map;
XColor tmp_color1, tmp_color2;
unsigned long valuemask = 0;
/*#########################################################################################################*/

/* Robot Algorithm Functions */
#define ROBOT_HEIGHT 12
#define ROBOT_WIDTH 12
#define R1_s_x 5
#define R1_s_y 5
#define R2_s_x 700
#define R2_s_y 5
#define R3_s_x 350
#define R3_s_y 495

struct Robot robot1 = {1, R1_s_x, R1_s_y, ROBOT_HEIGHT, ROBOT_WIDTH, 0.0, NULL};
struct Robot robot2 = {2, R2_s_x, R2_s_y, ROBOT_HEIGHT, ROBOT_WIDTH, 0.0, NULL};
struct Robot robot2 = {3, R2_s_x, R2_s_y, ROBOT_HEIGHT, ROBOT_WIDTH, 0.0, NULL};

void Robot_work(struct Pxy _reqpointxy);	/* Decision maker. Algorithm which decides which robot should move. */
void about_info();				/* Re/Draw info about our program. */

/* Drawing Functions */
void GetColors();	
void Re_Draw();
int drw_rp = 0;
void draw_line(GC _gc, int _x1, int _y1, int _x2, int _y2);
void draw_segment(GC _gc, int __id, int _isEdge);
void draw_request(GC _color, int _posx, int _posy, int _savetolist);
void draw_robot(GC _color, int _posx, int _posy, int _height, int _width);
void drawstring(GC _scolor, int sposx, int sposy, char *text);
void drawint(GC _scolor, int sposx, int sposy, int);
void drawdouble(GC _scolor, int sposx, int sposy, double inttodraw);
void ClearArea(int _psx, int _psy, int _wclear, int _hclear, int _riseExposeEvent); 

int main(int argc)
{
	/* Open Display: Try to connect to X server. */
	display_ptr = XOpenDisplay(display_name);
	if(display_ptr == NULL)
	{ 
	  printf("Could not open the window"); exit(-1);}
	  printf("Connected to X server %s\n", XDisplayName(display_name));
	  screen_num = DefaultScreen(display_ptr);
      	  screen_ptr = DefaultScreenOfDisplay(display_ptr);
	  color_map = XDefaultColormap(display_ptr, screen_num);
	  display_width = DisplayWidth(display_ptr, screen_num);
	  display_height = DisplayHeight(display_ptr, screen_num);

	/* Create the window. */
	border_width = 10;
	win_x = 0;
	win_y = 0;
	win_width = 900;
	win_height = 500;
	
	win = XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num),
				  win_x, win_y, win_width, win_height, border_width,
				  BlackPixel(display_ptr, screen_num),
				  BlackPixel(display_ptr, screen_num));
	
	size_hints = XAllocSizeHints();
	wm_hints = XAllocWMHints();
	class_hints = XAllocClassHint();
	
	if(size_hints == NULL || wm_hints == NULL || class_hints == NULL)
	{
		printf("Error allocating memory for hints.\n"); exit(-1);
	}	

	size_hints -> flags = PPosition | PSize | PMinSize  ;
  	size_hints -> min_width = 60;
  	size_hints -> min_height = 60;
	
	XStringListToTextProperty( &win_name_string,1,&win_name);
	XStringListToTextProperty( &icon_name_string,1,&icon_name);

	wm_hints -> flags = StateHint | InputHint ;
	wm_hints -> initial_state = NormalState;
	wm_hints -> input = False;	
	
	class_hints -> res_name = "x_use_example";
 	class_hints -> res_class = "examples";

	XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,
                          size_hints, wm_hints, class_hints );	

	XSelectInput( display_ptr, win, ExposureMask | StructureNotifyMask | ButtonPressMask | KeyPressMask);
	XMapWindow( display_ptr, win );

	XFlush(display_ptr);
	GetColors();	

	while(1)
	{ 
	      XNextEvent(display_ptr, &report );
	      switch( report.type )
	      {
			case Expose:
				Re_Draw(); 
				about_info();
				break;

			case ConfigureNotify:
				win_width = report.xconfigure.width;
				win_height = report.xconfigure.height;
				break;
			case ButtonPress:
			{  
				struct Pxy _pxy = {report.xbutton.x, report.xbutton.y}; 
				if (report.xbutton.button == Button1 )
				{	
					if(_pxy.x < XBORDER && _pxy.y < YBORDER )
					{
						draw_request(green, _pxy.x, _pxy.y, 0);	/* Draw request point. */
						drw_rp =1;
						Re_Draw();
					}
				}
				else exit(-1);
			}
				break;
			case KeyPress:
			{	
			     XLookupString(&report.xkey,pb_txt,255,&keyR,0);
			     if(pb_txt[0] == 'e') { ClearArea(0, 0, win_width, win_height, 1);}
			}
				break;		
			default: 
			break;
		  }

	}
	exit(0);
	return 0;
}

/* ****************************************** */
/* Drawing functions of segments, robots etc. */
/* ****************************************** */

void redraw()
{ 
	/* Sometimes we may want to draw edges of the possible roads */
	#ifdef DRAWEDGES
	 for(int l55 =0; l55<count_edges;l55++) 
		draw_segment(white, l55,1);
	#endif

	/* Re-draw request points */
	if(drw_rp ==1)
	{ 
	  for(int l2 = 0; l2<req_id; l2++)
	  { 
	    struct req_point* tmp_rq = GetPoint(l2); 
            draw_request(green,tmp_rq->_req_x, tmp_rq->_req_y, 1);
          }
	}

	/* Re-draw robot's actual positions. */
	draw_robot(blue, robot1.r_px, robot1.r_py, robot1.r_height, robot1.r_width);
	draw_robot(orange, robot2.r_px, robot2.r_py, robot2.r_height, robot2.r_width);
}

/* A draw a simple line. */
void draw_line(GC _gc, int _x1, int _y1, int _x2, int _y2)
{
	XDrawLine(display_ptr, win, _gc, _x1, _y1, _x2, _y2);
}

/* Draw a segment/edge. */
void draw_segment(GC _gc, int __id)
{
	struct Seg* temp =GetSegById(__id);
	if(temp!=NULL) 
	draw_line(_gc, temp->_x1, temp->_y1, temp->_x2, temp->_y2);
}

/* Draw the position of the robot. */
void draw_robot(GC _color, int _posx, int _posy, int _height, int _width)
{
     XFillArc(display_ptr, win, _color, _posx-_width/2, _posy-_height/2, _height, _width,0,360*64);
}

void draw_request(GC _color, int _posx, int _posy, int _isReDraw)
{   
    if(_isReDraw == 0) 
    {  AddRequestPoint(_posx, _posy); }
    XFillArc( display_ptr, win, _color, _posx-7.5, _posy-7.5, 15, 15, 0, 360*64);
}

void drawstring(GC _scolor, int sposx, int sposy, char *text)
{
	XDrawString(display_ptr, win, _scolor, sposx, sposy, text, strlen(text));
}

void drawint(GC _scolor, int sposx, int sposy, int inttodraw)
{
	char outtxt[50];
	sprintf(outtxt,"%d", inttodraw);
	char *tx = outtxt;
	drawstring(_scolor, sposx, sposy, tx);
}

void drawdouble(GC _scolor, int sposx, int sposy, double inttodraw)
{
	char outtxt[50];
	sprintf(outtxt,"%.2f", inttodraw);
	char *tx = outtxt;
	drawstring(_scolor, sposx, sposy, tx);
}

/* Clear a certain area of the window. */
void ClearArea(int _psx, int _psy, int _wclear, int _hclear, int _riseExposeEvent)
{
	XClearArea(display_ptr, win, _psx, _psy, _wclear, _hclear, _riseExposeEvent);
}

void about_info()
{
	XDrawRectangle(display_ptr, win, white, 705, 0, 190, 80);
	drawstring(blue, 708, 15, "ROBOT 1 Distance: ");
	drawstring(orange_shadow, 708, 30, "ROBOT 2 Distance: ");
	XDrawRectangle(display_ptr, win, white, 0, 0, 700, 485);
	drawstring(blue, 708, 45, "Last Travel Cost: ");
	drawstring(orange_shadow, 708, 60, "Last Travel Cost: ");
	drawstring(green, 708, 75, "Travel difference: ");

	XDrawRectangle(display_ptr, win, wallcolor, 705, 85, 190, 80);
	drawstring(white, 708, 100, "WEIGHTS OF PREDICTION: ");
	drawstring(white, 708, 115, "Travel Diff. Weight    : ");
	drawstring(white, 708, 130, "Distance Between Weight: ");
	drawstring(white, 708, 145, "High Border D. Weight  : ");
	drawstring(white, 708, 160, "Low Border D. Weight   : ");
	drawdouble(green,860,115, disdiffWeight);
	drawdouble(green,860,130, distWeight);
	drawint(green,860,145, (int)simdistHighBorder);
	drawint(green,860,160, (int)simdisLowBorder);

	
	XDrawRectangle(display_ptr,win,white,705,405,190,80);
	drawstring(white, 708, 420, "Graph Search - Dijkstra");
	drawstring(white, 708, 435, "2 Server Online Optimization");
	drawstring(orange, 708, 450, "Advanced Algorithms");
	drawstring(orange, 708, 465, "Lecturer: Peter Brass");
	drawstring(green, 708, 480, "Author: Ejup Hoxha");
}

/* Initialize colors which we will mostly use. */
void GetColors(){
	/* To be able to draw on this window we need to create graphics context. */
	gc = XCreateGC(display_ptr, win, valuemask, &gc_values);
	XSetForeground(display_ptr, gc, BlackPixel(display_ptr, screen_num));
	XSetLineAttributes(display_ptr, gc, 4, LineSolid, CapRound, JoinRound);

	green = XCreateGC(display_ptr, win, valuemask, &gc_green_v);
	XSetLineAttributes(display_ptr, green, 2, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "SpringGreen", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color Spring Green\n"); 
	    exit(-1);
	} 
  	else
   	    XSetForeground( display_ptr, green, tmp_color1.pixel );

	red = XCreateGC(display_ptr, win, valuemask, &gc_red_v);
	XSetLineAttributes(display_ptr, red, 1, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "red", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color red\n"); 
	    exit(-1);
	} 
  	else
   	    XSetForeground( display_ptr, red, tmp_color1.pixel );

	blue = XCreateGC(display_ptr, win, valuemask, &gc_blue_v);
	XSetLineAttributes(display_ptr, blue, 1, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "DeepSkyBlue", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color DeepSkyBlue\n"); 
	    exit(-1);
	}
  	else
	    XSetForeground( display_ptr, blue, tmp_color1.pixel );

	black = XCreateGC(display_ptr, win, valuemask, &gc_black_v);
	XSetLineAttributes(display_ptr, black, 1, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "Blue", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color Black\n"); 
	    exit(-1);
	}
  	else
	    XSetForeground( display_ptr, black, tmp_color1.pixel );

	orange = XCreateGC(display_ptr, win, valuemask, &gc_orange_v);
	XSetLineAttributes(display_ptr, orange, 1, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "OrangeRed", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color orange\n"); 
	    exit(-1);
	} 
  	else
	    XSetForeground( display_ptr, orange, tmp_color1.pixel );

	white = XCreateGC(display_ptr, win, valuemask, &gc_white_v);
	XSetLineAttributes(display_ptr, white, 1, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "white", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color white\n"); 
	    exit(-1);
	} 
  	else
	    XSetForeground( display_ptr, white, tmp_color1.pixel );

	yellow = XCreateGC(display_ptr, win, valuemask, &gc_yellow_v);
	XSetLineAttributes(display_ptr, yellow, 1, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "Yellow", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color yellow\n"); 
	    exit(-1);
	} 
  	else
	    XSetForeground( display_ptr, yellow, tmp_color1.pixel );

	blue_2 = XCreateGC(display_ptr, win, valuemask, &gc_blue_v2);
	XSetLineAttributes(display_ptr, blue_2, 2, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "DeepSkyBlue", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color DeepSkyBlue\n"); 
	    exit(-1);
	}
  	else
	    XSetForeground( display_ptr, blue_2, tmp_color1.pixel );

	orange_2 = XCreateGC(display_ptr, win, valuemask, &gc_orange_v2);
	XSetLineAttributes(display_ptr, orange_2, 2, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "OrangeRed", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color orange\n"); 
	    exit(-1);
	} 
  	else
	    XSetForeground( display_ptr, orange_2, tmp_color1.pixel );

	white_2 = XCreateGC(display_ptr, win, valuemask, &gc_white_v2);
	XSetLineAttributes(display_ptr, white_2, 3, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "white", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color white\n"); 
	    exit(-1);
	} 
  	else
	    XSetForeground( display_ptr, white_2, tmp_color1.pixel );

	wallcolor = XCreateGC(display_ptr, win, valuemask, &wallcolor_v);
	XSetLineAttributes(display_ptr, wallcolor, 2, LineSolid,CapRound, JoinRound);
	if( XAllocNamedColor( display_ptr, color_map, "DeepPink", &tmp_color1, &tmp_color2 ) == 0 )
    	{   
            printf("failed to get color of wall\n"); 
	    exit(-1);
	} 
  	else
	    XSetForeground( display_ptr, wallcolor, tmp_color1.pixel );
}
