
#ifndef CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_BASIC_PREDICATES_C2_H
#define CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_BASIC_PREDICATES_C2_H


#include <CGAL/Segment_Delaunay_graph_Linf_2/basic.h>
#include <CGAL/Segment_Delaunay_graph_2/Are_same_points_C2.h>

#include <CGAL/Polychain_2.h>

#include <CGAL/Segment_Delaunay_graph_2/Basic_predicates_C2.h>


namespace CGAL {

namespace SegmentDelaunayGraphLinf_2 {

template<class K>
struct Basic_predicates_C2
 : public CGAL::SegmentDelaunayGraph_2::Basic_predicates_C2<K>
{
public:
  //-------------------------------------------------------------------
  // TYPES
  //-------------------------------------------------------------------

  typedef CGAL::SegmentDelaunayGraph_2::Basic_predicates_C2<K> Base;

  typedef typename K::RT                  RT;
  typedef typename K::FT                  FT;
  typedef typename K::Point_2             Point_2;
  typedef typename K::Segment_2           Segment_2;
  typedef typename K::Site_2              Site_2;
  typedef typename K::Oriented_side       Oriented_side;
  typedef typename K::Bounded_side        Bounded_side;
  typedef typename K::Comparison_result   Comparison_result;
  typedef typename K::Sign                Sign;
  typedef typename K::Orientation         Orientation;
  typedef typename K::Compute_scalar_product_2
                        Compute_scalar_product_2;
  typedef typename K::Boolean             Boolean;
  typedef typename K::Direction_2         Direction_2;
  typedef typename K::Vector_2            Vector_2;
  typedef typename K::Compare_x_2         Compare_x_2;
  typedef typename K::Compare_y_2         Compare_y_2;

  typedef typename CGAL::Polychainline_2<K> Polychainline_2;
  typedef SegmentDelaunayGraph_2::Are_same_points_C2<K>  Are_same_points_2;

private:
  typedef typename Algebraic_structure_traits<RT>::Algebraic_category RT_Category;
  typedef typename Algebraic_structure_traits<FT>::Algebraic_category FT_Category;
public:

  typedef typename Base::Line_2               Line_2;
  typedef typename Base::Homogeneous_point_2  Homogeneous_point_2;

public:    //    compute_supporting_line(q.supporting_segment(), a1, b1, c1);
    //    compute_supporting_line(r.supporting_segment(), a2, b2, c2);

  //-------------------------------------------------------------------
  // BASIC CONSTRUCTIONS
  //-------------------------------------------------------------------

  using Base::compute_supporting_line;

  // compute horizontal line l that goes through p,
  // and leaves q on the oriented side s
  // s: has to be either +1 or -1 (not 0)
  // q: should not be on line l
  static
  Line_2
  compute_horizontal_side_line(
      const Point_2& p, const Point_2& q, Oriented_side s)
  {
    CGAL_precondition(s != ON_ORIENTED_BOUNDARY);

    RT b, c;

    b = RT(1);
    c = - p.y();

    // direction is (1, 0)

    Compare_y_2 cmpy;
    if (((cmpy(q, p) == LARGER) and
         (s == ON_NEGATIVE_SIDE)   ) or
        ((cmpy(q, p) == SMALLER) and
         (s == ON_POSITIVE_SIDE)   )   ) {
      b = -b;
      c = -c;
    }
    return Line_2(RT(0), b, c);
  }

  // compute vertical line l that goes through p,
  // and leaves q on the oriented side s
  // s: has to be either +1 or -1 (not 0)
  // q: should not be on line l
  static
  Line_2
  compute_vertical_side_line(
      const Point_2& p, const Point_2& q, Oriented_side s)
  {
    CGAL_precondition(s != ON_ORIENTED_BOUNDARY);

    RT a, c;

    a = RT(1);
    c = - p.x();

    // direction is (0, -1)

    Compare_x_2 cmpx;
    if (((cmpx(q, p) == LARGER) and
         (s == ON_NEGATIVE_SIDE)   ) or
        ((cmpx(q, p) == SMALLER) and
         (s == ON_POSITIVE_SIDE)   )   ) {
      a = -a;
      c = -c;
    }
    return Line_2(a, RT(0), c);
  }


  //using Base::compute_projection;

  static
  Homogeneous_point_2
  compute_linf_projection_hom(const Line_2& l, const Point_2& p)
  {
    //CGAL_precondition( not l.is_degenerate() );
    CGAL_precondition(
        (CGAL::sign(l.a()) != ZERO) or (CGAL::sign(l.b()) != ZERO) );

    Sign signa = CGAL::sign(l.a());
    Sign signb = CGAL::sign(l.b());

    RT hx, hy, hw;

    if (signa == ZERO) { // l is horizontal
      // l.a() == 0  =>  y = -c/b
      hx = p.x() * l.b();
      hy = - l.c();
      hw = l.b();
    } else if (signb == ZERO) { // l is vertical
      // l.b() == 0  =>  x = -c/a
      hx = - l.c();
      hy = p.y() * l.a();
      hw = l.a();
    } else {
      // here both l.a() and l.b() are non-zero
      if ( signa == signb ) {
        hx = l.b() * ( p.x() - p.y() ) - l.c();
        hy = l.a() * ( p.y() - p.x() ) - l.c();
        hw = l.a() + l.b();
      } else { // signa != signb
        hx = -l.b() * ( p.x() + p.y() ) - l.c();
        hy = l.a() * ( p.x() + p.y() ) + l.c();
        hw = l.a() - l.b();
      }
    }

    return Homogeneous_point_2(hx, hy, hw);
  }

  static
  Point_2
  compute_linf_projection_nonhom(const Line_2& l, const Point_2& p)
  {
    //CGAL_precondition( not l.is_degenerate() );
    CGAL_precondition(
        (CGAL::sign(l.a()) != ZERO) or (CGAL::sign(l.b()) != ZERO) );

    Sign signa = CGAL::sign(l.a());
    Sign signb = CGAL::sign(l.b());

    RT hx, hy, hw;

    if (signa == ZERO) { // l is horizontal
      // l.a() == 0  =>  y = -c/b
      hx = p.x() * l.b();
      hy = - l.c();
      hw = l.b();
    } else if (signb == ZERO) { // l is vertical
      // l.b() == 0  =>  x = -c/a
      hx = - l.c();
      hy = p.y() * l.a();
      hw = l.a();
    } else {
      // here both l.a() and l.b() are non-zero
      if ( signa == signb ) {
        hx = l.b() * ( p.x() - p.y() ) - l.c();
        hy = l.a() * ( p.y() - p.x() ) - l.c();
        hw = l.a() + l.b();
      } else { // signa != signb
        hx = -l.b() * ( p.x() + p.y() ) - l.c();
        hy = l.a() * ( p.x() + p.y() ) + l.c();
        hw = l.a() - l.b();
      }
    }

    return Point_2(hx, hy, hw);
  }

  static
  Homogeneous_point_2
  compute_horizontal_projection_hom(const Line_2& l, const Point_2& p)
  {
    //CGAL_precondition( not l.is_horizontal() );

    CGAL_precondition(
        (CGAL::sign(l.a()) != ZERO) );

    RT hx, hy, hw;

    hx = - l.b() * p.y() - l.c();
    hy = p.y() * l.a();
    hw = l.a();

    return Homogeneous_point_2(hx, hy, hw);
  }

  static
  Point_2
  compute_horizontal_projection(const Line_2& l, const Point_2& p)
  {
    //CGAL_precondition( not l.is_horizontal() );
    CGAL_precondition(
        (CGAL::sign(l.a()) != ZERO) );

    RT hx, hy, hw;

    hx = - l.b() * p.y() - l.c();
    hy = p.y() * l.a();
    hw = l.a();

    return Point_2(hx, hy, hw);
  }

  static
  Homogeneous_point_2
  compute_vertical_projection_hom(const Line_2& l, const Point_2& p)
  {
    //CGAL_precondition( not l.is_horizontal() );
    CGAL_precondition(
        (CGAL::sign(l.b()) != ZERO) );

    RT hx, hy, hw;

    hx = p.x() * l.b();
    hy = - l.a() * p.x() - l.c();
    hw = l.b();

    return Homogeneous_point_2(hx, hy, hw);
  }

  static
  Point_2
  compute_vertical_projection(const Line_2& l, const Point_2& p)
  {
    //CGAL_precondition( not l.is_horizontal() );
    CGAL_precondition(
        (CGAL::sign(l.b()) != ZERO) );

    RT hx, hy, hw;

    hx = p.x() * l.b();
    hy = - l.a() * p.x() - l.c();
    hw = l.b();

    return Point_2(hx, hy, hw);
  }


  //using Base::projection_on_line;

  using Base::midpoint;

  using Base::compute_perpendicular;

  // compute_cw_perpendicular is opposite of compute_perpendicular
  static
  Line_2 compute_cw_perpendicular(const Line_2& l, const Point_2& p)
  {
    RT a, b, c;
    a = l.b();
    b = -l.a();
    c = -l.b() * p.x() + l.a() * p.y();
    return Line_2(a, b, c);
  }

  static
  Line_2 compute_linf_perpendicular(const Line_2& l, const Point_2& p)
  {
    RT a, b, c;
    a = RT( - CGAL::sign(l.b()) );
    b = RT( CGAL::sign(l.a()) );
    c = - a * p.x() - b * p.y();
    return Line_2(a, b, c);
  }

  using Base::opposite_line;

  // philaris: similar to compute_supporting_line
  static
  Line_2 compute_line_from_to(const Point_2& p, const Point_2&q)
  {
    RT a, b, c;
    a = p.y() - q.y();
    b = q.x() - p.x();

    CGAL_assertion((CGAL::sign(a) != ZERO) or
                   (CGAL::sign(b) != ZERO))   ;

    c = p.x() * q.y() - q.x() * p.y();

    return Line_2(a, b, c);
  }

  static
  RT compute_linf_distance(const Point_2& p, const Point_2& q)
  {
    return CGAL::max(
              CGAL::abs(p.x() - q.x()),
              CGAL::abs(p.y() - q.y()));
  }

  static
  void compute_intersection_of_lines(
      const Line_2& l1, const Line_2& l2,
      RT& hx, RT& hy, RT& hw)
  {
    hx = l1.b() * l2.c() - l1.c() * l2.b();
    hy = l1.c() * l2.a() - l1.a() * l2.c();
    hw = l1.a() * l2.b() - l1.b() * l2.a();
  }

public:
  //-------------------------------------------------------------------
  // BASIC PREDICATES
  //-------------------------------------------------------------------
  static
  Comparison_result
  compare_linf_distances_to_line(const Line_2& l, const Point_2& p,
                                    const Point_2& q)
  {
    Homogeneous_point_2 hp = compute_linf_projection_hom(l, p);
    Homogeneous_point_2 hq = compute_linf_projection_hom(l, q);

    RT dlp = CGAL::max(CGAL::abs(p.x() - hp.x()),
                       CGAL::abs(p.y() - hp.y()));

    RT dlq = CGAL::max(CGAL::abs(q.x() - hq.x()),
                       CGAL::abs(q.y() - hq.y()));

    Comparison_result crude = CGAL::compare(dlp, dlq);

    if (crude != EQUAL) {
      return crude;
    } else {
      CGAL_SDG_DEBUG(std::cout
          << "compare_linf_distances_to_line refining"
          << std::endl;);
      return crude;
    }
  }

  static
  Comparison_result
  compare_linf_distances_to_lines(const Point_2& p,
				     const Line_2& l1,
                                     const Line_2& l2)
  {
    Homogeneous_point_2 hl1 = compute_linf_projection_hom(l1, p);
    Homogeneous_point_2 hl2 = compute_linf_projection_hom(l2, p);

    RT dl1p = CGAL::max(CGAL::abs(hl1.x() - p.x()),
                        CGAL::abs(hl1.y() - p.y()));

    RT dl2p = CGAL::max(CGAL::abs(hl2.x() - p.x()),
                        CGAL::abs(hl2.y() - p.y()));

    Comparison_result crude = CGAL::compare(dl1p, dl2p);

    if (crude != EQUAL) {
      return crude;
    } else {
      CGAL_SDG_DEBUG(std::cout << "compare_linf_distances_to_lines refining"
                     << std::endl;);
      return crude;
    }
  }

  using Base::oriented_side_of_line;

  using Base::is_on_positive_halfspace;

  static
  Comparison_result
  compare_distance_to_point_linf(
      const Point_2& p, const Point_2& q, const Point_2& r)
  {
    Comparison_result retval;
    retval =
      CGAL::compare(
        CGAL::max( CGAL::abs(p.x()-q.x()), CGAL::abs(p.y()-q.y()) ),
        CGAL::max( CGAL::abs(p.x()-r.x()), CGAL::abs(p.y()-r.y()) )
        );
    if (retval == EQUAL) {
      CGAL_SDG_DEBUG(std::cout <<
          "debug cmpdistlinf maybe break ties" << std::endl;);
      // here, p might be on the 2-dimensional bisector(q,r),
      // therefore we have to break ties, based on one coordinate
      if (CGAL::compare(q.x(), r.x()) == EQUAL) {
        CGAL_SDG_DEBUG(std::cout <<
            "debug cmpdistlinf try breaking with y" << std::endl;);
        return CGAL::compare( CGAL::abs(p.y()-q.y()),
                              CGAL::abs(p.y()-r.y()) ) ;
      } else if (CGAL::compare(q.y(), r.y()) == EQUAL) {
        CGAL_SDG_DEBUG(std::cout <<
            "debug cmpdistlinf try breaking with x" << std::endl;);
        return CGAL::compare( CGAL::abs(p.x()-q.x()),
                              CGAL::abs(p.x()-r.x()) ) ;
      }
    }
    return retval;
  }

  static
  Bounded_side
  bounded_side_of_bbox(
      const Point_2& p, const Point_2& q, const Point_2& r)
  {
    // precondition: p, q, r should be monotone points.
    // Predicate bounded_side_of_bbox (P_bbox) returns:
    //  0 if p = q,
    //  0 if r = p or r = q (ON_BOUNDARY),
    // -1 if r is strictly outside the bounding box of p,q
    //    (ON_UNBOUNDED_SIDE),
    // +1 if r is strictly inside the bounding box of p,q
    //    (ON_BOUNDED_SIDE).
    // If p and q are on the same vertical or horizontal
    // line but are not the same point, then the bounding
    // box of p and q degenerates to the line segment pq.

    CGAL_SDG_DEBUG(std::cout << "debug bounded_side_of_bbox (p q r) = ("
                   << p << ") (" << q << ") (" << r << ")" << std::endl; );

    if ((CGAL::compare(p.x(), q.x()) == EQUAL) and
        (CGAL::compare(p.y(), q.y()) == EQUAL)    ) {
      return ON_BOUNDARY;
    }

    Comparison_result cmpxpr, cmpxrq, cmpypr, cmpyrq;

    cmpxpr = CGAL::compare(p.x(), r.x());
    cmpxrq = CGAL::compare(r.x(), q.x());
    cmpypr = CGAL::compare(p.y(), r.y());
    cmpyrq = CGAL::compare(r.y(), q.y());

    Comparison_result comp =
      CGAL::compare(cmpxpr*cmpxrq + cmpypr*cmpyrq, 0);

    CGAL_SDG_DEBUG(std::cout << "debug bounded_side_of_bbox returns ";);

    switch(comp) {
      case SMALLER:
        CGAL_SDG_DEBUG(std::cout << "ON_UNBOUNDED_SIDE" << std::endl;);
        return ON_UNBOUNDED_SIDE;
      case EQUAL:
        CGAL_SDG_DEBUG(std::cout << "ON_BOUNDARY" << std::endl;);
        return ON_BOUNDARY;
      case LARGER:
        CGAL_SDG_DEBUG(std::cout << "ON_BOUNDED_SIDE" << std::endl;);
        return ON_BOUNDED_SIDE;
      default:
        CGAL_SDG_DEBUG(std::cout << "error: should never reach here";);
        CGAL_assertion( false );
        return ON_BOUNDARY;
    }
  }


  // returns true if and only if
  // the interior of s has non-empty intersection
  // with the positive halfplane of oriented line l
  static
  Boolean
  intersects_segment_positive_halfplane(
      const Site_2 & s,
      const Line_2 & l)
  {
    Segment_2 seg = s.segment();

    Point_2 ssrc = seg.source();
    Point_2 strg = seg.target();

    CGAL_SDG_DEBUG(std::cout
      << "debug: intersects_segment_positive_halfplane "
      << "s=" << s
      << " l=" << l.a() << " " << l.b() << " " << l.c()
                   << std::endl;);

    Oriented_side oslsrc = oriented_side_of_line(l, ssrc);
    Oriented_side osltrg = oriented_side_of_line(l, strg);

      CGAL_SDG_DEBUG(std::cout
          << "debug: intersects_segment_positive_halfplane "
          << "oslsrc=" << oslsrc << " osltrg=" << osltrg
                     << std::endl;);

    if ((oslsrc == ON_POSITIVE_SIDE) or
        (osltrg == ON_POSITIVE_SIDE)   )
    {
      return true;
    } else {
      return false;
    }
  }


  // returns true if and only if
  // the interior of s has non-empty intersection
  // with the negative halfplane of oriented line l
  static
  Boolean
  intersects_segment_negative_halfplane(
      const Site_2 & s,
      const Line_2 & l)
  {
    Segment_2 seg = s.segment();

    Point_2 ssrc = seg.source();
    Point_2 strg = seg.target();

    CGAL_SDG_DEBUG(std::cout
        << "debug: intersects_segment_negative_halfplane "
        << "s=" << s
        << " l=" << l.a() << " " << l.b() << " " << l.c()
        << std::endl;);

    Oriented_side oslsrc = oriented_side_of_line(l, ssrc);
    Oriented_side osltrg = oriented_side_of_line(l, strg);

    CGAL_SDG_DEBUG(std::cout
        << "debug: intersects_segment_negative_halfplane "
        << "oslsrc=" << oslsrc << " osltrg=" << osltrg
        << std::endl;);

    if ((oslsrc == ON_NEGATIVE_SIDE) or
        (osltrg == ON_NEGATIVE_SIDE)   )
    {
      return true;
    } else {
      return false;
    }
  }


  // returns true if and only if
  // the interior of s has non-empty intersection
  // with the interior of the following infinite box:
  // the only finite corner of the infinite box is corner
  // and if you traverse the infinite box ccw, then
  // you meet points in that order: q, corner, p
  static
  Boolean
  intersects_segment_interior_inf_box(const Site_2 & s,
      const Site_2 & q, const Point_2 & corner,
      const Site_2 & p)
  {
    Are_same_points_2 same_points;

    CGAL_assertion(s.is_segment());
    Segment_2 seg = s.segment();

    Point_2 ssrc = seg.source();
    Point_2 strg = seg.target();

    Point_2 qq = q.point();
    Point_2 pp = p.point();

    Line_2 lqc = compute_line_from_to(qq, corner);
    Line_2 lcp = compute_line_from_to(corner, pp);


    bool is_ssrc_positive;
    if (same_points(q, s.source_site()) or
        same_points(p, s.source_site())   ) {
      is_ssrc_positive = false;
    } else {
      Oriented_side os_lqc_ssrc = oriented_side_of_line(lqc, ssrc);
      Oriented_side os_lcp_ssrc = oriented_side_of_line(lcp, ssrc);
      is_ssrc_positive =
        ((os_lqc_ssrc == ON_POSITIVE_SIDE) and
         (os_lcp_ssrc == ON_POSITIVE_SIDE)    ) ;
    }

    bool is_strg_positive;
    if (same_points(q, s.target_site()) or
        same_points(p, s.target_site())   ) {
      is_strg_positive = false;
    } else {
      Oriented_side os_lqc_strg = oriented_side_of_line(lqc, strg);
      Oriented_side os_lcp_strg = oriented_side_of_line(lcp, strg);
      is_strg_positive =
        ((os_lqc_strg == ON_POSITIVE_SIDE) and
         (os_lcp_strg == ON_POSITIVE_SIDE)    ) ;
    }

    CGAL_SDG_DEBUG(std::cout << "debug qcp= (" << q << ") (" << corner
        << ") (" << p << ")"
        << " isssrcpos=" << is_ssrc_positive
        << " isstrgpos=" << is_strg_positive
        << std::endl;);

    if (is_ssrc_positive or is_strg_positive) {
      CGAL_SDG_DEBUG(std::cout << "debug is_segment_inside_inf_box "
                     << "endpoint inside" << std::endl;);
      return true;
    } else {
      // here you have to check if the interior is inside

      CGAL_SDG_DEBUG(std::cout << "debug is_segment_inside_inf_box "
                     << "try for interior to be inside" << std::endl;);

      // in fact, here you can intersect the segment
      // with the ray starting from corner and going to the
      // direction of the center of the infinite box

      Compare_x_2 cmpx;
      Compare_y_2 cmpy;

      Comparison_result cmpxpq = cmpx(pp,qq);
      Comparison_result cmpypq = cmpy(pp,qq);

      RT one(1);

      Point_2 displaced ( corner.x() + (-cmpypq)*one ,
                          corner.y() + cmpxpq * one   );

      Line_2 l = compute_line_from_to(corner, displaced);

      Line_2 lseg = compute_supporting_line(s.supporting_site());

      RT hx, hy, hw;

      compute_intersection_of_lines(l, lseg, hx, hy, hw);

      if (CGAL::sign(hw) == ZERO) {
        return false;
      } else {
        Point_2 ip ( hx/hw, hy/hw);
        Oriented_side os_lqc_ip = oriented_side_of_line(lqc, ip);
        Oriented_side os_lcp_ip = oriented_side_of_line(lcp, ip);

        Compare_x_2 cmpx;
        Compare_y_2 cmpy;

        Comparison_result cmpxsrcip = cmpx(ssrc, ip);
        Comparison_result cmpysrcip = cmpy(ssrc, ip);
        Comparison_result cmpxiptrg = cmpx(ip, strg);
        Comparison_result cmpyiptrg = cmpy(ip, strg);

        // philaris: to check
        Boolean is_ip_inside_segment =
          (CGAL::sign(cmpxsrcip * cmpxiptrg +
                      cmpysrcip * cmpyiptrg   )) == POSITIVE;

        if ((os_lqc_ip == ON_POSITIVE_SIDE) and
            (os_lcp_ip == ON_POSITIVE_SIDE) and
            is_ip_inside_segment ) {
          return true;
        } else {
          return false;
        }
      }
    }
  } // end of intersects_segment_interior_inf_box


  // returns true if and only if
  // the interior of s has non-empty intersection
  // with the interior of the following infinite box:
  // this infinite box is a 90 degree wedge defined
  // by the intersection of the halfplanes
  // with supporting lines lhor and lver, where
  // the halfplanes are both on the positive or negative
  // sides of the supporting lines
  static
  Boolean
  intersects_segment_side_of_wedge(const Site_2 & s,
      const Line_2 & lhor, const Line_2 & lver,
      Oriented_side orside)
  {
    CGAL_assertion(s.is_segment());
    Segment_2 seg = s.segment();

    CGAL_SDG_DEBUG(std::cout << "debug sofw s=" << s
                   << " orside=" << orside << std::endl;);

    Point_2 ssrc = seg.source();
    Point_2 strg = seg.target();

    Oriented_side os_lhor_ssrc = oriented_side_of_line(lhor, ssrc);
    Oriented_side os_lver_ssrc = oriented_side_of_line(lver, ssrc);

    Oriented_side os_lhor_strg = oriented_side_of_line(lhor, strg);
    Oriented_side os_lver_strg = oriented_side_of_line(lver, strg);

    if (((os_lhor_ssrc == orside) and
         (os_lver_ssrc == orside)) or
        ((os_lhor_strg == orside) and
         (os_lver_strg == orside))   ) {
          CGAL_SDG_DEBUG(std::cout
              << "debug intersects_segment_side_of_wedge "
              << "endpoint inside" << std::endl;);
      return true;
    } else {
      // here we have to check if the interior is inside

      CGAL_SDG_DEBUG(std::cout
          << "debug intersects_segment_side_of_wedge "
          << "try for interior to be inside" << std::endl;);

      // in fact, here you can intersect the segment
      // with the ray starting from corner and going to the
      // direction of the center of the infinite box

      // corner has homogenuous coordinates cx, cy, cw
      RT cx, cy, cw;
      compute_intersection_of_lines(lhor, lver, cx, cy, cw);

      CGAL_assertion( CGAL::sign(cw) != ZERO );

      Point_2 corner ( cx, cy, cw );

      CGAL_SDG_DEBUG(std::cout << "debug corner=" << corner << std::endl;);

      RT one(1);

      Point_2 displaced (
          corner.x() + ( (+orside)*CGAL::sign(lver.a()) ) * one ,
          corner.y() +   (+orside)*CGAL::sign(lhor.b())   * one   );

      CGAL_SDG_DEBUG(std::cout
          << "debug displaced=" << displaced << std::endl;);

      Line_2 l = compute_line_from_to(corner, displaced);

      Line_2 lseg = compute_supporting_line(s.supporting_site());

      RT hx, hy, hw;

      CGAL_SDG_DEBUG(std::cout
          << "debug: intersects_segment_side_of_wedge "
          << " l=" << l.a() << " " << l.b() << " " << l.c()
          << " lseg=" << lseg.a() << " " << lseg.b() << " " << lseg.c()
          << std::endl;);

      compute_intersection_of_lines(l, lseg, hx, hy, hw);

      if (CGAL::sign(hw) == ZERO) {
        CGAL_SDG_DEBUG(std::cout
            << "debug l and lseg are parallel" << std::endl;);
        return false;
      } else {
        Point_2 ip ( hx, hy, hw );
        CGAL_SDG_DEBUG(std::cout << "debug ip=" << ip << std::endl;);
        Oriented_side os_lhor_ip = oriented_side_of_line(lhor, ip);
        Oriented_side os_lver_ip = oriented_side_of_line(lver, ip);

        Compare_x_2 cmpx;
        Compare_y_2 cmpy;

        Comparison_result cmpxsrcip = cmpx(ssrc, ip);
        Comparison_result cmpysrcip = cmpy(ssrc, ip);
        Comparison_result cmpxiptrg = cmpx(ip, strg);
        Comparison_result cmpyiptrg = cmpy(ip, strg);

        // philaris: to check
        Boolean is_ip_inside_segment =
          (CGAL::sign(cmpxsrcip * cmpxiptrg +
                      cmpysrcip * cmpyiptrg   )) == POSITIVE;

        if ((os_lhor_ip == orside) and
            (os_lver_ip == orside) and
            is_ip_inside_segment      ) {
          return true;
        } else {
          return false;
        }
      }
    }
  } // end of intersects_segment_side_of_wedge

  // returns true if and only if
  // the interior of s has non-empty intersection
  // with the interior of the following infinite box:
  // this infinite box is a 90 degree wedge defined
  // by the intersection of the positive halfplanes
  // with supporting lines lhor and lver
  static
  Boolean
  intersects_segment_positive_of_wedge(const Site_2 & s,
      const Line_2 & lhor, const Line_2 & lver)
  {
    return intersects_segment_side_of_wedge(
        s, lhor, lver, ON_POSITIVE_SIDE);
  }

  // returns true if and only if
  // the interior of s has non-empty intersection
  // with the interior of the following infinite box:
  // this infinite box is a 90 degree wedge defined
  // by the intersection of the positive halfplanes
  // with supporting lines lhor and lver
  static
  Boolean
  intersects_segment_negative_of_wedge(const Site_2 & s,
      const Line_2 & lhor, const Line_2 & lver)
  {
    return intersects_segment_side_of_wedge(
        s, lhor, lver, ON_NEGATIVE_SIDE);
  }

  // returns true if and only if
  // the interior of t has non-empty intersection
  // with the interior of the following infinite box:
  // the only finite corner of the infinite box is p
  // and this infinite box is a 90 degree wedge defined
  // by the intersection of the halfplanes
  // with supporting lines lhor and lver through p, where
  // the halfplanes are both on the positive or negative
  // sides of the supporting lines. The positive or negative
  // side depends on the relative position of p with respect to s

  static
  Boolean
  intersects_segment_interior_inf_wedge_sp(const Site_2 & s,
                                           const Site_2 & p,
                                           const Site_2 & t)
  {
    CGAL_assertion( t.is_segment() );
    CGAL_assertion( s.is_segment() );
    CGAL_assertion(not is_site_h_or_v(s));

    Segment_2 seg = s.segment();

    Point_2 ssrc = seg.source();
    Point_2 strg = seg.target();

    Point_2 pp = p.point();

    Sign dxs = CGAL::sign(strg.x() - ssrc.x());
    Sign dys = CGAL::sign(strg.y() - ssrc.y());

    Line_2 lseg = compute_supporting_line(s.supporting_site());
    Oriented_side os_lseg_p = oriented_side_of_line(lseg, pp);

    CGAL_assertion( os_lseg_p != ON_ORIENTED_BOUNDARY );

    //sandeep: lhor, lver remains same for left turn and right turn

    if (dxs == NEGATIVE and dys == NEGATIVE) {

      Line_2 lhor = Line_2(0,1,-pp.y());
      Line_2 lver = Line_2(-1,0,pp.x());

      return intersects_segment_side_of_wedge(t,
                                       lhor, lver,
                                       os_lseg_p);
    } else if (dxs == POSITIVE and dys == NEGATIVE) {

      Line_2 lhor = Line_2(0,-1,pp.y());
      Line_2 lver = Line_2(-1,0,pp.x());

      return intersects_segment_side_of_wedge(t,
                                       lhor, lver,
                                       os_lseg_p);
    } else if (dxs == POSITIVE and dys == POSITIVE) {

      Line_2 lhor = Line_2(0,-1,pp.y());
      Line_2 lver = Line_2(1,0,-pp.x());

      return intersects_segment_side_of_wedge(t,
                                       lhor, lver,
                                       os_lseg_p);
    } else {//dxs == NEGATIVE and dys == POSITIVE

      Line_2 lhor = Line_2(0,1,-pp.y());
      Line_2 lver = Line_2(1,0,-pp.x());

      return intersects_segment_side_of_wedge(t,
                                       lhor, lver,
                                       os_lseg_p);
    }

  } // end of intersects_segment_interior_inf_wedge_sp


  // returns true if and only if
  // the interior of s has non-empty intersection
  // with the interior of the bounding box of q, p
  // precondition: the bounding box should be non-trivial,
  // i.e., it should not be a segment
  static
  Boolean
  intersects_segment_interior_bbox(const Site_2 & s,
      const Site_2 & q,
      const Site_2 & p)
  {
    CGAL_precondition(s.is_segment());
    CGAL_precondition(p.is_point());
    CGAL_precondition(q.is_point());

    Compare_x_2 cmpx;
    Compare_y_2 cmpy;

    Point_2 pp = p.point();
    Point_2 qq = q.point();

    CGAL_assertion(cmpx(pp,qq) != EQUAL);
    CGAL_assertion(cmpy(pp,qq) != EQUAL);

    Point_2 corner1 ( pp.x(), qq.y());
    Point_2 corner2 ( qq.x(), pp.y());

    if (CGAL::orientation( qq, corner1, pp ) == LEFT_TURN) {
      return intersects_segment_interior_inf_box(s, q, corner1, p)
         and intersects_segment_interior_inf_box(s, p, corner2, q);
    } else {
      return intersects_segment_interior_inf_box(s, q, corner2, p)
         and intersects_segment_interior_inf_box(s, p, corner1, q);
    }
  } // end of intersects_segment_interior_bbox

  // returns true if and only if
  // the non-horizontal/non-vertical segment at site s
  // has positive slope
  static
  Boolean
  has_positive_slope(const Site_2 & s)
  {
    CGAL_precondition(s.is_segment());
    CGAL_precondition(not is_site_h_or_v(s));
    Compare_x_2 cmpx;
    Compare_y_2 cmpy;
    Point_2 src = s.supporting_site().source();
    Point_2 trg = s.supporting_site().target();
    return cmpx(src, trg) == cmpy(src, trg);
  }

  static
  Boolean
  is_site_h_or_v(const Site_2 & s)
  {
    return is_site_horizontal(s) or is_site_vertical(s);
  }

  static
  Boolean
  is_site_horizontal(const Site_2 & s)
  {
    CGAL_assertion(s.is_segment());
    return s.supporting_site().segment().is_horizontal();
  }

  static
  Boolean
  is_site_vertical(const Site_2 & s)
  {
    CGAL_assertion(s.is_segment());
    return s.supporting_site().segment().is_vertical();
  }

};



} //namespace SegmentDelaunayGraphLinf_2

} //namespace CGAL


#endif // CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_BASIC_PREDICATES_C2_H
