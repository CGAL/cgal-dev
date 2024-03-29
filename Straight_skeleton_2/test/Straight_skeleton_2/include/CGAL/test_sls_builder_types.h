// Copyright (c) 2005, 2006 Fernando Luis Cacciola Carballal. All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Fernando Cacciola <fernando_cacciola@ciudad.com.ar>
//
#ifndef CGAL_TEST_SLS_BUILDER_TYPES_H
#define CGAL_TEST_SLS_BUILDER_TYPES_H

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>

#include <CGAL/Straight_skeleton_builder_2.h>
#include <CGAL/Polygon_offset_builder_2.h>
#include <CGAL/HalfedgeDS_const_decorator.h>

#include <memory>
#include <vector>

//typedef CGAL::Simple_cartesian<double> K ;
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt K;

typedef K::Point_2                    Point;
typedef CGAL::Aff_transformation_2<K> Transformation;
typedef std::vector<Point>            Polygon_2;
typedef std::shared_ptr<Polygon_2>  PolygonPtr;
typedef CGAL::Segment_2<K>            Segment;
typedef std::vector<PolygonPtr>       Region ;
typedef std::shared_ptr<Region>     RegionPtr ;
typedef std::vector<RegionPtr>        Regions ;
typedef std::vector<double>           Doubles ;

typedef CGAL::Straight_skeleton_2<K>                            Sls;
typedef CGAL::Straight_skeleton_builder_traits_2<K>             SlsBuilderTraits;
typedef CGAL::Straight_skeleton_builder_2<SlsBuilderTraits,Sls> SlsBuilder;

typedef Sls::Halfedge_iterator     Halfedge_iterator;
typedef Sls::Vertex_handle         Vertex_handle;
typedef Sls::Face_const_iterator   Face_const_iterator;
typedef Sls::Vertex_const_iterator Vertex_const_iterator;
typedef Sls::Halfedge_const_handle Halfedge_const_handle ;
typedef Sls::Vertex_const_handle   Vertex_const_handle ;

typedef CGAL::HalfedgeDS_const_decorator<Sls> Sls_const_decorator ;

typedef std::shared_ptr<Sls> SlsPtr ;

#endif

