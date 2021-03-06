#include "glsupport.h"
#include <glut.h>
#include "matrix4.h"
#include <iostream>
#include "quat.h"
#include <vector>
#include "geometrymaker.h"
#include <string>
#include <cassert>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

GLint program;
GLuint vertPostionVBO;
GLuint normalPositionVBO;
GLuint positionAttribute;

GLuint texCoordAttribute;
GLuint binormalAttribute;
GLuint tangentAttribute;

GLuint vertColorVBO;
GLuint colorAttribute;

GLuint modelViewMatrixUniformLocation;
GLuint projectionMatrixUniformLocation;
GLuint positionUniform;

GLuint normalAttribute;
GLuint normalMatrixUniformLocation;
GLuint uColorLocation;

GLuint vertexBO1, vertexBO2, vertexBO3, vertexBO4;
GLuint indexBO1, indexBO2, indexBO3, indexBO4;

GLuint diffuseTexture;
GLuint diffuseTextureUniformLocation;

GLuint specularTexture;
GLuint specularTextureUniformLocation;

GLuint normalTexture;
GLuint normalTextureUniformLocation;

GLuint lightDirectionUniform;
GLuint lightColorUniform;
GLuint specularLightColorUniform;

GLuint lightDirectionUniform1;
GLuint lightColorUniform1;
GLuint specularLightColorUniform1;


typedef struct Entity Entity;



struct VertexPNTBTG {
	Cvec3f p;
	Cvec3f n;
	Cvec2f t;
	Cvec3f b;
	Cvec3f tg;
	
	VertexPNTBTG() {}
	VertexPNTBTG(float x, float y, float z, float nx, float ny, float nz) : p(x, y, z), n(nx, ny, nz) {}

	VertexPNTBTG& operator = (const GenericVertex& v) {
		p = v.pos;
		n = v.normal;
		t = v.tex;
		b = v.binormal;
		tg = v.tangent;
		return *this;
	}
};

struct Geometry {
	int numIndices;
	void Draw(string type) {
	if (type == "Sphere") {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO1);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, p));
		glEnableVertexAttribArray(positionAttribute);

		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, n));
		glEnableVertexAttribArray(normalAttribute);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO1);

		
	}
	if (type == "Plane") {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO2);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, p));
		glEnableVertexAttribArray(positionAttribute);

		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, n));
		glEnableVertexAttribArray(normalAttribute);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO2);
	}
	if (type == "Cube") {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO3);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, p));
		glEnableVertexAttribArray(positionAttribute);

		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, n));
		glEnableVertexAttribArray(normalAttribute);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO3);

	}
	if (type == "Object3D") {
	
		
		glUniform1i(diffuseTextureUniformLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);

		glUniform1i(specularTextureUniformLocation, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture);
		
		glUniform1i(normalTextureUniformLocation, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normalTexture);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBO4);
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, p));
		glEnableVertexAttribArray(positionAttribute);

		glEnableVertexAttribArray(texCoordAttribute);
		glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, t));

		glEnableVertexAttribArray(binormalAttribute);
		glVertexAttribPointer(binormalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, b));
		
		glEnableVertexAttribArray(tangentAttribute);
		glVertexAttribPointer(tangentAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, tg));

		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPNTBTG), (void*)offsetof(VertexPNTBTG, n));
		glEnableVertexAttribArray(normalAttribute);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO4);


	}
		glDrawElements(GL_TRIANGLES, sizeof(VertexPNTBTG) * 100000, GL_UNSIGNED_SHORT, 0);
	}
};

class Entity {
private:
	Quat q1, q2, q3;
	Quat combined;

public:
	Geometry geometry;
	Cvec3 t;
	Cvec3 r;
	Cvec3 s;

	Entity *parent;
	Matrix4 modelMatrix;

	Entity() {
		s = (1.0, 1.0, 1.0);
	}


	void createMatrix() {

		q1 = Quat::makeXRotation(r[0]);
		q2 = Quat::makeYRotation(r[1]);
		q3 = Quat::makeZRotation(r[2]);

		combined = q1 * q2 * q3;

		Matrix4 tempRMatrix;
		tempRMatrix = quatToMatrix(combined);

		Matrix4 tempTMatrix;
		tempTMatrix = tempTMatrix.makeTranslation(t);

		Matrix4 tempSMatrix;
		tempSMatrix = tempSMatrix.makeScale(s);
		if (this->parent == NULL) {
			modelMatrix = tempTMatrix * tempRMatrix * tempSMatrix;
		}
		else {
			modelMatrix = parent->modelMatrix * tempTMatrix * tempRMatrix * tempSMatrix;
		}
	}
	
