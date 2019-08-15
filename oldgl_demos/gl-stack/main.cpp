// this file is dated around feb 2007
// using std::vector to store points
// render_string function utilizing GLUT BitmapCharacter
// TODO: solve Z-buffer and redo project to be able to build objects in 3d space

// fps camera (doom style)
// glplacedecal (do roomu na klik ve stylu glStack + 3d intersection)
// gl particles - particle emmiter
// a neco na perilin noise
// AABB collision test

#include <stdlib.h>
#include <stdarg.h>
#include <GL/glut.h>
#include <string.h>
#include <vector>

#define WINDOW_TITLE "GLUT Window - Adding points"

int display_mode = GL_POINT;
int window_height;
int window_width;
int mouse_x;
int mouse_y;

/* 3d point
 */
typedef struct Point_t {
	GLint x;
	GLint y;
	GLint z;

	GLfloat r;
	GLfloat g;
	GLfloat b;
} POINT_3D;

/* stack
 */ 
typedef struct Node_t {
	int id;
	POINT_3D point; // data
	struct Node_t *next;
} NODE;

NODE *top = NULL;
int node_size = 0;

/*
 */
void node_push(int x, int y, int z = 0.0f) {

	// random generate vertex colors
	const float scale = 1.0;

	float r = ((float)rand() / (float)(RAND_MAX)) * scale;
	float g = ((float)rand() / (float)(RAND_MAX)) * scale;
	float b = ((float)rand() / (float)(RAND_MAX)) * scale;

	POINT_3D point = { x, y, z, r, g, b };
	++node_size;
	NODE *n = (NODE*)malloc(sizeof(NODE));
	n->id = node_size;
	n->point = point;
	n->next = top;
	top = n;
}

/*
 */
void node_pop() {

	if (top == NULL) {
		return;
	}

	NODE *temp;
	temp = top;
	top = top->next;
	free(temp);
}

/*
 */
void add_point(int x, int y, int z = 0.0f) {
	POINT_3D p = { x, y, z };
	node_push(x, y, z);
}

/*
 */
void remove_point() {
	if (node_size <= 0) return;
	node_pop();
}

/*
 */
void render_string(GLdouble x, GLdouble y, const char *string) {
	const char *c;
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2d(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}

/*
 */
void render_axis2d() {
	// coor system

	// X
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex2i(0, window_height / 2);
		glVertex2i(window_width, window_height / 2);
	glEnd();

	// Y
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex2i(window_width / 2, 0);
		glVertex2i(window_width / 2, window_height);
	glEnd();
}

/*
 */
void display(void) {
	
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw help
	render_string(0, 0, "Right Mouse Button - Add Point");
	render_string(0, 24, "Left Mouse Button - Remove Point");
	render_string(mouse_x, mouse_y, "Click to add");

	// render grid
	render_axis2d();

	// user draw
	NODE *node = top;
	
	glBegin(GL_TRIANGLE_STRIP);
		while (node != NULL) {
			glColor3f(node->point.r, node->point.g, node->point.b);
			glVertex2i(node->point.x, window_height - node->point.y);
			node = node->next;
		}
	glEnd();
	glFlush();

	// swap buffers
	glutSwapBuffers();
}

/*
 */
void cleanup(void) {
	for (int i = 0; i < node_size; i++) {
		node_pop();
	}
}

/*
 */
void reshape(int w, int h) {
	
	if (h == 0) h = 1;

	glViewport (0, 0,w, h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D(0, w, 0, h);

	window_width = w;
	window_height = h;

	glutPostRedisplay();
}

/*
 */
void onMouseClick(int button, int state, int x, int y) {
	
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { 
		add_point(x, y);
	}	
  
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		remove_point();
	}

	mouse_x = x;
	mouse_y = window_height - y; /* swap Y coordinate because opengl is Y-fliped */

	glutPostRedisplay();
}

void onKeyboard(unsigned char key, int x, int y) {
	if (key == 27) {
		exit(0);
	}
}

/*
 */
int main(int argc, char **argv) {
	
	glutInit (&argc, argv);
	glutInitWindowSize(1270, 720);
	glutCreateWindow(WINDOW_TITLE);

	atexit(cleanup);

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(onKeyboard);
	glutMouseFunc(onMouseClick);

	glutMainLoop();
	
	return 0;
}