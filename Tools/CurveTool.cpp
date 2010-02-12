#include <Ogre.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>

using namespace Ogre;
using namespace std;


#define PI 3.14159265f
#define SEGMENT_SIZE 0.5f
#define SEGMENT_ANGLE 5.0f
#define SEGMENT_STEPS 64
#define SEGMENT_RADIUS 10.0f


Matrix4 trans = Matrix4::IDENTITY;
ManualObject* manual;
int segments = 0;
Vector3 lastSpinePosition;
int v = 0;


void generateRing() {
	Vector3 spinePosition = trans * Vector3::ZERO;
	Vector3 spineForward = trans * Vector3::UNIT_Z;
	Vector3 spineUp = trans * Vector3::UNIT_Y;

    if (segments != 0) {
        v += lastSpinePosition.distance(spinePosition);
    }
    lastSpinePosition = spinePosition;

	// Todo: write spine to a file
	for (int i = 0; i < SEGMENT_STEPS; i++) {
		float theta = 360.0f/SEGMENT_STEPS*i;
        float u = i; //(float)i/(float)SEGMENT_STEPS * 2;
		Vector3 position(cosf(PI/180.0*theta)*SEGMENT_RADIUS, sinf(PI/180.0*theta)*SEGMENT_RADIUS, 0.0f);
		Vector4 normalh(-position.normalisedCopy());
		normalh.w = 0;

		position = trans * position;
		normalh = trans * normalh;

		Vector3 normal(normalh.x, normalh.y, normalh.z);
		// Add to mesh object here
		
		manual->position(position);
        manual->normal(normal);
        manual->textureCoord(u, v / (2 * PI * SEGMENT_RADIUS));
		//manual->textureCoord(vertices[i].u, vertices[i].v);
		//manual->index(i);
	}
	segments++;
}

void generateStraight(float length) {
	generateRing();
	trans = trans.concatenate(Matrix4::getTrans(0, 0, length));
}

void generateCurve(const Vector3& normal, float angle, float radius) {
	for (float t = 0; t < angle; t += SEGMENT_ANGLE) {
		generateRing();
		Vector3 right = normal.crossProduct(Vector3::UNIT_Z);
		right.normalise();

		trans = trans.concatenate(Matrix4::getTrans(radius * right));
		trans = trans.concatenate(Matrix4(Quaternion(Degree(SEGMENT_ANGLE), normal)));
	    trans = trans.concatenate(Matrix4::getTrans(-radius * right));	
	}
}

void generateIndices() {

	for (int i = 0; i < segments-1; i++) {
		for (int j = 0; j <= SEGMENT_STEPS; j++) {

			    manual->index(j % SEGMENT_STEPS + i * SEGMENT_STEPS);
			    manual->index(j % SEGMENT_STEPS + (i+1) * SEGMENT_STEPS);

		}
	}
}

int main(int argc, char** argv) {
	Root* root = new Root("plugins.cfg", "ogre.cfg", "curve.log");
	root->showConfigDialog();
	root->initialise(true);
	SceneManager* sceneManager = root->createSceneManager(ST_INTERIOR, "Default");

	manual = sceneManager->createManualObject("Test");
	manual->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

	generateCurve(Vector3::UNIT_Y, 45, 200);
	generateStraight(100);
	generateCurve(-Vector3::UNIT_Y, 90, 200);
	generateStraight(100);
	generateStraight(100);
	generateCurve(Vector3::UNIT_Y, 90, 200);

	
	generateIndices();

	manual->end();


	Ogre::MeshPtr mesh = manual->convertToMesh("Test");
	Ogre::MeshSerializer* serializer = new Ogre::MeshSerializer();
	serializer->exportMesh(mesh.get(), "Test.mesh");

	/*if (argc < 2) {
		cout << "Usage: " << argv[0] << " points.txt" << endl;
		return 1;
	}
	
	ifstream in(argv[1]);
	if (!in.is_open()) {
		cout << "Unable to open file" << endl;
		return 1;
	}

	while (!in.eof()) {
		string command;
		in >> command;

		if (command == "straight") {
			float length;
			in >> length;
			straight(length);
		} else if (command == "curve") {
			Vector3 normal;
			float angle;
			float radius;
			in >> normal.x >> normal.y >> normal.z;
			in >> angle >> radius;
			curve(normal, angle, radius);
		}
	}*/
}