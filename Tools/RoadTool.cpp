#include <Ogre.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <OgreTerrainSceneManager.h>

using namespace std;

#define HEIGHTOFFSET 0.4 // meters
#define ROADWIDTH 9.0f // meters
//#define SMOOTHING 0.1f
#define WORLDSCALING 1.46484375f // 
#define OBJSCALING 1.0f

vector<Ogre::Vector3> points;
Ogre::Root* root;
Ogre::TerrainSceneManager* sceneManager;


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

void SerializeMesh(vector<Vertex>& vertices, const string& name, const string& mat) {
	ComputeNormals(vertices);

	Ogre::ManualObject* manual = sceneManager->createManualObject(name);
	manual->begin(mat, Ogre::RenderOperation::OT_TRIANGLE_STRIP);

		// Add vertices to manual object
	for (size_t i = 0; i < vertices.size(); i++) {
		manual->position(vertices[i].p);
		manual->normal(vertices[i].n.normalisedCopy());
		manual->textureCoord(vertices[i].u, vertices[i].v);
		manual->index(i);
	}

	manual->end();

	Ogre::MeshPtr mesh = manual->convertToMesh(name);
	Ogre::MeshSerializer* serializer = new Ogre::MeshSerializer();
	serializer->exportMesh(mesh.get(), name + ".mesh");
}

void ComputeVertices() {

	const Ogre::Vector3 up(0.0, 1.0, 0.0);

	// Create road segments
	//float hprev = -1.0f;
	float v = 0.0f;
	vector<Vertex> surface;
	vector<Ogre::Vector3> normals;
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

		surface.push_back(Vertex(Ogre::Vector3(p1.x, h, p1.z), 0.0f, v/ROADWIDTH));
		surface.push_back(Vertex(Ogre::Vector3(p2.x, h, p2.z), 1.0f, v/ROADWIDTH));

		normals.push_back(n);

		v += points[i-1].distance(points[i-2]);
	}

	// Create the object
#define SMOOTHFACTOR 20
	vector<Vertex> temp;
	vector<Vertex> left;
	vector<Vertex> right;
	for (size_t i = 0; i < surface.size(); i++) {

		size_t start = max(0U, i-SMOOTHFACTOR);
		size_t end = min(surface.size(), i+SMOOTHFACTOR+1);
		float y = 0.0f;
		for (size_t j = start; j < end; j++) {
			y += surface[j].p.y;
		}
		y /= (end-start);
		
		temp.push_back(Vertex(Ogre::Vector3(surface[i].p.x, y, surface[i].p.z), surface[i].u, surface[i].v));

		const Ogre::Vector3& normal = normals[i/2];

		if ((i % 2) == 0) {
			Ogre::Vector3 p1 = surface[i].p + (normal * (y));
			p1.y = 0;
			Ogre::Vector3 p2(surface[i].p.x, y, surface[i].p.z);
			float u = (p1-p2).length()/ROADWIDTH;

			left.push_back(Vertex(p1, u, surface[i].v));
			left.push_back(Vertex(p2, 0.0f, surface[i].v));
		} else {
			Ogre::Vector3 p1 = surface[i].p - (normal * (y));
			p1.y = 0;
			Ogre::Vector3 p2(surface[i].p.x, y, surface[i].p.z);
			float u = (p1-p2).length()/ROADWIDTH;

			right.push_back(Vertex(p2, 0.0f, surface[i].v));
			right.push_back(Vertex(p1, u, surface[i].v));
		}

	}
	surface = temp;

	SerializeMesh(surface, "Road", "Road");
	SerializeMesh(left, "LeftRoadEdge", "RoadSide");
	SerializeMesh(right, "RightRoadEdge", "RoadSide");



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
	} catch (Ogre::Exception ex) {
		cout << ex.getFullDescription() << endl;
	}

	return 0;
}