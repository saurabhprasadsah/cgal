// Copyright (c) 2005 Foundation for Research and Technology-Hellas (Greece).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Menelaos Karavelas <mkaravel@tem.uoc.gr>

#ifndef CGAL_DELAUNAY_TRIANGULATION_VORONOI_TRAITS_2_H
#define CGAL_DELAUNAY_TRIANGULATION_VORONOI_TRAITS_2_H 1

#include <CGAL/Voronoi_diagram_adaptor_2/basic.h>
#include <CGAL/Voronoi_diagram_adaptor_2/Default_Voronoi_traits_2.h>
#include <CGAL/Voronoi_diagram_adaptor_2/Voronoi_vertex_base_2.h>
#include <CGAL/Voronoi_diagram_adaptor_2/Voronoi_edge_base_2.h>
#include <CGAL/Voronoi_diagram_adaptor_2/Locate_type.h>
#include <cstdlib>
#include <algorithm>

CGAL_BEGIN_NAMESPACE

//=========================================================================
//=========================================================================


template<class DG>
class DT_Point_locator
  : public CGAL_VORONOI_DIAGRAM_2_NS::Locate_type_accessor<DG,false>
{
  typedef CGAL_VORONOI_DIAGRAM_2_NS::Locate_type_accessor<DG,false> Base;

 public:
  typedef DG                                          Dual_graph;
  typedef typename Dual_graph::Vertex_handle          Vertex_handle;
  typedef typename Dual_graph::Face_handle            Face_handle;
  typedef typename Dual_graph::Edge                   Edge;
  typedef typename Dual_graph::Geom_traits::Point_2   Point_2;

  typedef CGAL_VORONOI_DIAGRAM_2_NS::Locate_type<DG,false> Locate_type;

  typedef Arity_tag<2>  Arity;
  typedef Locate_type   return_type;

 private:
  typedef Triangulation_cw_ccw_2                      CW_CCW_2;
  typedef typename Dual_graph::Vertex_circulator      Vertex_circulator;
  typedef typename Dual_graph::Face_circulator        Face_circulator;
  typedef typename Dual_graph::Edge_circulator        Edge_circulator;

 public:
  Locate_type operator()(const Dual_graph& dg, const Point_2& p) const {
    CGAL_precondition( dg.dimension() >= 0 );

    typename DG::Geom_traits::Compare_distance_2 compare_distance =
      dg.geom_traits().compare_distance_2_object();

    Vertex_handle v = dg.nearest_vertex(p);

    if ( dg.dimension() == 0 ) {
      return Base::make_locate_type(v);
    }

    if ( dg.dimension() == 1 ) {
      Edge_circulator ec = dg.incident_edges(v);
      Edge_circulator ec_start = ec;
      Comparison_result cr;

      do {
	Edge e = *ec;
	Vertex_handle v1 = e.first->vertex(CW_CCW_2::ccw(e.second));
	Vertex_handle v2 = e.first->vertex(CW_CCW_2::cw(e.second) );

	if ( v == v1 ) {
	  if ( !dg.is_infinite(v2) ) {
	    cr = compare_distance(p, v2->point(), v->point());
	    CGAL_assertion( cr != SMALLER );
	    if ( cr == EQUAL ) {
	      return Base::make_locate_type( e );
	    }
	  }
	} else {
	  CGAL_assertion( v == v2 );
	  if ( !dg.is_infinite(v1) ) {
	    cr = compare_distance(p, v1->point(), v->point());
	    CGAL_assertion( cr != SMALLER );
	    if ( cr == EQUAL ) {
	      return Base::make_locate_type( e );
	    }
	  }
	}
	++ec;
      } while ( ec != ec_start );

      return Base::make_locate_type(v);
    }

    CGAL_assertion( dg.dimension() == 2 );

    Face_circulator fc_start = dg.incident_faces(v);
    Face_circulator fc = fc_start;

    // first check if the point lies on a Voronoi vertex
    do {
      int index = fc->index(v);
      Vertex_handle v1 = fc->vertex( CW_CCW_2::ccw(index) );
      Vertex_handle v2 = fc->vertex( CW_CCW_2::cw(index)  );

      Comparison_result cr1 = LARGER, cr2 = LARGER;

      // do the generic check now
      if ( !dg.is_infinite(v1) ) {
	cr1 = compare_distance(p, v1->point(), v->point());
      }
      if ( !dg.is_infinite(v2) ) {
	cr2 = compare_distance(p, v2->point(), v->point());
      }

      CGAL_assertion( cr1 != SMALLER );
      CGAL_assertion( cr2 != SMALLER );

      if ( cr1 == EQUAL && cr2 == EQUAL ) {
	Face_handle f(fc);
	return Base::make_locate_type(f);
      }

      ++fc;
    } while ( fc != fc_start );

    // now check if the point lies on a Voronoi edge
    fc_start = dg.incident_faces(v);
    fc = fc_start;
    do {
      int index = fc->index(v);
      Vertex_handle v1 = fc->vertex( CW_CCW_2::ccw(index) );
      Vertex_handle v2 = fc->vertex( CW_CCW_2::cw(index)  );

      Comparison_result cr1 = LARGER, cr2 = LARGER;

      // do the generic check now
      if ( !dg.is_infinite(v1) ) {
	cr1 = compare_distance(p, v1->point(), v->point());
      }
      if ( !dg.is_infinite(v2) ) {
	cr2 = compare_distance(p, v2->point(), v->point());
      }

      CGAL_assertion( cr1 != SMALLER );
      CGAL_assertion( cr2 != SMALLER );
      CGAL_assertion( cr1 != EQUAL || cr2 != EQUAL );

      if ( cr1 == EQUAL ) {
	Face_handle f(fc);
	Edge e(f, CW_CCW_2::cw(index) );
	return Base::make_locate_type(e);
      } else if ( cr2 == EQUAL ) {
	Face_handle f(fc);
	Edge e(f, CW_CCW_2::ccw(index) );
	return Base::make_locate_type(e);
      }

      ++fc;
    } while ( fc != fc_start );

    return Base::make_locate_type(v);
  }
};




