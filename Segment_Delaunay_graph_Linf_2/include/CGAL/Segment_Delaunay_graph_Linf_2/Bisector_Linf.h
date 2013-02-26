#ifndef CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_BISECTOR_LINF_H
#define CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_BISECTOR_LINF_H

#include <CGAL/Segment_Delaunay_graph_Linf_2/basic.h>
#include <CGAL/Polychain_2.h>
#include <CGAL/Segment_Delaunay_graph_2/Are_parallel_C2.h>
#include <CGAL/Segment_Delaunay_graph_2/Are_same_points_C2.h>

namespace CGAL {

namespace SegmentDelaunayGraphLinf_2 {

template< class K >
class Bisector_Linf
{

public:

  typedef typename K::Site_2    Site_2;

  typedef CGAL::Polychainline_2<K> Polychainline;
  typedef Polychainline            result_type;
  typedef Site_2                   argument_type;


private:
  typedef typename K::Segment_2 Segment_2;
  typedef typename K::FT        FT;
  typedef typename K::RT        RT;

  typedef typename K::Line_2             Full_Line_2;
  typedef typename K::Point_2            Point_2;
  typedef typename K::Direction_2        Direction_2;
  typedef typename K::Vector_2           Vector_2;
  typedef typename K::Sign               Sign;

  typedef typename K::Compare_x_2  Compare_x_2;
  typedef typename K::Compare_y_2  Compare_y_2;

  typedef SegmentDelaunayGraph_2::Are_parallel_C2<K>    Are_parallel_2;
  typedef SegmentDelaunayGraph_2::Are_same_points_C2<K> Are_same_points_2;
  Compare_x_2 compare_x_2;
  Compare_y_2 compare_y_2;

private:
  result_type bisector(const Site_2& p, const Site_2& q) const {
    if (p.is_point() and q.is_point()) {
      return bisector_PP(p, q);
    } else if (p.is_point() and q.is_segment()) {
      return bisector_PS(p, q);
    } else if (p.is_segment() and q.is_point()) {
      return - bisector_PS(q, p);
    } else { // p.is_segment() and q.is_segment()
      return bisector_SS(p, q);
    }
  }

  result_type bisector_PP(const Site_2& p, const Site_2& q) const {
    // adapt from constructions
    CGAL_assertion(p.is_point() and q.is_point());
    Point_2 pp = p.point();
    Point_2 pq = q.point();
    //CGAL_SDG_DEBUG(std::cout << "debug bisector infinite "
    //          << "p=" << pp << " q=" << pq << std::endl;);

    Compare_x_2 compare_x_2;
    Compare_y_2 compare_y_2;
    Are_same_points_2 are_same_points;//the arguments are sites
    CGAL_assertion( !(are_same_points(p, q)) );
    Comparison_result cmpx = compare_x_2(pp, pq);
    Comparison_result cmpy = compare_y_2(pp, pq);
    Comparison_result cmpabsdxy =
      CGAL::compare( CGAL::abs(pp.x()-pq.x()),
                     CGAL::abs(pp.y()-pq.y()) );
    unsigned int npts;
    Point_2 points[2];

    // (final) direction of bisector d=(-cmpy, cmpx)
    // the bisector should leave p to the right and q to the left
    Direction_2 d (
                   (cmpy == EQUAL)? 0 :
                   (  cmpy  == SMALLER )? +1 : -1,
                   (cmpx == EQUAL)? 0 :
                   (  cmpx  == SMALLER )? -1 : +1);

    //CGAL_SDG_DEBUG(std::cout << "debug: final direction d = "
    //    << d << std::endl;) ;

    // midpoint m of two points p and q
    Point_2 m = midpoint(pp, pq);

    if ((cmpabsdxy == EQUAL) or (cmpx == EQUAL) or (cmpy == EQUAL)) {
      // bisector is line going through m with direction d;
      // we will store this line as the union of two rays starting
      // at m with directions -d (incoming) and d (outgoing)
      npts = 1;
      points[0] = m;
    } else {
      // bisector consists of two rays and a middle segment;

      npts = 2;

      // compute length of middle segment
      FT half(0.5);
      FT seglenhalf ( half *
                     CGAL::abs(
                               CGAL::abs(pp.x()-pq.x()) -
                               CGAL::abs(pp.y()-pq.y()))   );

      // construct endpoints of middle segment of bisector
      Point_2 p1, p2;
      if (cmpabsdxy == SMALLER) {
        // middle segment is horizontal
        Point_2 p1temp (m.x() - seglenhalf, m.y());
        Point_2 p2temp (m.x() + seglenhalf, m.y());
        p1 = p1temp;
        p2 = p2temp;
      } else { // cmpabsdxy is LARGER
        // middle segment is vertical
        Point_2 p1temp (m.x(), m.y() - seglenhalf);
        Point_2 p2temp (m.x(), m.y() + seglenhalf);
        p1 = p1temp;
        p2 = p2temp;
      }

      // swap endpoints of segment if necessary
      if ( (cmpabsdxy == SMALLER ? cmpy : -cmpx) == LARGER ) {
        std::swap(p1, p2);
      }

      points[0] = p1;
      points[1] = p2;
    }

    Polychainline pcl(-d, points, points+npts, d);

    //CGAL_SDG_DEBUG(std::cout <<
    //    "debug bisector is " << pcl << std::endl;);

    return pcl;

  }

