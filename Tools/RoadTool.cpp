#include <Ogre.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <OgreTerrainSceneManager.h>

using namespace std;

#define HEIGHTOFFSET 1.2f // meters
#define ROADWIDTH 9.0f // meters
#define SMOOTHING 0.8f
#define WORLDSCALING 1.46484375f // 
#define OBJSCALING 1.0f

vector<Ogre::Vector3> points;
Ogre::Root* root;
Ogre::TerrainSceneManager* sceneManager;
Ogre::ManualObject* manual;

void ReadFile(char* file) {
	ifstream in(file);
	if (!in.is_open()) {
		cout << "Unable to open file" << endl;
		exit(1);
	}

	//for (float x = 0; x < 3000; x++) {
	//	points.push_back(Ogre::Vector3(x, 0, x));
	//}

	while (!in.eof()) {
		float x, z;
		in >> x >> z;
		if (in.fail()) {
			cout << "Could not read all points!" << endl;
			break;
		}
		x *= WORLDSCALING;
		z *= WORLDSCALING;

		points.push_back(Ogre::Vector3(x, 0, z));
	}

	cout << points.size() << " points read." << endl;
}

void LoadTerrain() {
	root = new Ogre::Root("plugins.cfg", "ogre.cfg", "road.log");
	root->showConfigDialog();
	root->initialise(true);

	// Load resources
	Ogre::ConfigFile config;
	config.load("resources.cfg");
	Ogre::ConfigFile::SectionIterator s = config.getSectionIterator();
	while (s.hasMoreElements()) {
		Ogre::String section = s.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap* settings = s.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i) {
			Ogre::String type = i->first;
			Ogre::String arch = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch, type, section);
		} 
	} 

	// Create scene manager
	sceneManager = static_cast<Ogre::TerrainSceneManager*>(root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "Default"));
	sceneManager->setWorldGeometry("terrain.cfg");

	manual = sceneManager->createManualObject("Manual");
	
}

void ComputeVertices() {

	manual->begin("Road", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	Ogre::Vector3 up(0.0, 1.0, 0.0);

	float u = 0.0f;
	float hprev = -1.0f;
	for (size_t i = 2; i < points.size(); i++) {
		Ogre::Vector3 v1 = points[i-1] - points[i-2];
		Ogre::Vector3 v2 = points[i] - points[i-1];
		Ogre::Vector3 n1 = up.crossProduct(v1);
		Ogre::Vector3 n2 = up.crossProduct(v2);
		Ogre::Vector3 n = ((n1 + n2) / 2.0f);
		n.normalise();

		Ogre::Vector3 p1 = points[i-1] + (ROADWIDTH/2) * n;
		Ogre::Vector3 p2 = points[i-1] - (ROADWIDTH/2) * n;

		float h1 = sceneManager->getHeightAt(p1.x, p1.z);
		float h2 = sceneManager->getHeightAt(p2.x, p2.z);

		float h = max(h1, h2) + HEIGHTOFFSET;
		if (hprev != 1.0f) {
			h = SMOOTHING*h + (1-SMOOTHING)*hprev;
		}
		hprev = h;

		
		manual->position(OBJSCALING * Ogre::Vector3(p1.x, h, p1.z));
		manual->normal(up);
		manual->textureCoord(0.0f, u/ROADWIDTH);
		manual->position(OBJSCALING * Ogre::Vector3(p2.x, h, p2.z));
		manual->normal(up);
		manual->textureCoord(1.0f, u/ROADWIDTH);

		manual->position(OBJSCALING * Ogre::Vector3(p1.x, 0, p1.z));
		manual->normal(n1);
		manual->textureCoord(0.0f, 0.0f);
		manual->position(OBJSCALING * Ogre::Vector3(p2.x, 0, p2.z));
		manual->normal(n2);
		manual->textureCoord(0.0f, 0.0f);

		cout << "X: " << points[i].x << endl;
		cout << "Z: " << points[i].z << endl;

		u += points[i-1].distance(points[i-2]);

		cout << i << endl;


	}

	int indices = 0;
	for (size_t i = 7; i < points.size(); i += 4) {
		//manual->triangle(i-3, i-2, i);
		//manual->triangle(i, i-1, i-3);

		// Top
		manual->triangle(i-7, i-6, i-2);
		manual->triangle(i-2, i-3, i-7);

		// Right
		manual->triangle(i, i-6, i-4);
		manual->triangle(i, i-2, i-6);

		// Left
		manual->triangle(i-1, i-5, i-7);
		manual->triangle(i-1, i-7, i-3);
	}

	cout << "INDICIES: " << indices << endl;

	manual->end();
}


void SerializeMesh() {
	
	Ogre::MeshPtr mesh = manual->convertToMesh("Road");
	Ogre::MeshSerializer* serializer = new Ogre::MeshSerializer();
	serializer->exportMesh(mesh.get(), "Road.mesh");


}

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " points.txt" << endl;
		return 0;
	}

	try {
		ReadFile(argv[1]);
		LoadTerrain();
		ComputeVertices();
		SerializeMesh();
	} catch (Ogre::Exception ex) {
		cout << ex.getFullDescription() << endl;
	}



	return 0;
}