//=========================================================================
//=========================================================================

template<class DG>
class DT_Edge_degeneracy_tester
{
  // tests whether a dual edge has zero length
 public:
  typedef DG                                          Dual_graph;

  typedef typename Dual_graph::Edge                   Edge;
  typedef typename Dual_graph::Face_handle            Face_handle;
  typedef typename Dual_graph::Edge_circulator        Edge_circulator;
  typedef typename Dual_graph::All_edges_iterator     All_edges_iterator;
  typedef typename Dual_graph::Finite_edges_iterator  Finite_edges_iterator;

  typedef bool           result_type;
  typedef Arity_tag<2>   Arity;

 private:
  typedef DT_Edge_degeneracy_tester<Dual_graph>    Self;

  typedef typename Dual_graph::Geom_traits         Geom_traits;
  typedef typename Dual_graph::Vertex_handle       Vertex_handle;
  typedef typename Geom_traits::Point_2            Point_2;

 public:
  bool operator()(const Dual_graph& dual, const Face_handle& f, int i) const
  {
    if ( dual.dimension() == 1 ) { return false; }

    if ( dual.is_infinite(f, i) ) { return false; }

    Vertex_handle v3 = f->vertex(     i  );
    Vertex_handle v4 = dual.tds().mirror_vertex(f, i);

    if ( dual.is_infinite(v3) || dual.is_infinite(v4) ) {
      return false;
    }

    Vertex_handle v1 = f->vertex( dual.ccw(i) );
    Vertex_handle v2 = f->vertex( dual.cw(i) );

    Point_2 p1 = v1->point();
    Point_2 p2 = v2->point();
    Point_2 p3 = v3->point();
    Point_2 p4 = v4->point();
    Oriented_side os =
      dual.geom_traits().side_of_oriented_circle_2_object()(p1,p2,p3,p4);
    return os == ON_ORIENTED_BOUNDARY;
  }

