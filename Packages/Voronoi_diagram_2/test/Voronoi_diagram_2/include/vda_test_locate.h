#ifndef VDA_TEST_LOCATE_H
#define VDA_TEST_LOCATE_H 1

#include <CGAL/basic.h>
#include <iostream>
#include <vector>


template<class T>
void kill_warning(const T&) {}

template<class VDA, class Projector, class QStream, class OStream>
void test_locate(const VDA& vda, const Projector& project,
		 QStream& isq, OStream& os = std::cout)
{
  std::cout << std::endl;
  std::cout << "is Delaunay graph valid? "
	    << (vda.dual().is_valid() ? "yes" : "no") << std::endl;
  std::cout << std::endl;

  os << "Dimension of Delaunay graph: " << vda.dual().dimension()
     << std::endl << std::endl;

  os << "Vertices of the Delaunay graph:" << std::endl;
  typename VDA::Dual_graph::Finite_vertices_iterator vit;
  for (vit = vda.dual().finite_vertices_begin();
       vit != vda.dual().finite_vertices_end(); ++vit) {
    os << project(vit) << std::endl;
  }
  os << std::endl;

  typedef typename VDA::Voronoi_traits::Point_2  Point_2;
  Point_2 p;
  std::vector<Point_2>  vecp;

  while ( isq >> p ) {
    vecp.push_back(p);
  }

  test_locate_dg(vda, project, vecp, os);
  test_locate_vd(vda, vecp, os);
}

template<class VDA, class Projector, class Point_vector, class OStream>
void test_locate_dg(const VDA& vda, const Projector& project,
		    const Point_vector& vecp, OStream& os)
{
  typedef typename VDA::Voronoi_traits                Voronoi_traits;
  typedef typename Voronoi_traits::Point_locator      Point_locator;
  
  typedef typename Point_locator::Vertex_handle       Vertex_handle;
  typedef typename Point_locator::Face_handle         Face_handle;
  typedef typename Point_locator::Edge                Edge;

  Point_locator locate = vda.voronoi_traits().point_locator_object();
  typename Point_locator::Locate_type pl_lt;

  os << "Query sites and location feature in dual graph:" << std::endl;
  for (unsigned int i = 0; i < vecp.size(); ++i) {
    os << vecp[i] << "\t --> \t" << std::flush;
    pl_lt = locate(vda.dual(), vecp[i]);
    if ( pl_lt.is_vertex() ) {
      os << "FACE";
      Vertex_handle v = pl_lt.vertex();
      kill_warning( v );
    } else if ( pl_lt.is_edge() ) {
      os << "EDGE";
      Edge e = pl_lt.edge();
      kill_warning( e );
    } else if ( pl_lt.is_face() ) {
      os << "VERTEX";
      Face_handle f = pl_lt.face();
      kill_warning( f );
    } else {
      os << " *** NOT READY YET *** ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

template<class VDA, class Point_vector, class OStream>
void test_locate_vd(const VDA& vda, const Point_vector& vecp, OStream& os)
{
  typename VDA::Locate_type lt;

  os << "Query sites and location feature in dual graph:" << std::endl;
  for (unsigned int i = 0; i < vecp.size(); ++i) {
    os << vecp[i] << "\t --> \t" << std::flush;
    lt = vda.locate(vecp[i]);
    if ( lt.is_edge() ) {
      os << "VORONOI EDGE";
      typename VDA::Halfedge_handle e = lt.edge();
      kill_warning( e );
    } else if ( lt.is_vertex() ) {
      os << "VORONOI VERTEX";
      typename VDA::Vertex_handle v = lt.vertex();
      kill_warning( v );
    } else if ( lt.is_face() ) {
      typename VDA::Face_handle f = lt.face();
      kill_warning( f );
      os << "VORONOI FACE";
    } else {
      os << " *** NOT READY YET *** ";
      CGAL_assertion( false );
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}


#endif // VDA_TEST_LOCATE_H
