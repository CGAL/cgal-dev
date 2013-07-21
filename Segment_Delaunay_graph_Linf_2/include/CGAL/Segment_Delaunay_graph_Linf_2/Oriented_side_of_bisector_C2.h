
#ifndef CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_ORIENTED_SIDE_OF_BISECTOR_C2_H
#define CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_ORIENTED_SIDE_OF_BISECTOR_C2_H

#include <CGAL/Segment_Delaunay_graph_Linf_2/Basic_predicates_C2.h>
#include <CGAL/Segment_Delaunay_graph_2/Are_same_points_C2.h>
#include <CGAL/Segment_Delaunay_graph_2/Are_same_segments_C2.h>

namespace CGAL {

namespace SegmentDelaunayGraphLinf_2 {

//------------------------------------------------------------------------
//------------------------------------------------------------------------

template<class K, class Method_tag>
class Oriented_side_of_bisector_C2
  : public Basic_predicates_C2<K>
{
private:
  typedef Basic_predicates_C2<K>             Base;
  typedef typename Base::Line_2              Line_2;
  typedef typename Base::RT                  RT;
  typedef typename Base::FT                  FT;
  typedef typename Base::Comparison_result   Comparison_result;
  typedef typename Base::Sign                Sign;
  typedef typename Base::Orientation         Orientation;
  typedef typename Base::Homogeneous_point_2 Homogeneous_point_2;

  typedef SegmentDelaunayGraph_2::Are_same_points_C2<K>   Are_same_points_2;
  typedef SegmentDelaunayGraph_2::Are_same_segments_C2<K> Are_same_segments_2;

private:
  Are_same_points_2    same_points;
  Are_same_segments_2  same_segments;

public:
  typedef typename Base::Oriented_side      Oriented_side;
  typedef typename Base::Site_2             Site_2;
  typedef typename Base::Point_2            Point_2;
  typedef typename Base::Segment_2          Segment_2;

  using Base::compute_supporting_line;
  using Base::compute_linf_perpendicular;
  using Base::compute_linf_projection_hom;
  using Base::opposite_line;
  using Base::oriented_side_of_line;
  using Base::compute_linf_distance;
  using Base::compare_distance_to_point_linf;

private:
  bool is_endpoint(const Site_2& p, const Site_2& s) const
  {
    CGAL_precondition( p.is_point() && s.is_segment() );
    return
      same_points(p, s.source_site()) || same_points(p, s.target_site());
  }

  bool is_degenerate(const Site_2& s) const
  {
    CGAL_precondition( s.is_segment() );
    return same_points( s.source_site(), s.target_site() );
  }

  //-----------------------------------------------------------------

  Comparison_result
  compare_distances_pp(const Site_2& p1, const Site_2& p2,
		       const Site_2& q) const
  {
    CGAL_precondition( p1.is_point() && p2.is_point() );
    CGAL_precondition( !same_points(p1,p2) );

    if ( same_points(q, p1) ) { return SMALLER; }
    if ( same_points(q, p2) ) { return LARGER; }

    //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_pp" << std::endl;);

    return
      compare_distance_to_point_linf(q.point(), p1.point(), p2.point());
  }

  //-----------------------------------------------------------------

  Comparison_result
  compare_distances_sp(const Site_2& s, const Site_2& p,
		       const Site_2& q) const
  {

    //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp entering "
    //  << "(s =" << s << ") p=(" << p << ") q=(" << q << ")"
    //  << std::endl;);

    CGAL_precondition( s.is_segment() && p.is_point() );
    CGAL_precondition( !is_degenerate(s) );

    if ( same_points(q, p) ) { return LARGER; }
    if ( same_points(q, s.source_site()) ) { return SMALLER; }
    if ( same_points(q, s.target_site()) ) { return SMALLER; }


    bool is_src = same_points(p, s.source_site());
    bool is_trg = same_points(p, s.target_site());

    if ( is_src || is_trg ) {
      Line_2 ls = compute_supporting_line(s.supporting_site());
      Line_2 lp = compute_linf_perpendicular(ls, p.point());

      if ( is_trg ) {
	lp = opposite_line(lp);
      }

      Oriented_side os = oriented_side_of_line(lp, q.point());

      CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp "
          << " is_src=" << is_src << " is_trg=" << is_trg
          << " has os=" << os << std::endl;);

      if ( os == ON_POSITIVE_SIDE ) {
	return LARGER;
      } else if ( os == ON_NEGATIVE_SIDE) {
	return SMALLER;
      } else {
	return EQUAL;
      }
    }

    Point_2 pp = p.point(), qq = q.point();

    // here, we have to compute closest point of segment to q

    //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp compute closest " << std::endl;);

    Point_2 closest;
    bool set_closest = false;

    Point_2 ssrc = s.source(), strg = s.target();

    Line_2 ls = compute_supporting_line(s.supporting_site());
    //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp ls="
    //  << ls.a() << ' ' << ls.b() << ' ' << ls.c() << std::endl;);

    Line_2 lsrc = compute_linf_perpendicular(ls, ssrc);
    //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp lsrc="
    //  << lsrc.a() << ' ' << lsrc.b() << ' ' << lsrc.c() << std::endl;);

    Line_2 ltrg = compute_linf_perpendicular(ls, strg);
    //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp lstrg="
    //  << ltrg.a() << ' ' << ltrg.b() << ' ' << ltrg.c() << std::endl;);

    Oriented_side os_src = oriented_side_of_line(lsrc, qq);
    if ( os_src != ON_NEGATIVE_SIDE ) {
      // ssrc is closest point to q
      //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp closest=ssrc" << std::endl;);
      closest = ssrc;
      set_closest = true;
    }

    Oriented_side os_trg = oriented_side_of_line(ltrg, qq);
    if ( os_trg != ON_POSITIVE_SIDE ) {
      // strg is closest point to q
      //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp closest=strg" << std::endl;);
      closest = strg;
      set_closest = true;
    }

    // here closest point of s to q is inside s
    if (not set_closest) {
      //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp closest inside"
      //  << std::endl;);
      Homogeneous_point_2 hp = compute_linf_projection_hom(ls, qq);
      closest = Point_2(hp.x(), hp.y());
    }

    //CGAL_SDG_DEBUG(std::cout << "debug compare_distances_sp closest="
    //  << closest << std::endl;);

    return
      compare_distance_to_point_linf(qq, closest, pp);
  }