  bool operator()(const Dual_graph& dual, const Edge& e) const {
    return operator()(dual, e.first, e.second);
  }

  bool operator()(const Dual_graph& dual,
		  const All_edges_iterator& eit) const {
    return operator()(dual, *eit);
  }

  bool operator()(const Dual_graph& dual,
		  const Finite_edges_iterator& eit) const {
    return operator()(dual, *eit);
  }

  bool operator()(const Dual_graph& dual, const Edge_circulator& ec) const {
    return operator()(dual, *ec);
  }
};

//=========================================================================
//=========================================================================

template<class DG> class Delaunay_triangulation_Voronoi_traits_2;
template<class DG> class DT_Voronoi_edge_2;

template<class DG>
class DT_Voronoi_vertex_2
  : public CGAL_VORONOI_DIAGRAM_2_NS::Voronoi_vertex_base_2
  <DG, typename DG::Geom_traits::Point_2, typename DG::Geom_traits::Point_2,
   DT_Voronoi_vertex_2<DG> >
{
  friend class Delaunay_triangulation_Voronoi_traits_2<DG>;
  friend class DT_Voronoi_edge_2<DG>;
#ifndef CGAL_CFG_NESTED_CLASS_FRIEND_DECLARATION_BUG
  friend class DT_Voronoi_edge_2<DG>::Base;
#else
  friend class
  CGAL_VORONOI_DIAGRAM_2_NS::Voronoi_edge_base_2
  <DG,
   typename DG::Geom_traits::Point_2,
   typename DG::Geom_traits::Point_2,
   DT_Voronoi_edge_2<DG>,
   DT_Voronoi_vertex_2<DG> >;
#endif

 private:
  typedef typename DG::Geom_traits::Point_2  Point_2;

  typedef CGAL_VORONOI_DIAGRAM_2_NS::Voronoi_vertex_base_2
  <DG, Point_2, Point_2, DT_Voronoi_vertex_2<DG> >
  Base;

 public:
  operator typename Base::Point_2() const {
    return typename Base::Geom_traits().construct_circumcenter_2_object()
      (this->s_[0], this->s_[1], this->s_[2]);
  }
};

//=========================================================================
  
template<class DG>
class DT_Voronoi_edge_2
  : public CGAL_VORONOI_DIAGRAM_2_NS::Voronoi_edge_base_2
  <DG, typename DG::Geom_traits::Point_2, typename DG::Geom_traits::Point_2,
   DT_Voronoi_edge_2<DG>, DT_Voronoi_vertex_2<DG> >
{
  friend class Delaunay_triangulation_Voronoi_traits_2<DG>;
  friend class DT_Voronoi_vertex_2<DG>;

 private:
  typedef typename DG::Geom_traits::Point_2  Point_2;

  typedef CGAL_VORONOI_DIAGRAM_2_NS::Voronoi_edge_base_2
  <DG,Point_2,Point_2,DT_Voronoi_edge_2<DG>,DT_Voronoi_vertex_2<DG> >
  Base;
};


//=========================================================================



