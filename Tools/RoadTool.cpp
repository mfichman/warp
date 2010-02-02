#include <Ogre.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <OgreTerrainSceneManager.h>

using namespace std;

#define HEIGHTOFFSET 0.35 // meters
#define ROADWIDTH 9.0f // meters
//#define SMOOTHING 0.1f
#define WORLDSCALING 1.46484375f // 
#define OBJSCALING 1.0f

vector<Ogre::Vector3> points;
Ogre::Root* root;
Ogre::TerrainSceneManager* sceneManager;
Ogre::ManualObject* manual;


struct Vertex {
	Vertex(const Ogre::Vector3 pos, float u, float v) : p(pos), u(u), v(v), n(0, 0, 0) {}
	Ogre::Vector3 p;
	Ogre::Vector3 n;
	float u;
	float v;
};


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

Ogre::Vector3 normal(Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3, bool even) {
	if (even) {
		return ((p2 - p1).crossProduct(p2 - p3)).normalisedCopy();
	} else {
		return -((p2 - p1).crossProduct(p2 - p3)).normalisedCopy();
	}
	
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

void ComputeNormals(vector<Vertex>& vertices) {
	for (size_t i = 2; i < vertices.size(); i++) {
		Ogre::Vector3& p1 = vertices[i-2].p;
		Ogre::Vector3& p2 = vertices[i-1].p;
		Ogre::Vector3& p3 = vertices[i-0].p;

		Ogre::Vector3 n = normal(p1, p2, p3, (i%2) != 0);
		vertices[i-2].n += n;
		vertices[i-1].n += n;
		vertices[i-0].n += n;
	}
}

void ComputeVertices() {

	manual->begin("Road", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

	const Ogre::Vector3 up(0.0, 1.0, 0.0);

	// Create road segments
	//float hprev = -1.0f;
	float u = 0.0f;
	vector<Vertex> vertices;
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

		vertices.push_back(Vertex(Ogre::Vector3(p1.x, h, p1.z), u, 0.0f));
		vertices.push_back(Vertex(Ogre::Vector3(p2.x, h, p2.z), u, 1.0f));

		u += points[i-1].distance(points[i-2]);
	}

	// Create the object
#define SMOOTHFACTOR 20
	vector<Vertex> temp;
	for (size_t i = 0; i < vertices.size(); i++) {

		size_t start = max(0U, i-SMOOTHFACTOR);
		size_t end = min(vertices.size(), i+SMOOTHFACTOR+1);
		float y = 0.0f;
		for (size_t j = start; j < end; j++) {
			y += vertices[j].p.y;
		}
		y /= (end-start);
		
		temp.push_back(Vertex(
			Ogre::Vector3(vertices[i].p.x, y, vertices[i].p.z), 
			vertices[i].v, 
			vertices[i].u/ROADWIDTH));

	}
	vertices = temp;


	// Set normals
	ComputeNormals(vertices);

	// Add vertices to manual object
	for (size_t i = 0; i < vertices.size(); i++) {
		manual->position(vertices[i].p);
		manual->normal(vertices[i].n.normalisedCopy());
		manual->textureCoord(vertices[i].u, vertices[i].v);
	}

	// Calculate index buffer
	for (size_t i = 0; i < vertices.size(); i++) {
		manual->index(i);
	}

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