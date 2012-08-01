#ifndef MESHCLEANERWORKER_HPP
#define MESHCLEANERWORKER_HPP
#include "mesh_definitions.hpp"

class MeshcleanerWorker
{
public:
    MeshcleanerWorker();
    // Loads a file while constructing the worker
    MeshcleanerWorker(std::string stl_fn);
    ~MeshcleanerWorker();

    // Loads a STL file
    bool LoadSTL(std::string fn);
    // Loads a mesh directly from memory
    bool LoadFromMesh(Mesh & inputmesh);
    // Writes out a stl, as binary by default but can be overridden
    bool WriteSTL(std::string fn, bool binary = true);

    // Check if a STL file is loaded into the worker
    bool IsLoaded() const { return STLLoaded; }
    // Get the last error
    const std::string & GetError() const { return ErrorDescription; }

    bool IsManifold();

    void GenerateTopology();

    void Clean();
protected:
    Mesh STL;
    bool STLLoaded;
    bool TopologyGenerated;
    std::string ErrorDescription;
};

#endif // MESHCLEANERWORKER_HPP
