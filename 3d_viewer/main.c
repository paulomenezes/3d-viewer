#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

GLfloat cameraZ = -1200.0f;
GLfloat cameraX = 0.0f;

GLfloat *vertices;
GLfloat *normals;
GLuint *indices;

int verticesQuantity, trianglesQuantity;

struct Face {
	int vertice1;
	int vertice2;
	int vertice3;

	float verticeX[3];
	float verticeY[3];
	float verticeZ[3];

	GLfloat normal[3];
};

struct Face *faces;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 1.0f, 1.0f };		// ambient 
GLfloat lightDiffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };		// diffuse 
GLfloat lightSpecular[] = { 1, 0, 0, 1 };					// specular 
float lightPosition[4] = { 100, 400, 0, 0 };				// position

float ambient[] = { 1, 0, 0, 1 };
float diffuse[] = { 1, 1, 0, 1 };
float specular[] = { 1, 1, 0, 1 };
float emission[] = { 0, 0, 0, 0 };

float FOV = 45;
float AspectRatio = 1;
float Near = 0.1f;
float Far = 10000.0f;

int n = 0;

int width;
int height;

GLfloat C[] = { 0, 0, 0 };
GLfloat N[] = { 0, 0, 0 };
GLfloat V[] = { 0, 1, 0 };

// Camera rotate

// angle of rotation for the camera direction
float angle = 0.0f;

// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;

// XZ position of the camera
float x = 0.0f, z = 5.0f;

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;
int xOrigin = -1;

void loadFile(char *name)
{
	FILE * fp = fopen(name, "r");
	fscanf(fp, "%d %d", &verticesQuantity, &trianglesQuantity);

	//verticesQuantity -= 1;
	//trianglesQuantity += 1;

	verticesQuantity *= 3;
	trianglesQuantity *= 3;

	int trianglesQuantity2 = trianglesQuantity / 3;
	int verticesQuantity2 = verticesQuantity / 3;

	vertices = malloc(sizeof(GLfloat) + &verticesQuantity);
	indices = malloc(sizeof(GLuint) + &trianglesQuantity);
	faces = malloc(sizeof(struct Face) + &trianglesQuantity2);

	normals = malloc(sizeof(GLfloat) + &verticesQuantity);

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

		faces[i].vertice1 = indices[j];
		faces[i].vertice2 = indices[j + 1];
		faces[i].vertice3 = indices[j + 2];

		faces[i].verticeX[0] = vertices[indices[j]];
		faces[i].verticeX[1] = vertices[indices[j] + 1];
		faces[i].verticeX[2] = vertices[indices[j] + 2];

		faces[i].verticeY[0] = vertices[indices[j + 1]];
		faces[i].verticeY[1] = vertices[indices[j + 1] + 1];
		faces[i].verticeY[2] = vertices[indices[j + 1] + 2];

		faces[i].verticeZ[0] = vertices[indices[j + 2]];
		faces[i].verticeZ[1] = vertices[indices[j + 2] + 1];
		faces[i].verticeZ[2] = vertices[indices[j + 2] + 2];

		float Vx = faces[i].verticeX[0] - faces[i].verticeY[0];
		float Vy = faces[i].verticeX[1] - faces[i].verticeY[1];
		float Vz = faces[i].verticeX[2] - faces[i].verticeY[2];

		float Wx = faces[i].verticeX[0] - faces[i].verticeZ[0];
		float Wy = faces[i].verticeX[1] - faces[i].verticeZ[1];
		float Wz = faces[i].verticeX[2] - faces[i].verticeZ[2];

		float PVx = Vy * Wz - Vz * Wy;
		float PVy = Vz * Wx - Vx * Wz;
		float PVz = Vx * Wy - Vy * Wx;

		float norma = sqrt(pow(PVx, 2) + pow(PVy, 2) + pow(PVz, 2));

		float normalizadoX = PVx / norma;
		float normalizadoY = PVy / norma;
		float normalizadoZ = PVz / norma;

		faces[i].normal[0] = normalizadoX;
		faces[i].normal[1] = normalizadoY;
		faces[i].normal[2] = normalizadoZ;

		j += 3;
	}

	for (int i = 0; i < verticesQuantity; i++)
	{
		normals[i] = 0;
	}

	for (int i = 0; i < trianglesQuantity / 3; i++)
	{
		normals[faces[i].vertice1] += faces[i].normal[0];
		normals[faces[i].vertice1 + 1] += faces[i].normal[1];
		normals[faces[i].vertice1 + 2] += faces[i].normal[2];

		normals[faces[i].vertice2] += faces[i].normal[0];
		normals[faces[i].vertice2 + 1] += faces[i].normal[1];
		normals[faces[i].vertice2 + 2] += faces[i].normal[2];

		normals[faces[i].vertice3] += faces[i].normal[0];
		normals[faces[i].vertice3 + 1] += faces[i].normal[1];
		normals[faces[i].vertice3 + 2] += faces[i].normal[2];
	}

	j = 0;
	for (int i = 0; i < verticesQuantity; i++)
	{
		float norma = sqrt(pow(normals[j], 2) + pow(normals[j + 1], 2) + pow(normals[j + 2], 2));

		float normalizadoX = normals[j] / norma;
		float normalizadoY = normals[j + 1] / norma;
		float normalizadoZ = normals[j + 2] / norma;

		normals[j] = normalizadoX;
		normals[j + 1] = normalizadoY;
		normals[j + 2] = normalizadoZ;

		j += 3;
	}
		
	fclose(fp);
}

