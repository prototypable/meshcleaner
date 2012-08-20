#include "meshcleanerworker.hpp"
#include <string>
// Includes for STL import
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/export_stl.h>
// Include cleaning and topology headers
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/append.h>
#include <vcg/complex/algorithms/hole.h>
#include<vcg/space/triangle3.h>

MeshcleanerWorker::MeshcleanerWorker()
{
    STLLoaded = false;
    TopologyGenerated = false;
}

MeshcleanerWorker::MeshcleanerWorker(std::string stl_fn)
{
    TopologyGenerated = false;
    LoadSTL(stl_fn);
}

bool MeshcleanerWorker::LoadSTL(std::string stl_fn)
{
    ErrorDescription.clear();
    STL.Clear();
    int error = vcg::tri::io::ImporterSTL<Mesh>::Open(STL, stl_fn.c_str());
    if (error != 0)
    {
        ErrorDescription = vcg::tri::io::ImporterSTL<Mesh>::ErrorMsg(error);
        STLLoaded = false;
        return false;
    }
    STLLoaded = true;

    vcg::tri::Clean<Mesh>::RemoveDuplicateVertex(STL);

    // Force garbage collection on vectors to free some memory
    vcg::tri::Allocator<Mesh>::CompactVertexVector(STL);
    vcg::tri::Allocator<Mesh>::CompactFaceVector(STL);
    return true;
}

bool MeshcleanerWorker::LoadFromMesh(Mesh & inputmesh)
{
    ErrorDescription.clear();
    STL.Clear();
    vcg::tri::Append<Mesh, Mesh>::Mesh(STL, inputmesh);
    return true;
}

bool MeshcleanerWorker::WriteSTL(std::string fn, bool binary)
{
    vcg::tri::io::ExporterSTL<Mesh>::Save(STL, fn.c_str(), binary);
}

MeshcleanerWorker::~MeshcleanerWorker()
{

}

bool MeshcleanerWorker::IsManifold()
{
    if (!TopologyGenerated)
        GenerateTopology();
    if (vcg::tri::Clean<Mesh>::CountNonManifoldEdgeFF(STL) > 0)
        return false;
    else
        return true;
}

void MeshcleanerWorker::GenerateTopology()
{
    // Clean out duplicate faces, unreferenced vertices and zero area faces to prevent
    // topology generation from crashing
    vcg::tri::Clean<Mesh>::RemoveDuplicateFace(STL);
    vcg::tri::Clean<Mesh>::RemoveUnreferencedVertex(STL);
    vcg::tri::Clean<Mesh>::RemoveZeroAreaFace(STL);
    vcg::tri::Clean<Mesh>::RemoveDegenerateFace(STL);

    vcg::tri::UpdateTopology<Mesh>::FaceFace(STL);
    TopologyGenerated = true;
}

void MeshcleanerWorker::Clean()
{
    if(!TopologyGenerated)
        GenerateTopology();
    // Cleanout non-manifold faces and vertices, if any is removed, rebuild topology after
    if (vcg::tri::Clean<Mesh>::RemoveNonManifoldFace(STL) > 0)
        vcg::tri::UpdateTopology<Mesh>::FaceFace(STL);
    if (vcg::tri::Clean<Mesh>::RemoveNonManifoldVertex(STL) > 0)
        vcg::tri::UpdateTopology<Mesh>::FaceFace(STL);

    STL.face.EnableMark();
    std::vector<Face *> SelfIntersectingFaces;
    vcg::tri::Clean<Mesh>::SelfIntersections(STL, SelfIntersectingFaces);
    if (SelfIntersectingFaces.size() == 0)
        return;
    std::cout << SelfIntersectingFaces.size() << " Self-intersections found, attempting repair" << std::endl;
    for(std::vector<Face *>::iterator it = SelfIntersectingFaces.begin(); it != SelfIntersectingFaces.end(); ++it)
    {
        if (! (*it)->IsD())
            vcg::tri::Allocator<Mesh>::DeleteFace(STL, *(*it));
    }
    vcg::tri::Allocator<Mesh>::CompactFaceVector(STL);
    SelfIntersectingFaces.clear();
    vcg::tri::Clean<Mesh>::SelfIntersections(STL, SelfIntersectingFaces);
    std::cout << SelfIntersectingFaces.size() << " Selfintersections remain after deletion\n";
    std::cout << "Filling in the holes, currently have " << vcg::tri::Clean<Mesh>::CountHoles(STL) << " holes" << std::endl;
    vcg::tri::Hole<Mesh>::EarCuttingFill< vcg::tri::TrivialEar<Mesh> >(STL, 50, false);
    vcg::tri::UpdateTopology<Mesh>::FaceFace(STL);
    std::cout << vcg::tri::Clean<Mesh>::CountHoles(STL) << " holes remain after removal of self-intersecting faces and re-fill\n";
}
