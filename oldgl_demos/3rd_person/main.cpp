#include <windows.h>
#include <GL/glut.h>
#include <GL/GL.h>
#include <math.h>

#define WINDOW_TITLE "GLUT Window - 3rd person camera"
#define M_PI 3.141592654f

/*
 */
float xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 0, angle = 0.0;

float cRadius = 10.0f; // our radius distance from our character
float lastx, lasty;
float positionz[10];
float positionx[10];

/*
 */
void place_cube_positions(void) {
	for (int i = 0; i < 10; i++){
		positionz[i] = rand() % 5 + 1;
		positionx[i] = rand() % 5 + 1;
	}
}

/*
 */
void cube(void) {
	for (int i = 0; i < 10 - 1; i++) {
		glPushMatrix();
		glTranslated(-positionx[i + 1] * 10, 0, -positionz[i + 1] * 10); //translate the cube
		glutSolidCube(2); //draw the cube
		glPopMatrix();
	}
}

/*
 */
void init(void) {
	place_cube_positions();
}

/*
 */
void display(void) {

	glClearColor(0.0, 0.0, 0.0, 1.0); //clear the screen to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST); //enable the depth testing
	glEnable(GL_LIGHTING); //enable the lighting
	glEnable(GL_LIGHT0); //enable LIGHT0, our Diffuse Light
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH); //set the shader to smooth shader

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -cRadius);
	glRotatef(xrot, 1.0, 0.0, 0.0);
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidCube(2); //Our character to follow

	glRotatef(yrot, 0.0, 1.0, 0.0);  //rotate our camera on the y - axis(up and down)
	glTranslated(-xpos, 0.0f, -zpos); //translate the screento the position of our camera
	glColor3f(1.0f, 1.0f, 1.0f);

	cube(); //call the cube drawing function

	glutSwapBuffers(); //swap the buffers
	angle++; //increase the angle
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); //set the viewport to the current window specifications
	glMatrixMode(GL_PROJECTION); //set the matrix to projection

	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW); //set the matrix back to model

}

void onKeyboard(unsigned char key, int x, int y) {

	if (key == 'q') {
		xrot += 1;
		if (xrot > 360) xrot -= 360;
	}

	if (key == 'z') {
		xrot -= 1;
		if (xrot < -360) xrot += 360;
	}

	if (key == 'w') {
		float xrotrad, yrotrad;
		yrotrad = (yrot / 180 * M_PI);
		xrotrad = (xrot / 180 * M_PI);
		xpos += float(sin(yrotrad));
		zpos -= float(cos(yrotrad));
		ypos -= float(sin(xrotrad));
	}

	if (key == 's') {
		float xrotrad, yrotrad;
		yrotrad = (yrot / 180 * M_PI);
		xrotrad = (xrot / 180 * M_PI);
		xpos -= float(sin(yrotrad));
		zpos += float(cos(yrotrad));
		ypos += float(sin(xrotrad));
	}

	if (key == 'd') {
		float yrotrad;
		yrotrad = (yrot / 180 * M_PI);
		xpos += float(cos(yrotrad)) * 0.2;
		zpos += float(sin(yrotrad)) * 0.2;
	}

	if (key == 'a') {
		float yrotrad;
		yrotrad = (yrot / 180 * M_PI);
		xpos -= float(cos(yrotrad)) * 0.2;
		zpos -= float(sin(yrotrad)) * 0.2;
	}

	if (key == 27) {
		exit(0);
	}
}

/*
 */
void onMouseMovement(int x, int y) {
	int diffx = x - lastx; //check the difference between the current x and the last x position
	int diffy = y - lasty; //check the difference between the current y and the last y position

	lastx = x; //set lastx to the current x position
	lasty = y; //set lasty to the current y position

	xrot += (float)diffy; //set the xrot to xrot with the addition of the difference in the y position
	yrot += (float)diffx;    //set the xrot to yrot with the addition of the difference in the x position
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(100, 100);

	glutCreateWindow(WINDOW_TITLE);

	init();

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(onMouseMovement); //check for mouse movement
	glutKeyboardFunc(onKeyboard);

	glutMainLoop();
	return 0;
}