void initLights()
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbient);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	glEnable(GL_LIGHT0);
}

void loadData()
{
	FILE * fp = fopen("../dados.txt", "r");
	fscanf(fp, "%f %f %f", &C[0], &C[1], &C[2]);
	fscanf(fp, "%f %f %f %f %f %f", &N[0], &N[1], &N[2], &V[0], &V[1], &V[2]);
	fscanf(fp, "%f %f %f %f", &FOV, &AspectRatio, &Near, &Far);
	fscanf(fp, "%f %f %f %f", &lightPosition[0], &lightPosition[1], &lightPosition[2], &lightPosition[3]);
	fscanf(fp, "%f %f %f %f", &lightAmbient[0], &lightAmbient[1], &lightAmbient[2], &lightAmbient[3]);
	fscanf(fp, "%f %f %f %f", &lightDiffuse[0], &lightDiffuse[1], &lightDiffuse[2], &lightDiffuse[3]);
	fscanf(fp, "%f %f %f %f", &lightSpecular[0], &lightSpecular[1], &lightSpecular[2], &lightSpecular[3]);
	fscanf(fp, "%f %f %f %f", &ambient[0], &ambient[1], &ambient[2], &ambient[3]);
	fscanf(fp, "%f %f %f %f", &diffuse[0], &diffuse[1], &diffuse[2], &diffuse[3]);
	fscanf(fp, "%f %f %f %f", &specular[0], &specular[1], &specular[2], &specular[3]);
	fscanf(fp, "%f %f %f %f", &emission[0], &emission[1], &emission[2], &emission[3]);
	fscanf(fp, "%d", &n);

	fclose(fp);
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	loadData();
	initLights();

	loadFile("../objects/calice2.byu");
}

void draw()
{
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glNormalPointer(GL_FLOAT, 0, normals);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	glMaterialf(GL_FRONT, GL_SHININESS, n);

	glDrawElements(GL_TRIANGLES, trianglesQuantity, GL_UNSIGNED_INT, indices);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void computePos(float deltaMove) {
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void display(void)
{
	if (deltaMove)
		computePos(deltaMove);

	gluLookAt(N[0] + x, N[1], N[2] + z, C[0] + x + lx, C[1], C[2] + z + lz, V[0], V[1], V[2]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glTranslatef(cameraX, 0, cameraZ);

	cameraX = 0.0f;
	cameraZ = 0.0f;
	
	glPushMatrix();
	
	draw();
	
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	width = w;
	height = h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, AspectRatio, Near, Far); // FOV, AspectRatio, NearClip, FarClip

	gluLookAt(N[0] + x, N[1], N[2] + z, C[0] + x + lx, C[1], C[2] + z + lz, V[0], V[1], V[2]);

	glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) {
			angle += deltaAngle;
			xOrigin = -1;
		} else {
			xOrigin = x;
		}
	}
}

void mouseMove(int button, int state, int x, int y)
{
	if (xOrigin >= 0) {
		deltaAngle = (x - xOrigin) * 0.01f;

		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
	}
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'a') {
		cameraX -= 10.0f;
	} else if (key == 'd') {
		cameraX += 10.0f;
	} else if (key == 'w') {
		cameraZ += 10.0f;
	} else if (key == 's') {
		cameraZ -= 10.0f;
	} else if (key == 'r') {
		loadData();
		initLights();
		reshape(width, height);
	}

	display();
}


void pressKey(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_UP: deltaMove = 0.5f; break;
		case GLUT_KEY_DOWN: deltaMove = -0.5f; break;
	}
}

void releaseKey(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN: deltaMove = 0; break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("3D Viewer - OpenGL");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutMainLoop();
	return 0;
}