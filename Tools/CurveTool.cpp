#include <Ogre.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <memory>
#include <sstream>
#include <OgreDefaultHardwareBufferManager.h>

using namespace Ogre;
using namespace std;


#define PI 3.14159265f
    
class CurveTool {
public:

    CurveTool(const std::string input, const std::string output, float step, int divisions, float radius) : 
        curveStep(step),
        ringDivisions(divisions),
        ringRadius(radius),
        transform(Matrix4::IDENTITY),
        manual(output),
        rings(0),
        v(0),
        input(input), 
        output(output),
        out((output + ".spine").c_str()),
        vertices(0),
        triangles(0),
        indices(0) {

	    manual.begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

        readInputFile();
    }

    ~CurveTool() {
        generateIndices();
        manual.end();
        Ogre::MeshPtr mesh = manual.convertToMesh(output);
	    Ogre::MeshSerializer serializer;
	    serializer.exportMesh(mesh.get(), output + ".mesh");

        cout << "Vertices: " << vertices << endl;
        cout << "Indices: " << indices << endl;
        cout << "Triangles: " << triangles << endl;
    }

private:
    void readInputFile() {
        // Create the first ring
        generateStraight(0);

        // Read the input file
        std::ifstream in(input.c_str());

        if (!in.is_open()) throw runtime_error("Unable to read file");
        while (!in.eof()) {
            std::string type;
            in >> type;
            
            if (type == "straight") {
                float length;
                in >> length;
                generateStraight(length);
            } else if (type == "curve") {
                float nx, ny, nz, angle, radius;
                in >> nx >> ny >> nz >> angle >> radius;
                angle = abs(angle);
                generateCurve(Vector3(nx, ny, nz), angle, radius);
            }

            if (in.fail()) throw runtime_error("Unable to read file");
        }
    }

    /** Generates one ring around the current spine point using the transform */
    void generateRing() {
	    Vector3 spinePosition = transform * Vector3::ZERO;
	    Vector3 spineForward = transform * Vector3::UNIT_Z;
	    Vector3 spineUp = transform * Vector3::UNIT_Y;

        if (rings != 0) {
            v += lastSpinePosition.distance(spinePosition);
        }
        lastSpinePosition = spinePosition;

        out << "position: " << spinePosition.x << " " << spinePosition.y << " " << spinePosition.z << endl;
        out << "forward: " << spineForward.x << " " << spineForward.y << " " << spineForward.z  << endl;
        out << "up: " << spineUp.x << " " << spineUp.y << " " << spineUp.z << endl;

	    // Todo: write spine to a file
	    for (int i = 0; i < ringDivisions; i++) {
		    float theta = 360.0f/(ringDivisions-1)*i + 45.0f;
            float u = (float)i/(float)ringDivisions * 2;
		    Vector3 position(cosf(PI/180.0*theta)*ringRadius, sinf(PI/180.0*theta)*ringRadius, 0.0f);
		    Vector4 normalh(-position.normalisedCopy());
		    normalh.w = 0;

		    position = transform * position;
		    normalh = transform * normalh;

		    Vector3 normal(normalh.x, normalh.y, normalh.z);
    		
		    manual.position(position);
            manual.normal(normal);
            manual.textureCoord(u, v / (2 * PI * ringRadius));
            vertices++;
	    }
	    rings++;
    }

    /** Generates a straight tube segment */
    void generateStraight(float length) {
	    transform = transform.concatenate(Matrix4::getTrans(0, 0, length));
        generateRing();
    }

    /** Generates a curved tube segment */
    void generateCurve(const Vector3& normal, float angle, float radius) {
	    for (float t = 0; t < angle; t += curveStep) {
		    Vector3 right = normal.crossProduct(Vector3::UNIT_Z);
		    right.normalise();

		    transform = transform.concatenate(Matrix4::getTrans(radius * right));
		    transform = transform.concatenate(Matrix4(Quaternion(Degree(curveStep), normal)));
	        transform = transform.concatenate(Matrix4::getTrans(-radius * right));	
	        generateRing();
        }
    }

    /** Generates the indices for the mesh */
    void generateIndices() {
	    for (int i = 0; i < rings-1; i++) {
		    for (int j = 0; j <= ringDivisions; j++) {
			        manual.index(j % ringDivisions + i * ringDivisions);
			        manual.index(j % ringDivisions + (i+1) * ringDivisions);
                    indices += 2;
                    triangles += 2;
		    }
	    }

        // This accounts for the first two indices, which don't 
        // create unique triangles
        triangles -= 2; 
    }

    /** Current transform for the spine */
    Matrix4 transform;

    /** OGRE manual object used to build the mesh */
    ManualObject manual;

    /** Total number of rings in the curve */
    int rings;

    /** Last spine location, used for wrapping the v texcoord */
    Vector3 lastSpinePosition;

    /** V texcoord */
    int v;

    /** Name of the output file */
    std::string output;

    /** Name of the input file */
    std::string input;

    /** File stream */
    std::ofstream out;
            
    /** Mesh generation parameters */
    const float curveStep;
    const int ringDivisions;
    const float ringRadius;

    /** Statistics  */
    int vertices;
    int triangles;
    int indices;
};



int main(int argc, char** argv) {

    if (argc >=2 && string(argv[1]) == "/?" || string(argv[1]) == "--help" || string(argv[1]) == "-h") { 
        cout << "Usage: " << argv[0] << " <input file> <output file> <options>" << endl;
        cout << "    --curve-step: degrees between each ring on a curve" << endl;
        cout << "    --ring-divisions: number of vertices per ring" << endl;
        cout << "    --ring-radius: radius of the ring in meters" << endl;
        return 0;
	}

    // Hack to get Ogre to load without the render system
    try {
		if (argc < 3) throw runtime_error(std::string("Usage: ") + argv[0] + " <input file> <output file> <options>");
        std::auto_ptr<LogManager> logManager(new LogManager());
        logManager->createLog("curve.log", false, false);
        std::auto_ptr<DefaultHardwareBufferManager> bufferManager(new DefaultHardwareBufferManager());
        std::auto_ptr<ResourceGroupManager> resourceManager(new ResourceGroupManager());
        std::auto_ptr<MeshManager> meshManager(new MeshManager());
               

        float curveStep = 5.0f;
        int ringDivisions = 32;
        float ringRadius = 10.0f;

        for (int i = 4; i < argc; i += 2) {
            if (string(argv[i-1]) == "--curve-step") {
                stringstream ss(argv[i]);
                ss >> curveStep;
            } else if (string(argv[i-1]) == "--ring-divisions") {
                stringstream ss(argv[i]);
                ss >> ringDivisions;
            } else if (string(argv[i-1]) == "--ring-radius") {
                stringstream ss(argv[i]);
                ss >> ringRadius;
            }
        }

        CurveTool tool(argv[1], argv[2], curveStep, ringDivisions+1, ringRadius);
    } catch (Exception& ex) {
        cout << "Ogre initialization error: " << ex.getDescription() << endl;
        return -1;
    } catch (std::exception& ex) {
        cout << ex.what();
        return -1;
    }
    return 0;
}