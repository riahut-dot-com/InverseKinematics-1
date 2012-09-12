// OpenGL main program
// contains
//    control of animation/simulation module: singel step, continuous run, etc.
//    graphics API initialization and setup (OpenGl in this case): camera, lights, display
//	  user interface handling of mouse and keyboard



// =================================================================================
// STANDARD INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include <math.h>



// LAB SPECIFIC
#include "./animate.h"


// =================================================================================
// STANDARD DEFINES

// Mode
#define	STEP	1
#define RUN	2

// =================================================================================
// TYPEDEFS

// CAMERA
typedef struct camera_struct {
	// polar coordinates controlled by interface
	float	distance;	// distance
	float	theta;		// angle around y-axis from positive x-axis (0:360)
	float	phi;		// angle up from x-z plane (-180:+180)
	// view parameters
	float	x,y,z;		// location
	// in case user interface is expanded to support more info
	float	cx,cy,cz;	// center of interest
	float	tilt;		// rotation around view direction
} camera_td;

// =================================================================================
// PROTOTYPE FUNCTIONS
void display(void);
void timedDisplay(int value);

void displayInitialize();
void displayReshape(int w, int h);

void graphicsInitialize(void);

void lightsInitialize();
void lightsPosition();

void cameraInitialize();
void cameraComputeParameters();

void keyboardParse(unsigned char key, int x, int y);
void keyboardParseSpecial(int key, int x, int y);

void mouseProcessButtons(int button, int state, int x, int y);
void mouseProcessActiveMotion (int x,int y);


// =================================================================================
// GLOBALS for display and interactiion

camera_td	camera;
//int mode = STEP;			// single step or continuous run
int mode = RUN;
int debug = 0;				// debug flag
int fps = 30;				// frames per second - for animation/simulation

// =================================================================================
// DEFINES

// Buttons for user interaction
#define LEFT 1
#define MIDDLE 2
#define RIGHT 3

// Constants
#define PI 3.14159
#define EPSILON 0.0001

// =================================================================================
// MAIN
// =================================================================================
int main(int argc, char** argv)
{
    
	// initialize glut and window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(40, 40);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Animate");

	// setup callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(displayReshape);
	glutKeyboardFunc(keyboardParse);
	glutSpecialFunc(keyboardParseSpecial);
	glutMouseFunc(mouseProcessButtons);
	glutMotionFunc(mouseProcessActiveMotion);
	glutTimerFunc(1000/fps,timedDisplay,0);
	//glutIdleFunc(timedDisplay);

	// initialize stuff
	graphicsInitialize();
	Animation_init();
	cameraInitialize();

    // execute
	glutMainLoop();
	return EXIT_SUCCESS;
}

// =================================================================================
// =================================================================================
// ROUTINES to MANAGE DISPLAY

// ---------------------------------------------------------------------------
// DISPLAY
// ---------------------------------------------------------------------------
void display(void)
{

	if (debug == 1) printf("display\n");
	if (mode == STEP) {
		displayInitialize();
		Animation_render();
		// force out and swap buffers
		glFlush();
		glutSwapBuffers();
	}

}

// ---------------------------------------------------------------------------
// TIMED DISPLAY
// ---------------------------------------------------------------------------
void timedDisplay(int value)
{
	static int debugMode = 0;

	if ((debug - debugMode) == 1) printf("timed display\n");
	debugMode = debug;

	displayInitialize();
	Animation_update();
	Animation_render();

	// force out and swap buffers
	glFlush();
	glutSwapBuffers();
	if (mode == RUN) glutTimerFunc(1000/fps,timedDisplay,0);
}

// ---------------------------------------------------------------------------
// INITIALIZE DISPLAY
// needs to be called each time the scene is to be rendered:
//     clears OpenGL buffer
//     resets transformation matrix
//     sets camera and lights in OpenGL
// ---------------------------------------------------------------------------
void displayInitialize()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear buffer
    glLoadIdentity();
	gluLookAt(camera.x,camera.y,camera.z,camera.cx,camera.cy,camera.cz,0.0,1.0,0.0);
	lightsPosition();
}

