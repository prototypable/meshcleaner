#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <json/json.h>

#include "mesh_definitions.hpp"

#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/export_stl.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/topology.h>
//#include <vcg

using namespace std;

using boost::asio::ip::tcp;

int main(int argc, char * argv[])
{
    boost::asio::io_service io_service;

    Mesh MyMesh;
    int errorcode = vcg::tri::io::ImporterSTL<Mesh>::Open(MyMesh, "test.stl");
    if (errorcode != 0)
    {
        std::string error = vcg::tri::io::ImporterSTL<Mesh>::ErrorMsg(errorcode);
        std::cerr << "An error occured while loading test.stl: " << error << std::endl;
        return -1;
    }

    std::cout << "Mesh test.stl contains " << MyMesh.vn << " Vertices and " << MyMesh.fn << " facets\n";

    std::cout << "Removing duplicated vertices...\n";
    int removed = vcg::tri::Clean<Mesh>::RemoveDuplicateVertex(MyMesh);
    std::cout << "Removing duplicated faces...\n";
    vcg::tri::Clean<Mesh>::RemoveDuplicateFace(MyMesh);
    std::cout << "Removing Unreferenced vertices...\n";
    vcg::tri::Clean<Mesh>::RemoveUnreferencedVertex(MyMesh);
    std::cout << "Removing Zero-area faces...\n";
    vcg::tri::Clean<Mesh>::RemoveZeroAreaFace(MyMesh);


    vcg::tri::UpdateTopology<Mesh>::FaceFace(MyMesh);
    std::cout << "Removing Non-manifold faces...\n";
    vcg::tri::Clean<Mesh>::RemoveNonManifoldFace(MyMesh);
    vcg::tri::UpdateTopology<Mesh>::FaceFace(MyMesh);
    vcg::tri::Clean<Mesh>::RemoveNonManifoldVertex(MyMesh);
    vcg::tri::UpdateTopology<Mesh>::FaceFace(MyMesh);

    vcg::tri::Allocator<Mesh>::CompactVertexVector(MyMesh);
    vcg::tri::Allocator<Mesh>::CompactFaceVector(MyMesh);
    std::cout << "Mesh now contains " << MyMesh.VertexNumber() << " Vertices and " << MyMesh.fn << " facets\n";
    std::cout << "Mesh has " << vcg::tri::Clean<Mesh>::CountNonManifoldEdgeFF(MyMesh) << " Non-manifold edges and " <<
                vcg::tri::Clean<Mesh>::CountNonManifoldVertexFF(MyMesh) << "Non-manifold-vertices\n";

    vcg::tri::io::ExporterSTL<Mesh>::Save(MyMesh, "out.stl", true);
    return 0;
}

