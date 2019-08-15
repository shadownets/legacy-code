#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

#define MODEL_LOADER_TYPE "OBJ Wavefront"

typedef float vec3[3];
typedef float vec4[4];

/* Vertex struct
 */
struct vertex_t {
	vec4 xyzw;
};

/* UVWs struct
 */
struct uvws_t {
	vec3 uvw;
};

/* Normal struct
 */
struct normal_t {
	vec3 xyz;
};

/* Face struct (Polygon)
 */
struct face_t {
	GLenum type;
	int num_elements;

	int *vertices_indicies;	// indices
	int *uvws_indicies;		// indices
	int *normals_indicies;	// indices
};

/* Model struct
 */
struct model_t {
	int num_vertices;
	int num_uvws;
	int num_normals;
	int num_faces;

	int has_uvws;
	int has_normals;

	struct vertex_t	*vertices;	// vertex list
	struct uvws_t	*uvws;		// uvws list
	struct normal_t	*normals;	// normals list
	struct face_t	*faces;		// polygons list
};

/* Dealloc model structure
 */
void free_model(struct model_t* model) {
	if (model) {
		if (model->vertices) {
			free(model->vertices);
			model->vertices = NULL;
		}

		if (model->uvws) {
			free(model->uvws);
			model->uvws = NULL;
		}

		if (model->normals) {
			free(model->normals);
			model->normals = NULL;
		}

		for (int i = 0; i < model->num_faces; ++i) {
			if (model->faces[i].vertices_indicies)	free(model->faces[i].vertices_indicies);
			if (model->faces[i].uvws_indicies)		free(model->faces[i].uvws_indicies);
			if (model->faces[i].normals_indicies)	free(model->faces[i].normals_indicies);
		}

		free(model->faces);
		model->faces = NULL;
	}
}

/* Alloc model structure
 */
int alloc_model(struct model_t *model) {

	// alloc number of vertices array
	if (model->num_vertices) {
		model->vertices = (struct vertex_t*)malloc(sizeof(struct vertex_t) * model->num_vertices);
		if (!model->vertices) return 0;
	}

	// alloc number of uvws array
	if (model->num_uvws) {
		model->uvws = (struct uvws_t*)malloc(sizeof(struct uvws_t) * model->num_uvws);
		if (!model->uvws) return 0;
	}

	// alloc number of normals array
	if (model->num_normals) {
		model->normals = (struct normal_t*)malloc(sizeof(struct normal_t) * model->num_normals);
		if (!model->normals) return 0;
	}

	// alloc number of faces array
	if (model->num_faces) {
		model->faces = (struct face_t*)calloc(model->num_faces, sizeof(struct face_t));
		if (!model->faces) return 0;
	}

	return 1;
}

/* OBJ STUFF
 */
int OBJ_parse_alloc(FILE *fp, struct model_t *model) {
	int v, t, n = 0;
	char s[256];

	while (!feof(fp)) {
		fgets(s, sizeof(s), fp);
		switch (s[0]) {
			// we found 'v'
			case 'v':
			{
				if (s[1] == ' ') {
					model->num_vertices++;
				} else if (s[1] == 't') {
					model->num_uvws++;
				} else if (s[1] == 'n') {
					model->num_normals++;
				} else {
					printf("Warning: unknown token \"%s\"! (ignoring)\n", s);
				}
				break;
			}

			// we found 'f'
			case 'f':
			{
				if (sscanf(s + 2, "%d/%d/%d", &v, &n, &t) == 3) {
					model->num_faces++;
					model->has_uvws = 1;
					model->has_normals = 1;
				} else if (sscanf(s + 2, "%d//%d", &v, &n) == 2) {
					model->num_faces++;
					model->has_uvws = 0;
					model->has_normals = 1;
				} else if (sscanf(s + 2, "%d/%d", &v, &t) == 2) {
					model->num_faces++;
					model->has_uvws = 1;
					model->has_normals = 0;
				} else if (sscanf(s + 2, "%d", &v) == 1) {
					model->num_faces++;
					model->has_uvws = 0;
					model->has_normals = 0;
				} else {
					fprintf(stderr, "Error: found face with no vertex!\n");
				}
				break;
			}

			case 'g':
			{
				// skip groups
				break;
			}

			default:
			{
				break;
			}
		}
	}

	// validation
	if ((model->has_uvws && !model->num_uvws) || (model->has_normals && !model->num_normals)) {
		fprintf(stderr, "Error: contradiction between collected info!\n");
		return 0;
	}

	if (!model->num_vertices) {
		fprintf(stderr, "Error: no vertex found!\n");
		return 0;
	}

#ifdef _DEBUG
	printf("%s Parse Information:\n", MODEL_LOADER_TYPE);
	printf("----------------------------------------\n");
	printf("\t%i vertices\n", model->num_vertices);
	printf("\t%i texture coords.\n", model->num_uvws);
	printf("\t%i normal vectors\n", model->num_normals);
	printf("\t%i faces\n", model->num_faces);
	printf("\thas texture coords.: %s\n", model->has_uvws ? "yes" : "no");
	printf("\thas normals: %s\n", model->has_normals ? "yes" : "no");
#endif

	return 1;
}