  //-----------------------------------------------------------------

  Comparison_result
  compare_distances_ss(const Site_2& s1, const Site_2& s2,
		       const Site_2& q) const
  {
    CGAL_precondition( s1.is_segment() && s2.is_segment() );
    CGAL_precondition( !is_degenerate(s1) );
    CGAL_precondition( !is_degenerate(s2) );

    CGAL_SDG_DEBUG(std::cout << "debug compare_distances_ss "
        << "entering with s1=" << s1 << " s2=" << s2
        << " q=" << q << std::endl;);

    bool is_on_s1 = is_endpoint(q, s1);
    bool is_on_s2 = is_endpoint(q, s2);

    if ( is_on_s1 && is_on_s2 ) {
      return EQUAL;
    } else if ( is_on_s1 && !is_on_s2 ) {
      return SMALLER;
    } else if ( !is_on_s1 && is_on_s2 ) {
      return LARGER;
    }

    if ( same_segments(s1, s2) ) {
      return EQUAL;
    }


    Point_2 qq = q.point();

    Point_2 ssrc1 = s1.source(), strg1 = s1.target();

    Line_2 ls1 = compute_supporting_line(s1.supporting_site());
    Line_2 lsrc1 = compute_linf_perpendicular(ls1, ssrc1);
    Line_2 ltrg1 = compute_linf_perpendicular(ls1, strg1);

    Point_2 ssrc2 = s2.source(), strg2 = s2.target();

    Line_2 ls2 = compute_supporting_line(s2.supporting_site());
    Line_2 lsrc2 = compute_linf_perpendicular(ls2, ssrc2);
    Line_2 ltrg2 = compute_linf_perpendicular(ls2, strg2);

    // idx1 and idx2 indicate if q is to the left (source endpoint
    // side), the right side (target endpoint side) or inside
    // the band of s1 and s2 respectively; if q is on the boundary of
    // the band we assign it to the adjacent halfplane; for left
    // halfplane the value is -1; for the band the value is 0; for the
    // right halfplane the value is 1.
    int idx1(0), idx2(0);

    Oriented_side os_src1 = oriented_side_of_line(lsrc1, qq);
    Oriented_side os_trg1;
    if ( os_src1 != ON_NEGATIVE_SIDE ) {
      idx1 = -1;
      os_trg1 = ON_POSITIVE_SIDE;
    } else {
      os_trg1 = oriented_side_of_line(ltrg1, qq);
      if ( os_trg1 != ON_POSITIVE_SIDE ) {
	idx1 = 1;
      }
    }

    Oriented_side os_src2 = oriented_side_of_line(lsrc2, qq);
    Oriented_side os_trg2;
    if ( os_src2 != ON_NEGATIVE_SIDE ) {
      idx2 = -1;
      os_trg2 = ON_POSITIVE_SIDE;
    } else {
      os_trg2 = oriented_side_of_line(ltrg2, qq);
      if ( os_trg2 != ON_POSITIVE_SIDE ) {
	idx2 = 1;
      }
    }

    CGAL_SDG_DEBUG(std::cout << "debug compare_distances_ss "
        << " os_src1=" << os_src1 << " os_trg1=" << os_trg1
        << " os_src2=" << os_src2 << " os_trg2=" << os_trg2 << std::endl;);

    CGAL_SDG_DEBUG(std::cout << "debug compare_distances_ss "
        << " idx=" << idx1 << " idx2=" << idx2 << std::endl;);

    CGAL_assertion( idx1 >= -1 && idx1 <= 1 );
    CGAL_assertion( idx2 >= -1 && idx2 <= 1 );

    Point_2 closest1;
    Point_2 closest2;

    if ( idx1 == -1 ) {
      //RT d2_s1 = compute_linf_distance(qq, ssrc1);
      closest1 = ssrc1;
      if ( idx2 == -1 ) {
	if ( same_points(s1.source_site(), s2.source_site()) ) {
          CGAL_SDG_DEBUG(std::cout << "debug compare_distances_ss"
              << " same_s1src_s2src" << std::endl;);
	  return EQUAL;
	}
	//RT d2_s2 = compute_linf_distance(qq, ssrc2);
        closest2 = ssrc2;
	//return CGAL::compare(d2_s1, d2_s2);
      } else if ( idx2 == 1 ) {
	if ( same_points(s1.source_site(), s2.target_site()) ) {
          CGAL_SDG_DEBUG(std::cout << "debug compare_distances_ss"
              << " same_s1src_s2trg, closest1=" << closest1 << std::endl;);
	  return EQUAL;
	}
	//RT d2_s2 = compute_linf_distance(qq, strg2);
        closest2 = strg2;
	//return CGAL::compare(d2_s1, d2_s2);
      } else {
        // here closest2 is inside segment s2
        Homogeneous_point_2 hp2 = compute_linf_projection_hom(ls2, qq);
        closest2 = Point_2(hp2.x(), hp2.y());
	//return CGAL::compare(d2_s1 * d2_s2.second, d2_s2.first);
      }
    } else if ( idx1 == 1 ) {
      //RT d2_s1 = compute_linf_distance(qq, strg1);
      closest1 = strg1;
      if ( idx2 == -1 ) {
	if ( same_points(s1.target_site(), s2.source_site()) ) {
          CGAL_SDG_DEBUG(std::cout << "debug compare_distances_ss"
              << " same_s1trg_s2src" << std::endl;);
	  return EQUAL;
	}
	//RT d2_s2 = compute_linf_distance(qq, ssrc2);
        closest2 = ssrc2;
	//return CGAL::compare(d2_s1, d2_s2);
      } else if ( idx2 == 1 ) {
	if ( same_points(s1.target_site(), s2.target_site()) ) {
          CGAL_SDG_DEBUG(std::cout << "debug compare_distances_ss"
              << " same_s1trg_s2trg" << std::endl;);
	  return EQUAL;
	}
	//RT d2_s2 = compute_linf_distance(qq, strg2);
        closest2 = strg2;
	//return CGAL::compare(d2_s1, d2_s2);
      } else {
        // here closest2 is inside segment s2
        Homogeneous_point_2 hp2 = compute_linf_projection_hom(ls2, qq);
        closest2 = Point_2(hp2.x(), hp2.y());
	//return CGAL::compare(d2_s1 * d2_s2.second, d2_s2.first);
      }
    } else {
      // here closest1 is inside segment s1
      CGAL_assertion( idx1 == 0 );
      Homogeneous_point_2 hp1 = compute_linf_projection_hom(ls1, qq);
      closest1 = Point_2(hp1.x(), hp1.y());
      if ( idx2 == -1 ) {
        //RT d2_s2 = compute_linf_distance(qq, ssrc2);
        closest2 = ssrc2;
        //return CGAL::compare(d2_s1.first, d2_s2 * d2_s1.second);
      } else if ( idx2 == 1 ) {
        //RT d2_s2 = compute_linf_distance(qq, strg2);
        closest2 = strg2;
        //return CGAL::compare(d2_s1.first, d2_s2 * d2_s1.second);
      } else {
        CGAL_assertion( idx2 == 0 );
        Homogeneous_point_2 hp2 = compute_linf_projection_hom(ls2, qq);
        closest2 = Point_2(hp2.x(), hp2.y());
        //return CGAL::compare(d2_s1.first * d2_s2.second,
        //    d2_s2.first * d2_s1.second);
      }
    }

    return
      compare_distance_to_point_linf(qq, closest1, closest2);
  }

