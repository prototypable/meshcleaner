#include <iostream>
#include <sstream>
#include "mesh_definitions.hpp"
#include "meshcleanerworker.hpp"

std::string file_basename(const std::string & fn)
{
    std::string::size_type index = fn.rfind('.');
    if (index != std::string::npos)
        return fn.substr(0, index);
    else
        return fn;
}

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " file.stl" << std::endl;
        return -1;
    }
    MeshcleanerWorker Worker;
    Worker.LoadSTL(argv[1]);
    if (!Worker.IsLoaded())
    {
        std::cerr << "Cannot open file " << argv[1] << std::endl;
        return -1;
    }

    if (Worker.IsManifold())
    {
        std::cerr << "File is already manifold and doesn't need cleaning" << std::endl;
        return 0;
    }

    Worker.Clean();

    if (Worker.IsManifold())
    {
        std::cerr << "File cleaned successfully, writing result to " << file_basename(argv[1]) << "_fixed.stl" << std::endl;
        std::stringstream fn;
        fn << file_basename(argv[1]) << "_fixed.stl";
        Worker.WriteSTL(fn.str());
        return 0;
    }

    std::cout << "Cleaning failed" << std::endl;
    return 1;
}
