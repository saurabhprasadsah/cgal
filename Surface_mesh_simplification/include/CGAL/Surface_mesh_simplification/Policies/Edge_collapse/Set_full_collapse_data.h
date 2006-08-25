// Copyright (c) 2005, 2006 Fernando Luis Cacciola Carballal. All rights reserved.
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
// $URL: svn+ssh://fcacciola@scm.gforge.inria.fr/svn/cgal/trunk/Surface_mesh_simplification/include/CGAL/Surface_mesh_simplification/Policies/Edge_collapse/LindstromTurk_set_collapse_data.h $
// $Id: LindstromTurk_set_collapse_data.h 33680 2006-08-24 15:22:37Z fcacciola $
//
// Author(s)     : Fernando Cacciola <fernando_cacciola@ciudad.com.ar>
//
#ifndef CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_SET_FULL_COLLAPSE_DATA_H
#define CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_SET_FULL_COLLAPSE_DATA_H 1

#include <CGAL/Surface_mesh_simplification/TSMS_common.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Full_collapse_data.h>


CGAL_BEGIN_NAMESPACE

namespace Triangulated_surface_mesh { namespace Simplification { namespace Edge_collapse
{

template<class TSM_, class GetCost_, class GetPlacement_>    
class Set_full_collapse_data
{
public:

  typedef TSM_          TSM ;
  typedef GetCost_      GetCost ;
  typedef GetPlacement_ GetPlacement ;
  
  typedef typename boost::graph_traits<TSM>::edge_descriptor edge_descriptor ;
  
  typedef Full_collapse_data<TSM> Collapse_data ;
  
  typedef typename Collapse_data::Optional_cost_type      Optional_cost_type ;
  typedef typename Collapse_data::Optional_placement_type Optional_placement_type ;

public :

  Set_full_collapse_data ( GetCost const& aGetCost, GetPlacement const& aGetPlacement ) : get_cost(aGetCost), get_placement(aGetPlacement) {}
    
  template<class Params>
  void operator() ( Collapse_data& rData, edge_descriptor const& aEdge, TSM& aSurface, Params const* aParams ) const 
  {
    CGAL_assertion(aParams);
    CGAL_assertion( handle_assigned(aEdge) );

    Optional_cost_type      lCost      = get_cost     (aEdge,aSurface,rData,aParams);
    Optional_placement_type lPlacement = get_placement(aEdge,aSurface,rData,aParams);
    
    rData = Collapse_data(lCost,lPlacement);
  }                         
  
  GetCost      get_cost ;
  GetPlacement get_placement ; 
};    

} } } // namespace Triangulated_surface_mesh::Simplification::Edge_collapse

CGAL_END_NAMESPACE

#endif // CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_SET_FULL_COLLAPSE_DATA_H //
// EOF //
 