  //-----------------------------------------------------------------
  //-----------------------------------------------------------------

  Oriented_side
  compare_distances(const Site_2& t1, const Site_2& t2,
		    const Site_2& q) const
  {
    Comparison_result r;

    if ( t1.is_point() && t2.is_point() ) {
      r = compare_distances_pp(t1, t2, q);
      //CGAL_SDG_DEBUG(std::cout << "debug compare_distances pp result"
      //          << " t1=" << t1 << " t2=" << t2
      //          << " q=" << q << "  res=" << r << std::endl;);
    } else if ( t1.is_segment() && t2.is_point() ) {
      r = compare_distances_sp(t1, t2, q);
      //CGAL_SDG_DEBUG(std::cout << "debug compare_distances sp result"
      //          << " t1=" << t1 << " t2=" << t2
      //          << " q=" << q << "  res=" << r << std::endl;);
    } else if ( t1.is_point() && t2.is_segment() ) {
      r = opposite( compare_distances_sp(t2, t1, q) );
    } else {
      r = compare_distances_ss(t1, t2, q);
    }

    return -r;
  }

public:
  typedef Oriented_side              result_type;
  typedef Site_2                     argument_type;


  Oriented_side
  operator()(const Site_2& t1, const Site_2& t2, const Site_2& q) const
  {
    CGAL_precondition( q.is_point() );
    return compare_distances(t1, t2, q);
  }
};

} //namespace SegmentDelaunayGraphLinf_2

} //namespace CGAL


#endif // CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_ORIENTED_SIDE_OF_BISECTOR_C2_H
