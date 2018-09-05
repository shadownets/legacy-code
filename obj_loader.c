#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

#if 0
/* Vertex struct (3d point)
 */
typedef struct vertex {
	float x;
	float y;
	float z;
	float w;
} vertex_t;

/* UVWS struct (tex coords)
 */
typedef struct uvw {
	float u;
	float v;
	float w;
} uvws_t;

/* Normal struct
 */
typedef struct normal {
	float x;
	float y;
	float z;
} normal_t;
#endif 

typedef float vec3[3];
typedef float vec4[4];

/* Vertex struct
 */
typedef struct vertex {
	vec4 xyzw;
} vertex_t;

/* UVWs struct
 */
typedef struct uvws {
	vec3 uvw;
} uvws_t;

/* Normal struct
 */
typedef struct normal {
	vec3 xyz;
} normal_t;

/* Face struct (Polygon)
 */
typedef struct face {
	GLenum type;
	int num_elements;

	int *vertices_indicies;	// indices
	int *uvws_indicies;		// indices
	int *normals_indicies;	// indices
} face_t;

/* Model struct
 */
typedef struct model {
	int num_vertices;
	int num_uvws;
	int num_normals;
	int num_faces;

	int has_uvws;
	int has_normals;

	vertex_t	*vertices;	// vertex list
	uvws_t		*uvws;		// uvws list
	normal_t	*normals;	// normals list
	face_t		*faces;		// polygons list
} model_t;

/* Dealloc model structure
 */
void free_model(model_t *model) {
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
int alloc_model(model_t *model) {

	// alloc number of vertices array
	if (model->num_vertices) {
		model->vertices = (vertex_t*)malloc(sizeof(vertex_t) * model->num_vertices);
		if (!model->vertices) return 0;
	}

	// alloc number of uvws array
	if (model->num_uvws) {
		model->uvws = (uvws_t*)malloc(sizeof(uvws_t) * model->num_uvws);
		if (!model->uvws) return 0;
	}

	// alloc number of normals array
	if (model->num_normals) {
		model->normals = (normal_t*)malloc(sizeof(normal_t) * model->num_normals);
		if (!model->normals) return 0;
	}

	// alloc number of faces array
	if (model->num_faces) {
		model->faces = (face_t*)calloc(model->num_faces, sizeof(face_t));
		if (!model->faces) return 0;
	}

	return 1;
}

/* OLD TESTING CODE DO NOT USE */
#if 0
int test_obj_load(const char *name) {
	FILE *fp = fopen(name, "r");
	if (fp == NULL)
	{
		printf("Can't load file. %s \n", name);
		return 0;
	}

	int i = 0; // index

	memset(model_points, 0, sizeof(vertex_t));
	memset(model_uvws, 0, sizeof(uvw_t));
	memset(model_normals, 0, sizeof(normal_t));

	num_vertices = 0;
	num_uvws = 0;
	num_normals = 0;

	char s[256];
	while (!feof(fp))
	{
		fgets(s, sizeof(s), fp);
		switch (s[0])
		{
			// we have 'comment'
			case '#':
			{
				break;
			}
			// we have 'v' (x,y,z,[w])
			case 'v':
			{
				if (s[1] == ' ')
				{
					if (sscanf_s(s + 2, "%f %f %f %f", model_points[num_vertices].x, model_points[num_vertices].y, model_points[num_vertices].z, model_points[num_vertices].w) != 4)
					{
						if (sscanf_s(s + 2, "%f %f %f", model_points[num_vertices].x, model_points[num_vertices].y, model_points[num_vertices].z) != 3)
						{
							fprintf(stderr, "Error reading vertex data.\n");
							return 0;
						}
						else
						{
							model_points[num_vertices].w = 1.0f;
						}
					}
					num_vertices++;
				}
				// we have 'vt' (u, v, [w])
				else if (s[1] == 't')
				{
					if (sscanf_s(s + 2, "%f %f %f", model_uvws[num_uvws].u, model_uvws[num_uvws].v, model_uvws[num_uvws].w) != 3)
					{
						if (sscanf_s(s + 2, "%f %f", model_uvws[num_uvws].u, model_uvws[num_uvws].v) != 2)
						{
							if (sscanf_s(s + 2, "%f", model_uvws[num_uvws].u) != 1)
							{
								fprintf(stderr, "Error reading uvw data.\n");
								return 0;
							}
							else
							{
								model_uvws[num_uvws].u = 0.0f;
								model_uvws[num_uvws].v = 0.0f;
							}
						}
						else
						{
							model_uvws[num_uvws].v = 0.0f;
						}
					}
					num_uvws++;
				}
				// we have 'vn' (x,y,z)
				else if (s[1] == 'n')
				{
					if (sscanf_s(s + 2, "%f %f %f", model_normals[num_normals].x, model_normals[num_normals].y, model_normals[num_normals].z) != 3) {
						fprintf(stderr, "Error reading normal data.\n");
					}
					num_normals++;
				}
				else
				{
					fprintf(stderr, "Unknown token \"%s\".\n", s[1]);
				}
				break;
			}
			// we have 'f'
			case 'f':
			{
				fprintf(stderr, "Skipping faces.\n");
				break;
			}
			// we have 'g'
			case 'g':
			{
				fprintf(stderr, "Skipping groups.\n");
				break;
			}
			default:
			{
				break;

			}
		}
	}

	printf("\nOBJ statistics:\n================================\n");
	printf("num vertices: %d\n", num_vertices);
	printf("num uvws: %d\n", num_uvws);
	printf("num normals: %d\n", num_normals);

	// close file
	if (fp != NULL)
	{
		fclose(fp);
	}

	return 1;
}
#endif

/* OBJ STUFF
 */
int OBJ_parse_alloc(FILE *fp, model_t *model) {
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
		fprintf(stderr, "error: contradiction between collected info!\n");
		return 0;
	}
	if (!model->num_vertices) {
		fprintf(stderr, "error: no vertex found!\n");
		return 0;
	}

	return 1;
}

