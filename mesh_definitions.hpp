#ifndef MESH_DEFINITIONS_HPP
#define MESH_DEFINITIONS_HPP

#include <vector>
#include <typeinfo>

#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/vertex/component.h>
#include <vcg/simplex/face/base.h>
#include <vcg/simplex/face/component.h>
#include <vcg/simplex/face/component_ocf.h>

#include <vcg/complex/complex.h>

class Vertex;
class Face;
class Edge;

class UsedTypes : public vcg::UsedTypes< vcg::Use<Vertex>::AsVertexType,
                                         vcg::Use<Edge>::AsEdgeType,
                                         vcg::Use<Face>::AsFaceType > {};
class Vertex : public vcg::Vertex<UsedTypes, vcg::vertex::Coord3d, vcg::vertex::Normal3f, vcg::vertex::BitFlags> {};
class Edge : public vcg::Edge<UsedTypes, vcg::edge::VertexRef, vcg::edge::BitFlags> {};
class Face : public vcg::Face<UsedTypes, vcg::face::VertexRef, vcg::face::BitFlags, vcg::face::FFAdj, vcg::face::Normal3f, vcg::face::InfoOcf, vcg::face::MarkOcf> {};
class Mesh : public vcg::tri::TriMesh< std::vector<Vertex>, vcg::face::vector_ocf<Face>, std::vector<Edge> > {};


#endif // MESH_DEFINITIONS_HPP