// ---------------------------------------------------------------------------
// RESHAPE
// ---------------------------------------------------------------------------
void displayReshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

// =================================================================================
// =================================================================================
// ROUTINES to setup GRAPHICS ENVIRONMENT

// ---------------------------------------------------------------------------
// INITIALIZE GRAPHICS
// only needs to be done once at beginning of program
// ---------------------------------------------------------------------------
void graphicsInitialize (void)
{
    glShadeModel (GL_SMOOTH);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2,0.2,0.8,1.0);
    glColor3f(1.0, 1.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum (-0.1, 0.1, -0.1, 0.1, 0.2, 550.0);
    //gluOrtho2D(0,100, 0,100);
    glMatrixMode(GL_MODELVIEW);

	lightsInitialize();
}

// ---------------------------------------------------------------------------
// CAMERA COMPUTE PARAMETERS
// should be called any time a camera parameter changes
// ---------------------------------------------------------------------------
void cameraComputeParameters(void)
{
	float	radiusOut;

	// rotate up
	radiusOut = cos(camera.theta);
	camera.y = camera.distance*sin(camera.theta);
	// rotate around y-axis
	camera.x = camera.distance*cos(camera.phi)*radiusOut;
	camera.z = camera.distance*sin(camera.phi)*radiusOut;
}

// ---------------------------------------------------------------------------
// CAMERA INITIALIZE
// initialize view parameters for camera
// only needs to be done once at begining or if a 'reset' is requested
// ---------------------------------------------------------------------------
void cameraInitialize ()
{
	// constant parameters
	camera.tilt = 0.0;
	camera.cx = 0.0; camera.cy = 0.0; camera.cz = 0.0;
	// initial polar coordinates
	camera.distance = 50.0;
	camera.phi = PI/2.0;
	camera.theta = 0;//PI/2.0;
	// extract camera coordinates
	cameraComputeParameters();
	glutPostRedisplay();
}

// ---------------------------------------------------------------------------
// INITIALIZE LIGHTS
// called each time a scene is to be rendered
// ---------------------------------------------------------------------------
#define  AMBIENT0   0.5
#define  DIFFUSE0   0.5
#define  SPECULAR0   0.1
#define  AMBIENT1   0.5
#define  DIFFUSE1   0.5
#define  SPECULAR1   0.1
void lightsInitialize()
{
  GLfloat light0_ambient[]  = { AMBIENT0,AMBIENT0,AMBIENT0,1.0 };
  GLfloat light0_diffuse[]  = { DIFFUSE0,DIFFUSE0,DIFFUSE0,1.0 };
  GLfloat light0_specular[] = { SPECULAR0,SPECULAR0,SPECULAR0,1.0 };

  GLfloat light1_ambient[]  = { AMBIENT1,AMBIENT1,AMBIENT1,1.0 };
  GLfloat light1_diffuse[]  = { DIFFUSE1,DIFFUSE1,DIFFUSE1,1.0 };
  GLfloat light1_specular[] = { SPECULAR1,SPECULAR1,SPECULAR1,1.0 };


  //glEnable(GL_LIGHTING);

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR,light0_specular);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR,light1_specular);
}

// ---------------------------------------------------------------------------
// POSITION LIGHTS
// called each time a scene is to be rendered
// ---------------------------------------------------------------------------
void lightsPosition()
{
  GLfloat light0_position[] = { 0.0, 10.0, 3.0, 1.0 };
  GLfloat light1_position[] = { -5.0,-10.0,2.0,1.0 };

  glLightfv(GL_LIGHT0, GL_POSITION,light0_position);
  glLightfv(GL_LIGHT1, GL_POSITION,light1_position);
}


// =================================================================================
// =================================================================================
// ROUTINES to MANAGE INTERACTION