/*
 */
int OBJ_allocate_model(FILE *fp, model_t *model) {
	vertex_t	*pvert	= model->vertices;
	uvws_t		*puvw	= model->uvws;
	normal_t	*pnorm	= model->normals;
	face_t		*pface	= model->faces;

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
				pface->vertices_indicies = (int *)malloc(sizeof(int) * pface->num_elements);

				if (model->has_uvws) pface->uvws_indicies = (int *)malloc(sizeof(int) * pface->num_elements);
				if (model->has_normals) pface->normals_indicies = (int *)malloc(sizeof(int) * pface->num_elements);

				// read face data
				ps = s;

				for (int i = 0; i < pface->num_elements; ++i) {
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

					if (model->has_uvws)
						pface->uvws_indicies[i]--;

					if (model->has_normals)
						pface->normals_indicies[i]--;
				}

				pface++;
				break;
			}
		}
	}

	return 1;
}

/*
 */
int OBJ_model_load(const char *filename, model_t *model) {
	FILE *fp;

	fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Error: couldn't open \"%s\"!\n", filename);
		return 0;
	}

	// reset model data
	memset(model, 0, sizeof(model_t));

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
void OBJ_render_model(model_t *model) {

	for (int i = 0; i < model->num_faces; ++i) {
		glBegin(model->faces[i].type);

		for (int j = 0; j < model->faces[i].num_elements; ++j) {

			if (model->has_uvws) {
				glTexCoord3fv(model->uvws[model->faces[i].uvws_indicies[j]].uvw);
			}

			if (model->has_normals) {
				glNormal3fv(model->normals[model->faces[i].normals_indicies[j]].xyz);
			}

			glVertex4fv(model->vertices[model->faces[i].vertices_indicies[j]].xyzw);
		}

		glEnd();
	}
}

/*
 */
model_t myObj;

/*
 */
void init(const char *filename)
{
	GLfloat lightpos[] = { 5.0f, 10.0f, 0.0f, 1.0f };

	/* Initialize OpenGL context */
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	// load OBJ model file
	if (!OBJ_model_load(filename, &myObj)) {
		exit(0);
	}
}

void cleanup(void) 
{	
	free_model(&myObj);
}

void reshape (int w, int h) 
{
	if (h == 0)
		h = 1;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, w/(GLdouble)h, 0.1, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutPostRedisplay();
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glTranslatef (0.0f, 0.0f, -15.0f);

  /* Draw the model */
  glColor3f(1.0f, 0.0f, 0.0f);
  OBJ_render_model(&myObj);


  glFlush();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) exit (0);
}

int main(int argc, char *argv[]) 
{
	if (argc < 2) 
	{
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