/*
 */
int OBJ_allocate_model(FILE *fp, struct model_t *model) {

	struct vertex_t	*pvert = model->vertices;
	struct uvws_t	*puvw = model->uvws;
	struct normal_t	*pnorm = model->normals;
	struct face_t	*pface = model->faces;
	int i;

	char s[128], *ps;

	while (!feof(fp)) {

		// read line
		fgets(s, sizeof(s), fp);

		switch (s[0]) {
			////////////////////////////
			// we have 'v'
			////////////////////////////
			case 'v':
			{
				if (s[1] == ' ') {
					if (sscanf(s + 2, "%f %f %f %f", &pvert->xyzw[0], &pvert->xyzw[1], &pvert->xyzw[2], &pvert->xyzw[3]) != 4) {
						if (sscanf(s + 2, "%f %f %f", &pvert->xyzw[0], &pvert->xyzw[1], &pvert->xyzw[2]) != 3) {
							fprintf(stderr, "Error reading vertex data!\n");
							return 0;
						} else {
							pvert->xyzw[3] = 1.0;
						}
					}
					pvert++;
				} 
				// we have 'vt'
				else if (s[1] == 't') {
					if (sscanf(s + 2, "%f %f %f", &puvw->uvw[0], &puvw->uvw[1], &puvw->uvw[2]) != 3) {
						if (sscanf(s + 2, "%f %f", &puvw->uvw[0], &puvw->uvw[1]) != 2) {
							if (sscanf(s + 2, "%f", &puvw->uvw[0]) != 1) {
								fprintf(stderr, "Error reading texture coordinates!\n");
								return 0;
							} else {
								puvw->uvw[1] = 0.0;
								puvw->uvw[2] = 0.0;
							}
						} else {
							puvw->uvw[2] = 0.0;
						}
					}

					puvw++;
				}
				// we have 'n'
				else if (s[1] == 'n') {
					if (sscanf(s + 2, "%f %f %f", &pnorm->xyz[0], &pnorm->xyz[1], &pnorm->xyz[2]) != 3) {
						fprintf(stderr, "Error reading normal vectors!\n");
						return 0;
					}

					pnorm++;
				}
				break;
			}
			////////////////////////////
			// we have 'f'
			////////////////////////////
			case 'f':
			{
				ps = s;
				pface->num_elements = 0;

				// count number of vertices for this face
				while (*ps) {
					if (*ps == ' ') pface->num_elements++;
					ps++;
				}

				// select primitive type 
				if (pface->num_elements < 3) {
					fprintf(stderr, "Error: a face must have at least 3 vertices!\n");
					return 0;
				} else if (pface->num_elements == 3) {
					pface->type = GL_TRIANGLES;
				} else if (pface->num_elements == 4) {
					pface->type = GL_QUADS;
				} else {
					pface->type = GL_POLYGON;
				}

				// memory allocation for vertices
				pface->vertices_indicies = (int*)malloc(sizeof(int) * pface->num_elements);

				if (model->has_uvws) pface->uvws_indicies = (int*)malloc(sizeof(int) * pface->num_elements);
				if (model->has_normals) pface->normals_indicies = (int*)malloc(sizeof(int) * pface->num_elements);

				// read face data
				ps = s;
				i = 0;

				for ( i = 0; i < pface->num_elements; ++i) {
					ps = strchr(s, ' ');
					ps++; // skip space

					// try reading vertices
					if (sscanf(ps, "%d/%d/%d", &pface->vertices_indicies[i], &pface->uvws_indicies[i], &pface->normals_indicies[i]) != 3) {
						if (sscanf(ps, "%d//%d", &pface->vertices_indicies[i], &pface->normals_indicies[i]) != 2) {
							if (sscanf(ps, "%d/%d", &pface->vertices_indicies[i], &pface->normals_indicies[i]) != 2) {
								sscanf(ps, "%d", &pface->vertices_indicies[i]);
							}
						}
					}

					// indices must start at 0
					pface->vertices_indicies[i]--;

					if (model->has_uvws) pface->uvws_indicies[i]--;
					if (model->has_normals) pface->normals_indicies[i]--;
				}

				pface++;
				break;
			}
		}
	}

#ifdef _DEBUG
	printf("%s Alloc Information:\n", MODEL_LOADER_TYPE);
	printf("----------------------------------------\n");
	printf("\t%li vertices\n", pvert - model->vertices);
	printf("\t%li texture coords.\n", puvw - model->uvws);
	printf("\t%li normal vectors\n", pnorm - model->normals);
	printf("\t%li faces\n", pface - model->faces);
#endif

	return 1;
}