	void Draw(Matrix4 eyeMatrix, Matrix4 projectionMatrix, GLuint modelViewMatrixUniformLocation, GLuint projectionMatrixUniformLocation, GLuint normalMatrixUniformLocation, string type)
	{
		createMatrix();
		
		Matrix4 modelViewMatrix = inv(eyeMatrix) * modelMatrix;
		GLfloat glmatrix[16];
		modelViewMatrix.writeToColumnMajorMatrix(glmatrix);
		glUniformMatrix4fv(modelViewMatrixUniformLocation, 1, false, glmatrix);

		GLfloat glmatrixProjection[16];
		projectionMatrix.writeToColumnMajorMatrix(glmatrixProjection);
		glUniformMatrix4fv(projectionMatrixUniformLocation, 1, false, glmatrixProjection);

		Matrix4 normalMatrixtemp = normalMatrix(modelMatrix);
		GLfloat glMatrixANormal[16];
		normalMatrixtemp.writeToColumnMajorMatrix(glMatrixANormal);
		glUniformMatrix4fv(normalMatrixUniformLocation, 1, false, glMatrixANormal);
		

		geometry.Draw(type);
	}
};



void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	int timeStart = glutGet(GLUT_ELAPSED_TIME);
	
	//EyeMatrix
	Matrix4 eyeMatrix;
	Matrix4 tempTmatrix;
	tempTmatrix = tempTmatrix.makeTranslation(Cvec3(0.0, 0.0, 35.0));
	Matrix4 tempTnegativematrix = tempTnegativematrix.makeTranslation(Cvec3(0.0, 0.0, -35.0));
	//eyeMatrix = eyeMatrix.makeTranslation(Cvec3(0.0, 0.0, 35.0));
	Quat rot = Quat::makeYRotation(30.0);
	Matrix4 tempRmatrix = quatToMatrix(rot);
	eyeMatrix = tempTmatrix;
	

	//Projection Matrix
	Matrix4 projectionMatrix;
	projectionMatrix = projectionMatrix.makeProjection(45.0, 1.0, -0.1, -100.0);
	
	Entity Object3D;
	Object3D.t = Cvec3(0.0, -3.0, 4.0);
	Object3D.r = Cvec3(0.0, 0.0, 0.0);
	Object3D.s = Cvec3(1.0, 1.0, 1.0);
	Object3D.parent = NULL;
	Object3D.Draw(eyeMatrix, projectionMatrix, modelViewMatrixUniformLocation, projectionMatrixUniformLocation, normalMatrixUniformLocation, "Object3D");

	
	Entity matrixA;
	matrixA.t = Cvec3(0.0, -4.0, 0.0);
	matrixA.r = Cvec3(0.0, 0.0, 0.0);
	matrixA.s = Cvec3(20.0, 20.0, 20.0);
	matrixA.parent = NULL;
	matrixA.Draw(eyeMatrix, projectionMatrix, modelViewMatrixUniformLocation, projectionMatrixUniformLocation, normalMatrixUniformLocation, "Plane");
	/*
	Entity objectB;
	objectB.t = Cvec3(-2.0, 2.0, 0.0);
	objectB.parent = &matrixA;
	objectB.Draw(eyeMatrix, projectionMatrix, modelViewMatrixUniformLocation, projectionMatrixUniformLocation, normalMatrixUniformLocation, "Cube");
	
	Entity objectC;
	objectC.t = Cvec3(-3.0, 4.0, 0.0);du
	objectC.parent = NULL;
	objectC.Draw(eyeMatrix, projectionMatrix, modelViewMatrixUniformLocation, projectionMatrixUniformLocation, normalMatrixUniformLocation, "Sphere");
	*/

	glUniform3f(lightColorUniform, 1.0, 0.0, 0.0);
	glUniform3f(specularLightColorUniform, 0.0, 0.0, 1.0);
	Cvec4 lightDirection = Cvec4(-0.6447, 0.6447, 0.6447, 0);
	lightDirection = normalMatrix(eyeMatrix) * lightDirection;
	glUniform3f(lightDirectionUniform, lightDirection[0], lightDirection[1], lightDirection[2]);

	glUniform3f(lightColorUniform1, 0.0, 1.0, 0.0);
	glUniform3f(specularLightColorUniform1, 0.0, 0.0, 1.0);
	Cvec4 lightDirection1 = Cvec4(0.6447, 0.6447, 0.6447, 0);
	lightDirection1 = normalMatrix(eyeMatrix) * lightDirection1;
	glUniform3f(lightDirectionUniform1, lightDirection1[0], lightDirection1[1], lightDirection1[2]);


	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(colorAttribute);
	glDisableVertexAttribArray(normalAttribute);

	glutSwapBuffers();
}

