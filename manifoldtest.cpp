#include <string>
// Include mesh definitions from meshcleaner
#include "../mesh_definitions.hpp"

// STL IO headers
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/export_stl.h>

// Mesh cleanup algorithms
#include <vcg/complex/algorithms/clean.h>

bool getoption(int argc, char * argv[], std::string option)
{
    for(int i = 0; i < argc; ++i)
    {
        std::string temp = argv[i];
        if (temp.compare(option) == 0)
            return true;
    }
    return false;
}

enum errorcodes {
    ERROR_NONMANIFOLD_EDGE = 1,
    ERROR_HOLES = 4,
    ERROR_SELF_INTERSECT = 8
};

int main(int argc, char * argv[])
{
    bool quietmode = getoption(argc, argv, "-q");
    if((!quietmode && argc != 2) || (argc != 3 && quietmode))
    {
        std::cerr << "Usage:\nmanifoldtest file.stl\n"
                  << "OR \"manifoldtest -q file.stl\" and return status will be 0 if OK or a bitmask of any errors found\n"
                  << "Non-manifold edge = 1, Hole in mesh = 4, Self-intersections = 8\n"
                  << "manifoldtest tests for and prints a number of problems that's known to cause problems with slicing" << std::endl;
        return -1;
    }

    Mesh stl;
    std::string filename;
    if(quietmode)
        filename = argv[2];
    else
        filename = argv[1];
    int errorcode = vcg::tri::io::ImporterSTL<Mesh>::Open(stl, filename.c_str());
    if (errorcode != 0)
    {
        std::string error = vcg::tri::io::ImporterSTL<Mesh>::ErrorMsg(errorcode);
        std::cerr << "An error occured while loading " << filename << ": " << error << std::endl;
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
    int Holes = 0;
    if (NonManifoldEdges == 0) Holes = vcg::tri::Clean<Mesh>::CountHoles(stl);
    else if (!quietmode)
        std::cout << "Hole detection skipped, mesh is not manifold\n";
    std::vector<Mesh::FaceType *> SelfIntersectList;
    stl.face.EnableMark();
    vcg::tri::Clean<Mesh>::SelfIntersections(stl, SelfIntersectList);
    stl.face.DisableMark();
    int SelfIntersections = SelfIntersectList.size();
    if (!quietmode)
        std::cout << "Model has " << NonManifoldEdges << " non-manifold edges and "
              << NonManifoldVertices << " non-manifold vertices" << std::endl
              << SelfIntersections << " self-intersecting faces and "
              << Holes << " holes" << std::endl;

    // Calculate return value
    int returnvalue = 0;
    if (NonManifoldEdges > 0)
        returnvalue |= ERROR_NONMANIFOLD_EDGE;
    if (Holes > 0)
        returnvalue |= ERROR_HOLES;
    if (SelfIntersections > 0)
        returnvalue |= ERROR_SELF_INTERSECT;
    return returnvalue;
}
