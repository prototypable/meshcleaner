#ifndef MESH_DEFINITIONS_HPP
#define MESH_DEFINITIONS_HPP

#include <vector>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component.h>

#include <vcg/complex/complex.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/import_stl.h>

class Vertex;
class Face;
class Edge;

class UsedTypes : public vcg::UsedTypes< vcg::Use<Vertex>::AsVertexType,
                                         vcg::Use<Edge>::AsEdgeType,
                                         vcg::Use<Face>::AsFaceType > {};
class Vertex : public vcg::Vertex<UsedTypes, vcg::vertex::Coord3d, vcg::vertex::Normal3f, vcg::vertex::BitFlags> {};
class Face : public vcg::Face<UsedTypes, vcg::face::VertexRef, vcg::face::BitFlags, vcg::face::FFAdj> {};
class Edge : public vcg::Edge<UsedTypes, vcg::edge::VertexRef, vcg::edge::BitFlags> {};
class Mesh : public vcg::tri::TriMesh< std::vector<Vertex>, std::vector<Face> > {};

#endif // MESH_DEFINITIONS_HPP