/*
 */
int OBJ_model_load(const char *filename, struct model_t *model) {

	FILE *fp;

	fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Error: couldn't open \"%s\"!\n", filename);
		return 0;
	}

	// reset model data
	memset(model, 0, sizeof(struct model_t));

	// read model info
	if (!OBJ_parse_alloc(fp, model)) {
		fclose(fp);
		return 0;
	}

	rewind(fp);

	// memory allocation
	if (!alloc_model(model)) {
		fclose(fp);
		free_model(model);
		return 0;
	}

	// second pass: read model data
	if (!OBJ_allocate_model(fp, model)) {
		fclose(fp);
		free_model(model);
		return 0;
	}

	fclose(fp);
	return 1;
}	

/*
 */
void OBJ_render_model(struct model_t *model) {

	int i, j;

	for ( i = 0; i < model->num_faces; ++i) {
		glBegin(model->faces[i].type);
		for ( j = 0; j < model->faces[i].num_elements; ++j) {

			if (model->has_uvws) glTexCoord3fv(model->uvws[model->faces[i].uvws_indicies[j]].uvw);
			if (model->has_normals) glNormal3fv(model->normals[model->faces[i].normals_indicies[j]].xyz);

			glVertex4fv(model->vertices[model->faces[i].vertices_indicies[j]].xyzw);
		}
		glEnd();
	}
}

/*
 */
struct model_t myObj;
float z_translate = 0.0f;

/*
 */
void init(const char *filename) {
	GLfloat lightpos[] = { 5.0f, 10.0f, 0.0f, 1.0f };

	// init opengl
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	// load OBJ model file
	if (!OBJ_model_load(filename, &myObj)) {
		exit(-1);
	}
}

/*
 */
void cleanup(void)  {	
	free_model(&myObj);
}

/*
 */
void reshape (int w, int h) {

	if (h == 0)
		h = 1;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, w/(GLdouble)h, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutPostRedisplay();
}

/*
 */
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, z_translate);
	printf("\nZ-translate: %f", z_translate);

	/* Draw the model */
	//glColor3f(1.0f, 0.0f, 0.0f);
	OBJ_render_model(&myObj);

	//glFlush();
	glutSwapBuffers();
}

/*
 */
void keyboard(unsigned char key, int x, int y) {
	if (key == 27) exit (0);
	if (key == '-') z_translate -= 2.0f;
	if (key == '+') z_translate += 2.0f;

	display();
}

/*
 */
int main(int argc, char *argv[])  {

	if (argc < 2) {
      fprintf (stderr, "usage: %s <filename.obj>\n", argv[0]);
      return -1;
    }
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (640, 480);
	glutCreateWindow("OBJ Model");

	atexit(cleanup);
	init(argv[1]);

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	
	return 0;
}