void SphereGenerator() {
	int ibLen, vbLen;
	getSphereVbIbLen(8, 8, vbLen, ibLen);
	vector<VertexPNTBTG> vtx(vbLen);
	vector<unsigned short> idx(ibLen);
	makeSphere(2, 8, 8, vtx.begin(), idx.begin());

	// fill our arrays
	glGenBuffers(1, &vertexBO1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPNTBTG) * vtx.size(), vtx.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &indexBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idx.size(), idx.data(), GL_STATIC_DRAW);
}

void PlaneGenerator() {
	int ibLen, vbLen;
	getPlaneVbIbLen(vbLen, ibLen);
	vector<VertexPNTBTG> vtx(vbLen);
	vector<unsigned short> idx(ibLen);
	makePlane(4, vtx.begin(), idx.begin());

	// fill our arrays
	glGenBuffers(1, &vertexBO2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPNTBTG) * vtx.size(), vtx.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &indexBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idx.size(), idx.data(), GL_STATIC_DRAW);
}

void CubeGenerator() {
	int ibLen, vbLen;
	getCubeVbIbLen(vbLen, ibLen);
	vector<VertexPNTBTG> vtx(vbLen);
	vector<unsigned short> idx(ibLen);
	makeCube(3, vtx.begin(), idx.begin());

	// fill our arrays
	glGenBuffers(1, &vertexBO3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPNTBTG) * vtx.size(), vtx.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &indexBO3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idx.size(), idx.data(), GL_STATIC_DRAW);
}