template<class DT2>
class Delaunay_triangulation_Voronoi_traits_2
  : public CGAL_VORONOI_DIAGRAM_2_NS::Default_Voronoi_traits_2
  <DT2, DT_Edge_degeneracy_tester<DT2>,
   CGAL_VORONOI_DIAGRAM_2_NS::Default_face_degeneracy_tester<DT2>,
   DT_Point_locator<DT2> >
{
 private:
  typedef DT_Edge_degeneracy_tester<DT2>              Edge_tester;

  typedef CGAL_VORONOI_DIAGRAM_2_NS::Default_face_degeneracy_tester<DT2>
  Face_tester;

  typedef DT_Point_locator<DT2>                       DT_Point_locator;

  typedef CGAL_VORONOI_DIAGRAM_2_NS::Default_Voronoi_traits_2
  <DT2,Edge_tester,Face_tester,DT_Point_locator>
  Base;

  typedef Delaunay_triangulation_Voronoi_traits_2<DT2>  Self;

 public:
  typedef typename DT2::Geom_traits::Point_2      Point_2;
  typedef Point_2                                 Site_2;
  typedef typename Base::Vertex_handle            Vertex_handle;

  typedef DT_Voronoi_vertex_2<DT2>                Voronoi_vertex_2;
  typedef DT_Voronoi_edge_2<DT2>                  Voronoi_edge_2;
  typedef Voronoi_edge_2                          Curve;

  static Voronoi_vertex_2 make_vertex(const Vertex_handle& v1,
				      const Vertex_handle& v2,
				      const Vertex_handle& v3) {
    Voronoi_vertex_2 vv;
    vv.set_sites(v1->point(), v2->point(), v3->point());
    return vv;
  }

  static Voronoi_edge_2 make_edge(const Vertex_handle& v1,
				  const Vertex_handle& v2) {
    Voronoi_edge_2 ve;
    ve.set_sites(v1->point(), v2->point());
    return ve;
  }

  static Voronoi_edge_2 make_edge(const Vertex_handle& v1,
				  const Vertex_handle& v2,
				  const Vertex_handle& v3,
				  bool is_src) {
    Voronoi_edge_2 ve;
    ve.set_sites(v1->point(), v2->point(), v3->point(), is_src);
    return ve;
  }

  static Voronoi_edge_2 make_edge(const Vertex_handle& v1,
				  const Vertex_handle& v2,
				  const Vertex_handle& v3,
				  const Vertex_handle& v4) {
    Voronoi_edge_2 ve;
    ve.set_sites(v1->point(), v2->point(), v3->point(), v4->point());
    return ve;
  }
};


//=========================================================================
//=========================================================================

template<class DT2>
class Delaunay_triangulation_cached_Voronoi_traits_2
  : public CGAL_VORONOI_DIAGRAM_2_NS::Default_cached_Voronoi_traits_2
  <DT2, DT_Edge_degeneracy_tester<DT2>,
   CGAL_VORONOI_DIAGRAM_2_NS::Default_face_degeneracy_tester<DT2>,
   DT_Point_locator<DT2> >
{
 private:
  typedef DT_Edge_degeneracy_tester<DT2>              Edge_tester;
  typedef CGAL_VORONOI_DIAGRAM_2_NS::Default_face_degeneracy_tester<DT2>
  Face_tester;

  typedef DT_Point_locator<DT2>                       DT_Point_locator;

  typedef
  CGAL_VORONOI_DIAGRAM_2_NS::Default_cached_Voronoi_traits_2
  <DT2,Edge_tester,Face_tester,DT_Point_locator>
  Base;

  typedef Delaunay_triangulation_cached_Voronoi_traits_2<DT2>  Self;
};

//=========================================================================
//=========================================================================

template<class DT2>
class Delaunay_triangulation_ref_counted_Voronoi_traits_2
  : public CGAL_VORONOI_DIAGRAM_2_NS::Default_ref_counted_Voronoi_traits_2
  <DT2, DT_Edge_degeneracy_tester<DT2>,
   CGAL_VORONOI_DIAGRAM_2_NS::Default_face_degeneracy_tester<DT2>,
   DT_Point_locator<DT2> >
{
 private:
  typedef DT_Edge_degeneracy_tester<DT2>               Edge_tester;

  typedef CGAL_VORONOI_DIAGRAM_2_NS::Default_face_degeneracy_tester<DT2>
  Face_tester;

  typedef DT_Point_locator<DT2>                       DT_Point_locator;

  typedef
  CGAL_VORONOI_DIAGRAM_2_NS::Default_ref_counted_Voronoi_traits_2
  <DT2,Edge_tester,Face_tester,DT_Point_locator>
  Base;

  typedef Delaunay_triangulation_ref_counted_Voronoi_traits_2<DT2>  Self;
};

//=========================================================================
//=========================================================================


CGAL_END_NAMESPACE

#endif // CGAL_DELAUNAY_TRIANGULATION_VORONOI_TRAITS_2_H