  result_type bisector_PS(const Site_2& p, const Site_2& q) const {

    CGAL_SDG_DEBUG(std::cout << "bisector_PS entering with p=" << p
              << " q=" << q << std::endl;);

    CGAL_assertion(p.is_point() and q.is_segment());
    Point_2 pnt = p.point();
    Segment_2 seg = q.segment();
    Full_Line_2 lseg = q.supporting_site().segment().supporting_line();

    Are_same_points_2 same_points;
    Compare_x_2 compare_x_2;
    Compare_y_2 compare_y_2;

    if (same_points(p,q.source_site()) or
        same_points(p,q.target_site())) {
      //p must be one of the end point of segment q,
      //and the bisector is a line passing through p
      Point_2 points[1];
      unsigned int npts = 1;

      points[0] = pnt;

      Point_2 pq = (same_points(p,q.source_site())) ?
                    seg.target() : seg.source();

      Comparison_result cmpx = compare_x_2(pnt, pq);
      Comparison_result cmpy = compare_y_2(pnt, pq);

      Direction_2 d (
                     (cmpy == EQUAL)? 0 :
                     (  cmpy  == SMALLER )? +1 : -1,
                     (cmpx == EQUAL)? 0 :
                     (  cmpx  == SMALLER )? -1 : +1);

      Polychainline pcl(-d, points, points+npts, d);
      return pcl;

    }
    else {
      Oriented_side side = lseg.oriented_side(pnt);

      // point pp sould not lie on the supporting line of q
      CGAL_assertion(not (side == ON_ORIENTED_BOUNDARY));

      Point_2 points[3];
      unsigned int npts;
      if (q.supporting_site().segment().is_horizontal()) {
        // segment site is horizontal
        // pver is vertical projection from point site on to segment site
        npts = 2;
        Point_2 pver;
        pver = Point_2(pnt.x(), lseg.y_at_x(pnt.x()));

        Point_2 m = midpoint(pnt, pver);
        FT half = FT(0.5);
        FT seglenhalf ( half * CGAL::abs(pnt.y()-pver.y()) );

        Direction_2 dinc, dout;

        Comparison_result cmp = compare_y_2(pnt, pver);
        if (cmp == LARGER) {
          points[0] = Point_2(m.x() + seglenhalf, m.y());
          points[1] = Point_2(m.x() - seglenhalf, m.y());
          dinc = Direction_2 ( +1, +1 );
          dout = Direction_2 ( -1, +1 );
        } else {
          CGAL_assertion(cmp == SMALLER);
          points[0] = Point_2(m.x() - seglenhalf, m.y());
          points[1] = Point_2(m.x() + seglenhalf, m.y());
          dinc = Direction_2 ( -1, -1 );
          dout = Direction_2 ( +1, -1 );
        }

        Polychainline pcl(dinc, points, points+npts, dout);
        return pcl;
      }//end of horizontal segment case
      else if(q.supporting_site().segment().is_vertical()) {
        //segment site is vertical
        // phor is the projection of pnt on seg
        npts = 2;
        Point_2 phor;
        phor = Point_2(lseg.x_at_y(pnt.y()), pnt.y());

        Point_2 m = midpoint(pnt, phor);
        FT half = FT(0.5);
        FT seglenhalf ( half * CGAL::abs(pnt.x()-phor.x()) );

        Direction_2 dinc, dout;

        Comparison_result cmp = compare_x_2(pnt, phor);
        if (cmp == LARGER) {
          points[0] = Point_2(m.x(), m.y() - seglenhalf);
          points[1] = Point_2(m.x(), m.y() + seglenhalf);
          dinc = Direction_2 ( +1, -1 );
          dout = Direction_2 ( +1, +1 );
        } else {
          points[0] = Point_2(m.x(), m.y() + seglenhalf);
          points[1] = Point_2(m.x(), m.y() - seglenhalf);
          dinc = Direction_2 ( -1, +1 );
          dout = Direction_2 ( -1, -1 );
        }

        Polychainline pcl(dinc, points, points+npts, dout);
        return pcl;
      }// end of the vertical segment case
      else {//the segment is neither horizontal nor vertical
        Point_2 phor,pver;
        phor = Point_2(lseg.x_at_y(pnt.y()), pnt.y());
        pver = Point_2(pnt.x(), lseg.y_at_x(pnt.x()));
        //pfirst and plast are points on the supporting line of seg
        Point_2 pfirst, plast;
        //pcfirst and pclast are points on the bisector
        Point_2 pcfirst, pclast;

        // segment with positive slope will have pfirst as phor
        // segment with negative slope will have pfirst as pver
        bool has_lseg_pos_slope =
          CGAL::sign(lseg.a()) != CGAL::sign(lseg.b());
        pfirst = has_lseg_pos_slope ? phor : pver;
        plast  = has_lseg_pos_slope ? pver : phor;

        FT half = FT(0.5);
        Point_2 pmid_pfirst_pnt = midpoint(pfirst, pnt);
        Point_2 pmid_plast_pnt = midpoint(plast, pnt);
        FT seglenhalffirst ( half *
                            CGAL::abs(
                                      CGAL::abs(pnt.x()-pfirst.x()) -
                                      CGAL::abs(pnt.y()-pfirst.y()))   );
        FT seglenhalflast ( half *
                           CGAL::abs(
                                     CGAL::abs(pnt.x()-plast.x()) -
                                     CGAL::abs(pnt.y()-plast.y()))   );

        if (has_lseg_pos_slope) {
          //segment with positive slope
          if ( (compare_x_2(seg.source(),seg.target()) == SMALLER
                  and lseg.has_on_positive_side(pnt))
              or (compare_x_2(seg.source(),seg.target()) == LARGER
                  and lseg.has_on_negative_side(pnt)) ) {
                // pcfirst is center of square,
                // pfirst = phor, upward direction
                // pclast is center of sqaure, plast = pver, left direction
                pcfirst = Point_2(pmid_pfirst_pnt.x(),
                                  pmid_pfirst_pnt.y()+seglenhalffirst);
                pclast = Point_2(pmid_plast_pnt.x()-seglenhalflast,
                                 pmid_plast_pnt.y());
          }
          else {
            //pfirst = phor , pcfirst in downward direction
            //plast = pvor , pclast in right direction
            pcfirst = Point_2(pmid_pfirst_pnt.x(),
                              pmid_pfirst_pnt.y()-seglenhalffirst);
            pclast = Point_2(pmid_plast_pnt.x()+seglenhalflast,
                             pmid_plast_pnt.y());
          }
        }
        else {
          //segment with negative slope
          if ( (compare_x_2(seg.source(),seg.target()) == SMALLER
                and lseg.has_on_positive_side(pnt))
              or (compare_x_2(seg.source(),seg.target()) == LARGER
                  and lseg.has_on_negative_side(pnt)) ) {
                // pcfirst is center of square,
                // pfirst = pver, right direction
                // pclast is center of sqaure, plast = phor, upward dir
                pcfirst = Point_2(pmid_pfirst_pnt.x()+seglenhalffirst,
                                  pmid_pfirst_pnt.y());
                pclast = Point_2(pmid_plast_pnt.x(),
                                 pmid_plast_pnt.y()+seglenhalflast);
              }
          else {
            //pfirst = pver , pcfirst in left direction
            //plast = phor , pclast in downward direction
            pcfirst = Point_2(pmid_pfirst_pnt.x()-seglenhalffirst,
                              pmid_pfirst_pnt.y());
            pclast = Point_2(pmid_plast_pnt.x(),
                             pmid_plast_pnt.y()-seglenhalflast);
          }
        }//end of pcfirst and pclast

        //compute pmid and then pcmid = mid point of pmid and pnt
        Full_Line_2 lmid(pnt,pcfirst);
        Full_Line_2 lmidp = lmid.perpendicular(pnt);
        CGAL::Object pmidobject = intersection(lmidp, lseg);
        Point_2 pmid;
        if(CGAL::assign(pmid, pmidobject)){
          points[1] = midpoint(pmid, pnt);
        }
        npts = 3;
        points[0]=pcfirst;
        points[2]=pclast;

        // compute direction
        Direction_2 d (
            ( CGAL::sign(lseg.a()) == NEGATIVE ? +1 : -1 ) ,
            ( CGAL::sign(lseg.b()) == NEGATIVE ? +1 : -1 )  ) ;
        if (side == ON_POSITIVE_SIDE) {
          d = -d;
        }

        Polychainline pcl(d, points, points+npts, d);

        //CGAL_SDG_DEBUG(std::cout << "about to return pcl" << std::endl;);
        return pcl;
      }//end of general segment case, seg != hor or ver

    }


  }//end of bisector_PS