void init() {
	glClearDepth(0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glReadBuffer(GL_BACK);


	glClearColor(0.2, 0.2, 0.2, 0.0);
	program = glCreateProgram();
	readAndCompileShader(program, "vertex.glsl", "fragment.glsl");

	glUseProgram(program);
	positionAttribute = glGetAttribLocation(program, "position");
	colorAttribute = glGetAttribLocation(program, "color");
	modelViewMatrixUniformLocation = glGetUniformLocation(program, "modelViewMatrix");
	projectionMatrixUniformLocation = glGetUniformLocation(program, "projectionMatrix");
	positionUniform = glGetUniformLocation(program, "modelPosition");
	

	normalMatrixUniformLocation = glGetUniformLocation(program, "normalMatrix");
	normalAttribute = glGetAttribLocation(program, "normal");
	
	texCoordAttribute = glGetAttribLocation(program, "texCoord");
	binormalAttribute = glGetAttribLocation(program, "binormal");
	tangentAttribute = glGetAttribLocation(program, "tangent");

	diffuseTexture = loadGLTexture("Monk_Giveaway/Monk_D.tga");
	diffuseTextureUniformLocation = glGetUniformLocation(program, "diffuseTexture");

	specularTexture = loadGLTexture("Monk_Giveaway/Monk_S.tga");
	specularTextureUniformLocation = glGetUniformLocation(program, "specularTexture");

	normalTexture = loadGLTexture("Monk_Giveaway/Monk_N_Normal_Bump.tga");
	normalTextureUniformLocation = glGetUniformLocation(program, "normalTexture");


	lightDirectionUniform = glGetUniformLocation(program, "lights[0].lightDirection");
	lightColorUniform = glGetUniformLocation(program, "lights[0].lightColor");
	specularLightColorUniform = glGetUniformLocation(program, "lights[0].specularLightColor");

	lightDirectionUniform1 = glGetUniformLocation(program, "lights[1].lightDirection");
	lightColorUniform1 = glGetUniformLocation(program, "lights[1].lightColor");
	specularLightColorUniform1 = glGetUniformLocation(program, "lights[1].specularLightColor");


	CubeGenerator();
	SphereGenerator();
	PlaneGenerator();


}

void calculateFaceTangent(const Cvec3f &v1, const Cvec3f &v2, const Cvec3f &v3, const Cvec2f &texCoord1, const Cvec2f &texCoord2,
	const Cvec2f &texCoord3, Cvec3f &tangent, Cvec3f &binormal) {
	Cvec3f side0 = v1 - v2;
	Cvec3f side1 = v3 - v1;
	Cvec3f normal = cross(side1, side0);
	normalize(normal);
	float deltaV0 = texCoord1[1] - texCoord2[1];
	float deltaV1 = texCoord3[1] - texCoord1[1];
	tangent = side0 * deltaV1 - side1 * deltaV0;
	normalize(tangent);
	float deltaU0 = texCoord1[0] - texCoord2[0];
	float deltaU1 = texCoord3[0] - texCoord1[0];
	binormal = side0 * deltaU1 - side1 * deltaU0;
	normalize(binormal);
	Cvec3f tangentCross = cross(tangent, binormal);
	if (dot(tangentCross, normal) < 0.0f) {
		tangent = tangent * -1;
	}
}

static void PrintInfo(const tinyobj::attrib_t& attrib,
	const std::vector<tinyobj::shape_t>& shapes,
	const std::vector<tinyobj::material_t>& materials) {
	std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
	std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
	std::cout << "# of texture coordinates : " << (attrib.texcoords.size() / 2) << std::endl;
	vector<VertexPNTBTG> vtx;
	vector<unsigned short> idx;
	/*
	for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
		printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.vertices[3 * v + 0]),
			static_cast<const double>(attrib.vertices[3 * v + 1]),
			static_cast<const double>(attrib.vertices[3 * v + 2]));
	}

	for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
		printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.normals[3 * v + 0]),
			static_cast<const double>(attrib.normals[3 * v + 1]),
			static_cast<const double>(attrib.normals[3 * v + 2]));
	}
	*/
	/*
	for (int i = 0; i < attrib.vertices.size(); i += 3) {
		VertexPNTBTG v;
		v.p[0] = attrib.vertices[i];
		v.p[1] = attrib.vertices[i + 1];
		v.p[2] = attrib.vertices[i + 2];
		v.n[0] = attrib.normals[i];
		v.n[1] = attrib.normals[i + 1];
		v.n[2] = attrib.normals[i + 2];
		v.t[0] = attrib.texcoords[i];
		v.t[1] = 1.0 - attrib.texcoords[i + 1];
		vtx.push_back(v);
	}
	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
			idx.push_back(shapes[i].mesh.indices[j].vertex_index);
		}
	}
	*/
	
	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
			unsigned int vertexOffset = shapes[i].mesh.indices[j].vertex_index * 3;
			unsigned int normalOffset = shapes[i].mesh.indices[j].normal_index * 3;
			unsigned int texOffset = shapes[i].mesh.indices[j].texcoord_index * 2;
			VertexPNTBTG v;
			v.p[0] = attrib.vertices[vertexOffset];
			v.p[1] = attrib.vertices[vertexOffset + 1];
			v.p[2] = attrib.vertices[vertexOffset + 2];
			v.n[0] = attrib.normals[normalOffset];
			v.n[1] = attrib.normals[normalOffset + 1];
			v.n[2] = attrib.normals[normalOffset + 2];
			v.t[0] = attrib.texcoords[texOffset];
			v.t[1] = 1.0 - attrib.texcoords[texOffset + 1];
			vtx.push_back(v);
			idx.push_back(vtx.size()-1);
		}
	}
	
	for (int i = 0; i < vtx.size(); i += 3) {
		Cvec3f tangent;
		Cvec3f binormal;
		calculateFaceTangent(vtx[i].p, vtx[i + 1].p, vtx[i + 2].p,
			vtx[i].t, vtx[i + 1].t, vtx[i + 2].t, tangent, binormal);
		vtx[i].tg = tangent;
		vtx[i + 1].tg = tangent;
		vtx[i + 2].tg = tangent;
		vtx[i].b = binormal;
		vtx[i + 1].b = binormal;
		vtx[i + 2].b = binormal;
	}

	glGenBuffers(1, &vertexBO4);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPNTBTG) * vtx.size(), vtx.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &indexBO4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * idx.size(), idx.data(), GL_STATIC_DRAW);
	cout << "Buffer is loaded now!!" << endl;

}


static bool TestLoadObj(const char* filename, const char* basepath = NULL,
	bool triangulate = true) {
	std::cout << "Loading " << filename << std::endl;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;


	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename,
		basepath, triangulate);


	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return false;
	}

	PrintInfo(attrib, shapes, materials);

	return true;
}


void reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void idle(void) {
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("CS - 6533");

	glewInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	cout << "Coming overhere!!";
	assert(true == TestLoadObj("Monk_Giveaway/Monk_Giveaway_Fixed.obj", "/", false));
	init();
	glutMainLoop();
	

	return 0;
}