// ---------------------------------------------------------------------------
// PARSE KEY
// ---------------------------------------------------------------------------
void keyboardParse(unsigned char key, int x, int y)
{
    switch (key) {
		case 27: exit(0);						// cntrl-Q
		case 'v':							// v: reset view
			cameraInitialize();
			cameraComputeParameters();
			glutPostRedisplay();
			break;
		case 'q':							// q: quit
			exit(1);
		case 'd':							// d: toggle debug
			debug = 1 - debug;
			if (debug == 0) printf("debug off\n");
			else printf("debug on\n");
			break;
		case 'r':							// r: reset
			fps = 30;
			Animation_reset();
			glutPostRedisplay();
			break;
		case 's':							// s: single step
			if ((mode != STEP) && (debug == 1)) printf("mode = STEP\n");
			mode = STEP;
			Animation_update();
			glutPostRedisplay();
			break;
		case 'c':							// c: continuous run
			if (mode != RUN) {
				mode = RUN;
				if (debug == 1) printf("mode = RUN\n");
				glutTimerFunc(1000/fps,timedDisplay,0);
			}
			break;
		case 'h':							// h: help
			printf("\n");
			printf("mouse movement\n");
			printf("left mouse button down -> rotate around origin\n");
			printf("middle mouse button down -> zoom in and out\n");
			printf("\n");
			printf("keys\n");
			printf("---\n");
			printf("d: toggle debug\n");
			printf("s: single step\n");
			printf("c: continuous run\n");
			printf("r: reset parameters\n");
			printf("q: quit\n");
			printf("v: view reset\n");
			printf("h: help\n");
			break;
		case '+':							// +: speed up
			if (debug == 1) printf("speed up\n");
			fps = 2*fps;
			break;
		case '-':							// -: slow down
			if (debug == 1) printf("slow down\n");
			fps = fps/2;
			break;
		default:
			printf("%d\n",key);
    }
}


// ---------------------------------------------------------------------------
// PARSE SPECIAL KEY
// ---------------------------------------------------------------------------
void keyboardParseSpecial(int key, int x, int y)
{
    switch (key) {
		case GLUT_KEY_UP:	camera.theta -= 0.25; break;
		case GLUT_KEY_DOWN:	camera.theta += 0.25; break;
		case GLUT_KEY_RIGHT:	camera.phi += 0.25; break;
		case GLUT_KEY_LEFT:	camera.phi -= 0.25; break;
    }
	cameraComputeParameters();
	glutPostRedisplay();
}

int mousex,mousey;
float eyeD,eyeA,eyeT;
int	  buttonDown;

// ---------------------------------------------------------------------------
// PROCESS MOUSE
// when down, use to control rotation of camera around origin
// ---------------------------------------------------------------------------
void mouseProcessButtons(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)  {
		if (button == GLUT_LEFT_BUTTON) {
			buttonDown = LEFT;
			eyeA = camera.phi;
			eyeT = camera.theta;
		}
		else if (button == GLUT_MIDDLE_BUTTON) {
				buttonDown = MIDDLE;
				eyeD = camera.distance;
		}
		else {
				buttonDown = RIGHT;
		}
		mousex = x; mousey = y;
	}
}

// ---------------------------------------------------------------------------
// PROCESS MOUSE ACTIVE MOTION
// ---------------------------------------------------------------------------
void mouseProcessActiveMotion (int x,int y)
{
	if (buttonDown == LEFT) {
		// angle around
		camera.phi = eyeA + 0.005*(x - mousex);

		// angle up and down
		camera.theta = eyeT + 0.005*(y - mousey);
		if (camera.theta >= PI/2) camera.theta = PI/2 - EPSILON;
		if (camera.theta <= -PI/2) camera.theta = -(PI/2 - EPSILON);
	}
	else if (buttonDown == MIDDLE) {
		camera.distance = eyeD*(1.0 + (y - mousey)/500.0);
	}
	cameraComputeParameters();
	glutPostRedisplay();
}