  result_type bisector_SS(const Site_2& p, const Site_2& q) const {
    CGAL_precondition( p.is_segment() and q.is_segment() );

    CGAL_SDG_DEBUG(std::cout << "bisector_SS entering with p=" << p
              << " q=" << q << std::endl;);

    // another precondition:
    // p and q have no intersection in the interior of any segment
    // (but maybe they intersect at their endpoints)

    // another precondition:
    // p, q may be parallel but do not have the same supporting line

    Are_parallel_2 are_parallel;

    // compute supporting lines of segments
    Full_Line_2 lp ( p.supporting_site().segment() );
    Full_Line_2 lq ( q.supporting_site().segment() );

    CGAL_SDG_DEBUG(std::cout << "bisector_SS lp = "
      << lp.a() << ' ' << lp.b() << ' ' << lp.c() << std::endl;);
    CGAL_SDG_DEBUG(std::cout << "bisector_SS lq = "
      << lq.a() << ' ' << lq.b() << ' ' << lq.c() << std::endl;);

    Point_2 points[1];
    Direction_2 dinc, dout;

    if (are_parallel(p, q)) {
      // here p and q are parallel,
      // but not on the same supporting line (precondition)

      // here lp.a lq.b - lq.a lp.b = 0

      // the bisector line bis of the two lines lp and lq
      RT bisa, bisb, bisc;

      if ( CGAL::sign ( lq.a() ) != ZERO ) {
        bisa = RT(2) * lp.a() * lq.a();
        bisb = RT(2) * lp.a() * lq.b();
        bisc = lp.a() * lq.c() + lp.c() * lq.a();
      } else {
        bisa = RT(2) * lp.a() * lq.b();
        bisb = RT(2) * lp.b() * lq.b();
        bisc = lp.c() * lq.b() + lp.b() * lq.c();
      }

      CGAL_SDG_DEBUG(std::cout << "bisector_SS parallel bis = "
        << bisa << ' ' << bisb << ' ' << bisc << std::endl;);

      Point_2 psrc = p.segment().source();

      Sign s = CGAL::sign(
          bisa * psrc.x() + bisb * psrc.y() + bisc);

      CGAL_assertion(s != ZERO);

      if (s == POSITIVE) {
        bisa = -bisa;
        bisb = -bisb;
        bisc = -bisc;
      }

      // convert bis to the polychainline format

      Direction_2 d ( bisb, -bisa);

      if (CGAL::sign(bisa) == ZERO) {
        // use point of bis line with x=0
        Point_2 mid (RT(0), -bisc, bisb);
        points[0] = mid;
      } else {
        // use point of bis line with y=0
        Point_2 mid (-bisc, RT(0), bisa);
        points[0] = mid;
      }

      dinc = -d;
      dout = d;

    } else {
      // here p and q are not parallel

      // compute intersection point of two lines

      Point_2 mid ( - lp.c() * lq.b() + lq.c() * lp.b(),
                    - lp.a() * lq.c() + lq.a() * lp.c(),
                    + lp.a() * lq.b() - lq.a() * lp.b() );

      points[0] = mid;

      CGAL_SDG_DEBUG(std::cout << "debug bisector_SS lp=" <<
          lp.a() << ' ' << lp.b() << ' ' << lp.c() << std::endl;);
      CGAL_SDG_DEBUG(std::cout << "debug bisector_SS lq=" <<
          lq.a() << ' ' << lq.b() << ' ' << lq.c() << std::endl;);
      CGAL_SDG_DEBUG(std::cout << "debug bisector_SS mid=" <<
          mid << std::endl;);

      // check if mid is inside one of the segments

      Point_2 psrc = p.segment().source();
      Point_2 ptrg = p.segment().target();
      Point_2 qsrc = q.segment().source();
      Point_2 qtrg = q.segment().target();

      if (
          (compare_x_2(psrc, mid) == compare_x_2(mid, ptrg)) and
          (compare_y_2(psrc, mid) == compare_y_2(mid, ptrg))    )
      {
        // mid is inside p
        CGAL_SDG_DEBUG(std::cout <<
            "debug bisector_SS mid in p" << std::endl;);

        // take any endpoint of q not the same as mid
        Point_2 qrep = ( mid == qsrc) ? qtrg : qsrc;

        //Vector_2 vecmq ( mid, qrep );
        //Direction_2 dirq ( vecmq );
        Direction_2 dirq ( qrep.x() - mid.x(),
                           qrep.y() - mid.y() );
        //Vector_2 vecmpsrc ( mid, psrc );
        //Direction_2 dirpsrc =  ( vecmpsrc );
        Direction_2 dirpsrc ( psrc.x() - mid.x(),
                              psrc.y() - mid.y() );
        //Vector_2 vecmptrg ( mid, ptrg );
        //Direction_2 dirptrg =  ( vecmptrg );
        Direction_2 dirptrg ( ptrg.x() - mid.x(),
                              ptrg.y() - mid.y() );

        CGAL_SDG_DEBUG(std::cout << "debug bisector_SS psrc=" << psrc
         << " ptrg=" <<  ptrg << std::endl;);

        if (dirq.counterclockwise_in_between(dirpsrc, dirptrg))
        {
          CGAL_SDG_DEBUG(std::cout <<
              "debug bisector_SS q btw psrc ptrg" << std::endl;);
          dout = compute_linf_bisecting_direction(dirpsrc, dirq);
          dinc = compute_linf_bisecting_direction(dirq, dirptrg);
        } else
        {
          CGAL_SDG_DEBUG(std::cout <<
              "debug bisector_SS q not btw psrc ptrg" << std::endl;);
          dout = compute_linf_bisecting_direction(dirptrg, dirq);
          dinc = compute_linf_bisecting_direction(dirq, dirpsrc);
        }

        CGAL_SDG_DEBUG(std::cout <<
            "debug bisector_SS dinc=" << dinc << std::endl;);
        CGAL_SDG_DEBUG(std::cout <<
            "debug bisector_SS dout=" << dout << std::endl;);

      } else if (
          (compare_x_2(qsrc, mid) == compare_x_2(mid, qtrg)) and
          (compare_y_2(qsrc, mid) == compare_y_2(mid, qtrg))    )
      {
        // mid is inside q
        CGAL_SDG_DEBUG(std::cout <<
            "debug bisector_SS mid in q" << std::endl;);

        // take any endpoint of p not the same as mid
        Point_2 prep = ( mid == psrc ) ? ptrg : psrc;

        //Vector_2 vecmp ( mid, prep );
        //Direction_2 dirp ( vecmp );
        Direction_2 dirp( prep.x() - mid.x(),
                          prep.y() - mid.y() );
        //Vector_2 vecmqsrc ( mid, qsrc );
        //Direction_2 dirqsrc =  ( vecmqsrc );
        Direction_2 dirqsrc ( qsrc.x() - mid.x(),
                              qsrc.y() - mid.y() );
        //Vector_2 vecmqtrg ( mid, qtrg );
        //Direction_2 dirqtrg =  ( vecmqtrg );
        Direction_2 dirqtrg ( qtrg.x() - mid.x(),
                              qtrg.y() - mid.y() );

        if (dirp.counterclockwise_in_between(dirqsrc, dirqtrg))
        {
          CGAL_SDG_DEBUG(std::cout <<
              "debug bisector_SS spt" << std::endl;);
          dinc = compute_linf_bisecting_direction(dirqsrc, dirp);
          dout = compute_linf_bisecting_direction(dirp, dirqtrg);
        } else
        {
          CGAL_SDG_DEBUG(std::cout <<
              "debug bisector_SS tps" << std::endl;);
          dinc = compute_linf_bisecting_direction(dirqtrg, dirp);
          dout = compute_linf_bisecting_direction(dirp, dirqsrc);
        }

        CGAL_SDG_DEBUG(std::cout <<
            "debug bisector_SS dinc=" << dinc << std::endl;);
        CGAL_SDG_DEBUG(std::cout <<
            "debug bisector_SS dout=" << dout << std::endl;);

      } else {
        // here mid is neither inside p nor inside q
        CGAL_SDG_DEBUG(std::cout <<
            "debug bisector_SS mid not in p, q" << std::endl;);

        // take any endpoint of p not the same as mid
        Point_2 prep = ( mid == psrc) ? ptrg : psrc;

        // take any endpoint of q not the same as mid
        Point_2 qrep = ( mid == qsrc) ? qtrg : qsrc;

        //Direction_2 dirp ( Vector_2(mid, prep) );
        Direction_2 dirp ( prep.x() - mid.x(),
                           prep.y() - mid.y() );
        //Direction_2 dirq ( Vector_2(mid, qrep) );
        Direction_2 dirq ( qrep.x() - mid.x(),
                           qrep.y() - mid.y() );

        Direction_2 d =
          dirq.counterclockwise_in_between(dirp, -dirp) ?
            compute_linf_bisecting_direction(dirp, dirq) :
            - compute_linf_bisecting_direction(dirq, dirp) ;

        dinc = -d;
        dout = d;
      }
    }

    //CGAL_SDG_DEBUG(std::cout << "debug bisector_SS points[0]="
    //  << points[0] << std::endl;);

    Polychainline pcl (dinc, points, points + 1, dout);

    return pcl;

  }


