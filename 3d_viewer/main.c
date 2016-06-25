#include <GL/glut.h>
#include <stdio.h>

GLfloat cameraZ = -800.0f;
GLfloat cameraX = 0.0f;

GLfloat *vertices;
GLubyte *indices;

int verticesQuantity, trianglesQuantity;

void loadFile(char *name)
{
	FILE * fp = fopen(name, "r");
	fscanf(fp, "%d %d", &verticesQuantity, &trianglesQuantity);

	verticesQuantity *= 3;
	trianglesQuantity *= 3;

	vertices = malloc(sizeof(GLfloat) + &verticesQuantity);
	indices = malloc(sizeof(GLubyte) + &trianglesQuantity);

	int j = 0;
	for (int i = 0; i < verticesQuantity / 3; i++)
	{
		float x, y, z;
		fscanf(fp, "%f %f %f", &x, &y, &z);

		vertices[j] = x;
		vertices[j + 1] = -y;
		vertices[j + 2] = z;

		j += 3;
	}

	j = 0;
	for (int i = 0; i < trianglesQuantity / 3; i++)
	{
		int x, y, z;
		fscanf(fp, "%d %d %d", &x, &y, &z);

		indices[j] = x - 1;
		indices[j + 1] = y - 1;
		indices[j + 2] = z - 1;

		j += 3;
	}
		
	fclose(fp);
}

void initLights()
{
	// set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] = { 0.0f, 0.0f, 1.0f, 1.0f };  // ambient light
	GLfloat lightKd[] = { 1.0f, 0.0f, 0.0f, 1.0f };  // diffuse light
	GLfloat lightKs[] = { 1, 1, 1, 1 };           // specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	// position the light
	float lightPos[4] = { 0, 300, -200, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}


void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	initLights();

	float white[] = { 1,1,1,1 };
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);

	loadFile("../objects/calice2.byu");
}

void drawCubes()
{
	// enable and specify pointers to vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glNormalPointer(GL_FLOAT, 0, normals);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glPushMatrix();
	// set colour of cube

	// draw a cube
	glDrawElements(GL_TRIANGLES, trianglesQuantity, GL_UNSIGNED_BYTE, indices);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);

}

void display(void)
{
	// clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//printf("glTranslatef: %f %f\n", cameraX, cameraZ);
	glTranslatef(cameraX, 0, cameraZ);

	cameraX = 0.0f;
	cameraZ = 0.0f;

	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();

	drawCubes();

	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)(w) / h, 0.1f, 10000.0f); // FOV, AspectRatio, NearClip, FarClip

																				// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{

}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'a') {
		cameraX -= 10.0f;
	} else if (key == 'd') {
		cameraX += 10.0f;
	} else if (key == 'w') {
		cameraZ -= 10.0f;
	} else if (key == 's') {
		cameraZ += 10.0f;
	}

	display();

	//printf("%f %f\n", cameraX, cameraZ);
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);   // display mode
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}