// Include mesh definitions from meshcleaner
#include "../mesh_definitions.hpp"

// STL IO headers
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/export_stl.h>

// Mesh cleanup algorithms
#include <vcg/complex/algorithms/clean.h>

enum errorcodes {
    ERROR_NONMANIFOLD_EDGE = 2,
    ERROR_HOLES = 4,
    ERROR_SELF_INTERSECT = 8
};

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: manifoldtest file.stl\n"
                     "Tests for and prints a number of problems that's known to cause problems with slicing" << std::endl;
        return -1;
    }

    Mesh stl;
    int errorcode = vcg::tri::io::ImporterSTL<Mesh>::Open(stl, argv[1]);
    if (errorcode != 0)
    {
        std::string error = vcg::tri::io::ImporterSTL<Mesh>::ErrorMsg(errorcode);
        std::cerr << "An error occured while loading " << argv[1] << ": " << error << std::endl;
        return -1;
    }

    // STL will load with duplicate vertices due to how the format is designed, so remove those
    vcg::tri::Clean<Mesh>::RemoveDuplicateVertex(stl);
    // Clean out duplicate faces if they exist
    vcg::tri::Clean<Mesh>::RemoveDuplicateFace(stl);
    // Remove any vertices that are unreferenced from any tri
    vcg::tri::Clean<Mesh>::RemoveUnreferencedVertex(stl);
    // Remove all faces with zero area
    vcg::tri::Clean<Mesh>::RemoveZeroAreaFace(stl);
    // Generate face to face topology data for manifoldness detection
    vcg::tri::UpdateTopology<Mesh>::FaceFace(stl);

    // Perform the actual checking
    int NonManifoldEdges = vcg::tri::Clean<Mesh>::CountNonManifoldEdgeFF(stl);
    int NonManifoldVertices = vcg::tri::Clean<Mesh>::CountNonManifoldVertexFF(stl);
    int Holes = vcg::tri::Clean<Mesh>::CountHoles(stl);
    std::vector<Mesh::FaceType *> SelfIntersectList;
    stl.face.EnableMark();
    vcg::tri::Clean<Mesh>::SelfIntersections(stl, SelfIntersectList);
    stl.face.DisableMark();
    int SelfIntersections = SelfIntersectList.size();

    std::cout << "Model has " << NonManifoldEdges << " non-manifold edges and "
              << NonManifoldVertices << " non-manifold vertices" << std::endl;
    std::cout << SelfIntersections << " self-intersecting faces and "
              << Holes << " holes" << std::endl;

}
