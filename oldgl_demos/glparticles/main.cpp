#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <stdio.h>
#include <math.h>

#define WINDOW_TITLE "GLUT Window - OpenGL Particles"

/*
 */
typedef struct Particle_t {
	double x_pos;
	double y_pos;
	double z_pos;
	double x_move;
	double z_move;
	double r_color;
	double g_color;
	double b_color;
	double direction;
	double acceleration;
	double deceleration;
	double z_scale;
	bool is_visible;
} PARTICLES;

/*
 */
GLfloat texture[10];
const int num_particles = 500;
PARTICLES particle[num_particles];

/*
 */
GLuint LoadTextureRAW(const char * filename, int width, int height);
void FreeTexture(GLuint texturez);

/*
 */
void square(void) {

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);
		glVertex2d(-1.0, -1.0);
		
		glTexCoord2d(1.0, 0.0);
		glVertex2d(1.0, -1.0);
		
		glTexCoord2d(1.0, 1.0);
		glVertex2d(1.0, 1.0);
		
		glTexCoord2d(0.0, 1.0);
		glVertex2d(-1.0, 1.0);
	glEnd();
}

/*
 */
void glCreateParticles(void) {

	 for (int i = 1; i < num_particles; i++) {
		particle[i].x_pos = 0;
		particle[i].y_pos = -5;
		particle[i].z_pos = -5;
		particle[i].x_move = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);
		particle[i].z_move = (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005) - (((((((2 - 1 + 1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1) * 0.005);
		particle[i].r_color = 1;
		particle[i].g_color = 1;
		particle[i].b_color = 1;
		particle[i].z_scale = 0.25;
		particle[i].direction = 0;
		particle[i].acceleration = ((((((8 - 5 + 2) * rand() % 11) + 5) - 1 + 1) * rand() % 11) + 1) * 0.02;
		particle[i].deceleration = 0.0025;
	}
}

/*
 */
void glUpdateParticles(void) {

	for (int i = 1; i < num_particles; i++) {
		glColor3f(particle[i].r_color, particle[i].g_color, particle[i].b_color);

		particle[i].y_pos = particle[i].y_pos + particle[i].acceleration - particle[i].deceleration;
		particle[i].deceleration = particle[i].deceleration + 0.0025;
		particle[i].x_pos = particle[i].x_pos + particle[i].x_move;
		particle[i].z_pos = particle[i].z_pos + particle[i].z_move;
		particle[i].direction = particle[i].direction + ((((((int)(0.5 - 0.1 + 0.1) * rand() % 11) + 1) - 1 + 1) * rand() % 11) + 1);

		if (particle[i].y_pos < -5) {
			particle[i].x_pos = 0;
			particle[i].y_pos = -5;
			particle[i].z_pos = -5;
			particle[i].r_color = 1;
			particle[i].g_color = 1;
			particle[i].b_color = 1;
			particle[i].direction = 0;
			particle[i].acceleration = ((((((8 - 5 + 2) * rand() % 11) + 5) - 1 + 1) * rand() % 11) + 1) * 0.02;
			particle[i].deceleration = 0.0025;
		}
	}
}

/*
 */
void glDrawParticles(void) {

	for (int i = 1; i < num_particles; i++) {
		glPushMatrix();

		glTranslatef(particle[i].x_pos, particle[i].y_pos, particle[i].z_pos);
		glRotatef(particle[i].direction - 90, 0, 0, 1);

		glScalef(particle[i].z_scale, particle[i].z_scale, particle[i].z_scale);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		glBegin(GL_QUADS);
			glTexCoord2d(0, 0);
			glVertex3f(-1, -1, 0);
			
			glTexCoord2d(1, 0);
			glVertex3f(1, -1, 0);
		
			glTexCoord2d(1, 1);
			glVertex3f(1, 1, 0);
			glTexCoord2d(0, 1);
		
			glVertex3f(-1, 1, 0);
		glEnd();

		glBlendFunc(GL_ONE, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, texture[1]);

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
			glVertex3f(-1, -1, 0);
			glTexCoord2d(1, 0);
		
			glVertex3f(1, -1, 0);
			glTexCoord2d(1, 1);
		
			glVertex3f(1, 1, 0);
			glTexCoord2d(0, 1);
		
			glVertex3f(-1, 1, 0);
		glEnd();

		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
}

/*
 */
void display(void) {
	
	glClearDepth(1);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -10);
	
	glUpdateParticles();
	glDrawParticles();
	
	glutSwapBuffers();
}

/*
 */
void init(void) {

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glCreateParticles();

	texture[0] = LoadTextureRAW("particle_mask.raw", 256, 256);
	texture[1] = LoadTextureRAW("particle.raw", 256, 256);
}

/*
 */
void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(WINDOW_TITLE);

	init();
	
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	
	return 0;
}

/*
 */
GLuint LoadTextureRAW(const char * filename, int width, int height) {

	GLuint texture;
	unsigned char * data;
	FILE * file;

	file = fopen(filename, "rb");
	if (file == NULL) return 0;

	data = (unsigned char*)malloc(width * height * 3);

	fread(data, width * height * 3, 1, file);
	fclose(file);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	free(data);

	return texture;
}

/*
 */
void FreeTexture(GLuint texture) {
	glDeleteTextures(1, &texture);
}