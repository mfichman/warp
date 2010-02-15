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
        curveStep_(step),
        ringDivisions_(divisions),
        ringRadius_(radius),
        transform_(Matrix4::IDENTITY),
        manual(output),
        rings_(0),
        v_(0),
        input(input), 
        output(output),
        out((output + ".spine").c_str()),
        vertices_(0),
        triangles_(0),
        indices_(0) {

	    manual.begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_STRIP);

        readInputFile();
    }

    ~CurveTool() {
        generateIndices();
        manual.end();
        Ogre::MeshPtr mesh = manual.convertToMesh(output);
	    Ogre::MeshSerializer serializer;
	    serializer.exportMesh(mesh.get(), output + ".mesh");

        cout << "Vertices: " << vertices_ << endl;
        cout << "Indices: " << indices_ << endl;
        cout << "Triangles: " << triangles_ << endl;
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

    /** Generates one ring around the current spine point using the transform_ */
    void generateRing() {
	    Vector3 spinePosition = transform_ * Vector3::ZERO;

        // Use 'normal'-style transformation, discard the w coordinate
	    Vector4 spineForward = transform_ * Vector4(0, 0, 1, 0); //Vector3::UNIT_Z;
	    Vector4 spineUp = transform_ * Vector4(0, 1, 0, 0); // Vector3::UNIT_Y;

        if (rings_ != 0) {
            v_ += lastSpinePosition.distance(spinePosition);
        }
        lastSpinePosition = spinePosition;

        out << "position: " << spinePosition.x << " " << spinePosition.y << " " << spinePosition.z << endl;
        out << "forward: " << spineForward.x << " " << spineForward.y << " " << spineForward.z  << endl;
        out << "up: " << spineUp.x << " " << spineUp.y << " " << spineUp.z << endl;

	    // Todo: write spine to a file
	    for (int i = 0; i < ringDivisions_; i++) {
		    float theta = 360.0f/(ringDivisions_-1)*i + 45.0f;
            float u = (float)i/(float)ringDivisions_ * 2;
		    Vector3 position(cosf(PI/180.0*theta)*ringRadius_, sinf(PI/180.0*theta)*ringRadius_, 0.0f);
		    Vector4 normalh(-position.normalisedCopy());
		    normalh.w = 0;

		    position = transform_ * position;
		    normalh = transform_ * normalh;

		    Vector3 normal(normalh.x, normalh.y, normalh.z);
    		
		    manual.position(position);
            manual.normal(normal);
            manual.textureCoord(u, v_ / (2 * PI * ringRadius_));
            vertices_++;
	    }
	    rings_++;
    }

    /** Generates a straight tube segment */
    void generateStraight(float length) {
	    transform_ = transform_.concatenate(Matrix4::getTrans(0, 0, length));
        generateRing();
    }

    /** Generates a curved tube segment */
    void generateCurve(const Vector3& normal, float angle, float radius) {
	    for (float t = 0; t < angle; t += curveStep_) {
		    Vector3 right = normal.crossProduct(Vector3::UNIT_Z);
		    right.normalise();

		    transform_ = transform_.concatenate(Matrix4::getTrans(radius * right));
		    transform_ = transform_.concatenate(Matrix4(Quaternion(Degree(curveStep_), normal)));
	        transform_ = transform_.concatenate(Matrix4::getTrans(-radius * right));	
	        generateRing();
        }
    }

    /** Generates the indices_ for the mesh */
    void generateIndices() {
	    for (int i = 0; i < rings_-1; i++) {
		    for (int j = 0; j <= ringDivisions_; j++) {
			        manual.index(j % ringDivisions_ + i * ringDivisions_);
			        manual.index(j % ringDivisions_ + (i+1) * ringDivisions_);
                    indices_ += 2;
                    triangles_ += 2;
		    }
	    }

        // This accounts for the first two indices_, which don't 
        // create unique triangles_
        triangles_ -= 2; 
    }

    /** Current transform_ for the spine */
    Matrix4 transform_;

    /** OGRE manual object used to build the mesh */
    ManualObject manual;

    /** Total number of rings_ in the curve */
    int rings_;

    /** Last spine location, used for wrapping the v_ texcoord */
    Vector3 lastSpinePosition;

    /** V texcoord */
    int v_;

    /** Name of the output file */
    std::string output;

    /** Name of the input file */
    std::string input;

    /** File stream */
    std::ofstream out;
            
    /** Mesh generation parameters */
    const float curveStep_;
    const int ringDivisions_;
    const float ringRadius_;

    /** Statistics  */
    int vertices_;
    int triangles_;
    int indices_;
};



int main(int argc, char** argv) {

    if (argc >=2 && string(argv[1]) == "/?" || string(argv[1]) == "--help" || string(argv[1]) == "-h") { 
        cout << "Usage: " << argv[0] << " <input file> <output file> <options>" << endl;
        cout << "    --curve-step: degrees between each ring on a curve" << endl;
        cout << "    --ring-divisions: number of vertices_ per ring" << endl;
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
               

        float curveStep_ = 5.0f;
        int ringDivisions_ = 32;
        float ringRadius_ = 10.0f;

        for (int i = 4; i < argc; i += 2) {
            if (string(argv[i-1]) == "--curve-step") {
                stringstream ss(argv[i]);
                ss >> curveStep_;
            } else if (string(argv[i-1]) == "--ring-divisions") {
                stringstream ss(argv[i]);
                ss >> ringDivisions_;
            } else if (string(argv[i-1]) == "--ring-radius") {
                stringstream ss(argv[i]);
                ss >> ringRadius_;
            }
        }

        CurveTool tool(argv[1], argv[2], curveStep_, ringDivisions_+1, ringRadius_);
    } catch (Exception& ex) {
        cout << "Ogre initialization error: " << ex.getDescription() << endl;
        return -1;
    } catch (std::exception& ex) {
        cout << ex.what();
        return -1;
    }
    return 0;
}