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
                     "Return value is 0 if OK, 1 non-manifold vertex found, 2 non-manifold edge found, 3 if both, -1 for file not found/loading error" << std::endl;
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

    // Perform the actual check
    int NonManifoldEdges = vcg::tri::Clean<Mesh>::CountNonManifoldEdgeFF(stl);
    int NonManifoldVertices = vcg::tri::Clean<Mesh>::CountNonManifoldVertexFF(stl);

    std::cout << "Model has " << NonManifoldEdges << " non-manifold edges and "
              << NonManifoldVertices << " non-manifold vertices" << std::endl;

}
