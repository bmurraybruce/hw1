//Brandon Murray-Bruce
//cs335 Spring 2015 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

extern "C" {
#include "fonts.h"
} 

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 100000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
    float x, y, z;
};

struct Shape {
    float width, height;
    float radius;
    Vec center;
};

struct Particle {
    Shape s;
    Vec velocity;
};

struct Game {
    Shape box[5];
    Shape circle;
    Particle particle[MAX_PARTICLES];
    int n;
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);
void makeParticle(Game * game, int, int);


int main(void)
{
    int done=0;
    srand(time(NULL));
    initXWindows();
    init_opengl();
    //declare game object
    Game game;
    game.n=0;

    //declare a box shape

    for (int i = 0; i < 5; i++) {
	game.box[i].width = 70 + i*5;
	game.box[i].height = 10;
	game.box[i].center.x = 120 + (i*80);
	game.box[i].center.y = 500 - (i*70);
    }

    game.circle.center.x = 700;
    game.circle.center.y = 0;
    game.circle.radius = 200;

    //start animation
    while(!done) {
	while(XPending(dpy)) {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    //check_mouse(&e, &game);
	    done = check_keys(&e, &game);
	}
	makeParticle(&game,35,550);
	movement(&game);
	render(&game);
	glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    return 0;
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void) {
    //do not change
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void initXWindows(void) {
    //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
	std::cout << "\n\tcannot connect to X server\n" << std::endl;
	exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if(vi == NULL) {
	std::cout << "\n\tno appropriate visual found\n" << std::endl;
	exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
	ButtonPress | ButtonReleaseMask |
	PointerMotionMask |
	StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
	    InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);

    //Allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();
}

#define rnd() (float)rand() / (float)RAND_MAX

void makeParticle(Game *game, int x, int y) {
    if (game->n >= MAX_PARTICLES)
	return;
    std::cout << "makeParticle() " << x << " " << y << std::endl;
    //position of particle
    Particle *p = &game->particle[game->n];
    p->s.center.x = x;
    p->s.center.y = y;
    p->velocity.y = rnd() - 0.5;//1 - rnd()*0.6;
    p->velocity.x = 1 + rnd()*0.05; //rnd() - 0.1;
    game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
    static int savex = 0;
    static int savey = 0;
    static int n = 0;

    if (e->type == ButtonRelease) {
	return;
    }
    if (e->type == ButtonPress) {
	if (e->xbutton.button==1) {
	    //Left button was pressed
	    //int y = WINDOW_HEIGHT - e->xbutton.y;
	    makeParticle(game, 35, 550);
	    //makeParticle(game, e->xbutton.x, y);
	    return;
	}
	if (e->xbutton.button==3) {
	    //Right button was pressed
	    return;
	}

    }

    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
	savex = e->xbutton.x;
	savey = e->xbutton.y;
	if (++n < 10)
	    return;
	//int y = WINDOW_HEIGHT - e->xbutton.y;
	makeParticle(game, 35, 550);
	//makeParticle(game, e->xbutton.x, y);
    }

}

int check_keys(XEvent *e, Game *game)
{
    //Was there input from the keyboard?
    if (e->type == KeyPress) {
	int key = XLookupKeysym(&e->xkey, 0);
	if (key == XK_Escape) {
	    return 1;
	}
	//You may check other keys here.

    }
    return 0;
}

void movement(Game *game)
{
    Particle *p;

    if (game->n <= 0)
	return;

    for (int i = 0; i < game->n; i++)
    {
	p = &game->particle[i];
	p->s.center.x += p->velocity.x;
	p->s.center.y += p->velocity.y;

	//gravity
	p->velocity.y -= GRAVITY;

	//check for collision with shapes...
	//Shape *s;

	for (int k = 0; k < 5; k++) {
	    Shape *s = &game->box[k];
	    if (p->s.center.y >= s->center.y - (s->height) &&
		    p->s.center.y <= s->center.y + (s->height) &&
		    p->s.center.x >= s->center.x - (s->width) &&
		    p->s.center.x <= s->center.x + (s->width)) {
		p->velocity.y *= -0.35;
		p->s.center.y = s->center.y + s->height + 0.02;
		p->velocity.x += 0.02;
	    }
	}

	float r0, r1, rad;
	r0 = p->s.center.x - game->circle.center.x;
	r1 = p->s.center.y - game->circle.center.y;
	rad = sqrt(r0*r0 + r1*r1);
	if (rad <= game->circle.radius) {
	    r0 /= rad;
	    r1 /= rad;
	    r0 *= game->circle.radius * 1.01;
	    r1 *= game->circle.radius * 1.01;
	    p->s.center.x = game->circle.center.x + r0;
	    p->s.center.y = game->circle.center.y + r1;
	    p->velocity.x += r0 * 0.003;
	    p->velocity.y += r1 * 0.005;
	}



	//check for off-screen
	if (p->s.center.y < 0.0) {
	    std::cout << "off screen" << std::endl;

	    game->particle[i] = game->particle[game->n-1];
	    game->n--;
	}
    }
}

void render(Game *game)
{
    Rect r[6];
    float w, h;
    glClear(GL_COLOR_BUFFER_BIT);
    //Draw shapes...

    //Draw circle

    static int first = 1;
    static int vertices[60][2];
    static int x = 60;
    glColor3ub(50,0,0);
    if (first) {
	float angle = 0.0;
	float inc = (3.14159 * 2.0) / (float) x;

	for (int i = 0; i < x; i++) {
	    vertices[i][0] = cos(angle) * game->circle.radius + game->circle.center.x;
	    vertices[i][1] = sin(angle) * game->circle.radius + game->circle.center.y;
	    angle += inc;
	}
	first = 0;
    }

    glPushMatrix();
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < x; i++) {
	glVertex2i(vertices[i][0], vertices[i][1]);
    }
    glEnd();
    glPopMatrix();


    //draw box
    Shape *s;
    glColor3ub(90,140,90);
    for (int k = 0; k < 5; k++) {
	s = &game->box[k];
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
	glVertex2i(-w,-h);
	glVertex2i(-w, h);
	glVertex2i( w, h);
	glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
    }

    //draw all particles here
    glPushMatrix();
    glColor3ub(150,160,220);
    for (int i = 0; i < game->n; i++)
    {
	Vec *c = &game->particle[i].s.center;
	w = 2;
	h = 2;
	glBegin(GL_QUADS);
	glVertex2i(c->x-w, c->y-h);
	glVertex2i(c->x-w, c->y+h);
	glVertex2i(c->x+w, c->y+h);
	glVertex2i(c->x+w, c->y-h);
	glEnd();
	glPopMatrix();
    }


    //Enter text

    r[5].bot = WINDOW_HEIGHT - 20;
    r[5].left = 10;
    r[5].center = 0;
    unsigned int black = 0x0000000000;

    //ggprint8b(&r[5], 16, cref, "Waterfall Model");

    for (int k = 0; k < 5; k++) {
	s = &game->box[k];

	r[k].bot = s->center.y - 5;
	r[k].left = s->center.x;
	r[k].right = 1;

	if (k == 0) {
	    ggprint8b(&r[k],16,black, "Requirements");
	}
	if (k == 1) {
	    ggprint8b(&r[k],16,black, "Design");
	}
	if (k == 2) {
	    ggprint8b(&r[k],16,black, "Coding");
	}
	if (k == 3) {
	    ggprint8b(&r[k],16,black, "Testing");
	}
	if (k == 4) {
	    ggprint8b(&r[k],16,black, "Maintenance");
	}
    }
}