  Direction_2
  compute_linf_bisecting_direction(
        Direction_2 dirp, Direction_2 dirq) const
  {
    // precondition:
    // dirq is strictly after (counterclockwise) dirp
    // and dirq is less than 180 degrees after dirp

    CGAL_SDG_DEBUG(std::cout << "debug dir1 = " << dirp
              << " dir2 = " << dirq << std::endl;);

    RT two(2);
    RT plusone(+1);
    RT minusone(-1);

    RT A (dirp.dx());
    RT B (dirp.dy());
    RT C (dirq.dx());
    RT D (dirq.dy());

    CGAL_SDG_DEBUG(std::cout << "debug ABCD = " << A << ' ' << B << ' '
              << C << ' ' << D << std::endl;);

    if ((CGAL::sign(A) == POSITIVE) and
        (CGAL::sign(B) != NEGATIVE)    ) {
      // A > 0 and B >= 0

      if ((CGAL::sign(C) == POSITIVE) and
          (CGAL::sign(D) != NEGATIVE)    ) {
        // C > 0 and D >= 0
        return Direction_2 (two*A*C+A*D+B*C,
                            two*B*D+A*D+B*C);
      }
      else if ((CGAL::sign(C) != POSITIVE) and
          (CGAL::sign(D) == POSITIVE)    ) {
        // C <= 0 and D > 0
        return Direction_2 (A*D+B*C,
                            two*B*D+A*D-B*C);
      }
      else {
        // here C < 0 and D <= 0
        return Direction_2( minusone, plusone);
      }
    }
    else if ((CGAL::sign(A) != POSITIVE) and
        (CGAL::sign(B) == POSITIVE)    ) {
      // A <= 0 and B > 0
      if ((CGAL::sign(C) != POSITIVE) and
          (CGAL::sign(D) == POSITIVE)    ) {
        // C <= 0 and D > 0
        return Direction_2 (-two*A*C+A*D+B*C,
                             two*B*D-A*D-B*C);
      }
      else if ((CGAL::sign(C) == NEGATIVE) and
          (CGAL::sign(D) != POSITIVE)    ) {
        // C < 0 and D <= 0
        return Direction_2 (-two*A*C-A*D+B*C,
                            -A*D-B*C);
      }
      else {
        // here C >= 0 and D < 0
        return Direction_2( minusone, minusone);
      }
    }
    else if ((CGAL::sign(A) == NEGATIVE) and
        (CGAL::sign(B) != POSITIVE)    ) {
      // A < 0 and B <= 0
      if ((CGAL::sign(C) == NEGATIVE) and
          (CGAL::sign(D) != POSITIVE)    ) {
        // C < 0 and D <= 0
        return Direction_2 (-two*A*C-A*D-B*C,
                            -two*B*D-A*D-B*C);
      }
      else if ((CGAL::sign(C) != NEGATIVE) and
          (CGAL::sign(D) == NEGATIVE)    ) {
        // C >= 0 and D < 0
        return Direction_2 (-A*D-B*C,
                            -two*B*D-A*D+B*C);
      }
      else {
        // here C > 0 and D >= 0
        return Direction_2( plusone, minusone);
      }
    }
    else {
      // here A >= 0 and B < 0
      if ((CGAL::sign(C) != NEGATIVE) and
          (CGAL::sign(D) == NEGATIVE)    ) {
        // C >= 0 and D < 0
        return Direction_2 ( two*A*C-A*D-B*C,
                            -two*B*D+A*D+B*C);
      }
      else if ((CGAL::sign(C) == POSITIVE) and
          (CGAL::sign(D) != NEGATIVE)    ) {
        // C > 0 and D >= 0
        return Direction_2 ( two*A*C+A*D-B*C,
                             A*D+B*C);
      }
      else {
        // here C <= 0 and D > 0
        return Direction_2( plusone, plusone);
      }
    }

  }



public:
  result_type operator()(const Site_2& p, const Site_2& q) const
  {
    return bisector(p, q);
  }
};

} //namespace SegmentDelaunayGraphLinf_2

} //namespace CGAL


#endif // CGAL_SEGMENT_DELAUNAY_GRAPH_LINF_2_ARE_PARALLEL_C2_H
