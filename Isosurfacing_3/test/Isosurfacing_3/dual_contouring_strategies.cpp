#include <CGAL/Simple_cartesian.h>

#include <CGAL/Isosurfacing_3/Cartesian_grid_3.h>
#include <CGAL/Isosurfacing_3/dual_contouring_3.h>
#include <CGAL/Isosurfacing_3/Dual_contouring_domain_3.h>
#include <CGAL/Isosurfacing_3/Value_function_3.h>
#include <CGAL/Isosurfacing_3/Finite_difference_gradient_3.h>
#include <CGAL/Isosurfacing_3/internal/implicit_shapes_helper.h>

#include <CGAL/Real_timer.h>

#include <CGAL/IO/polygon_soup_io.h>

#include <vector>

using Kernel = CGAL::Simple_cartesian<double>;
using FT = typename Kernel::FT;
using Point = typename Kernel::Point_3;
using Vector = typename Kernel::Vector_3;

using Grid = CGAL::Isosurfacing::Cartesian_grid_3<Kernel>;
using Values = CGAL::Isosurfacing::Value_function_3<Grid>;
using Gradients = CGAL::Isosurfacing::Finite_difference_gradient_3<Kernel>;
using Domain = CGAL::Isosurfacing::Dual_contouring_domain_3<Grid, Values, Gradients>;

using Point_range = std::vector<Point>;
using Polygon_range = std::vector<std::vector<std::size_t> >;

namespace IS = CGAL::Isosurfacing;

auto implicit_function = [](const Point& q) -> FT
{
  auto cyl = [](const Point& q) { return IS::Shapes::infinite_cylinder<Kernel>(Point(0,0,0), Vector(0,0,1), 0.5, q); };
  auto cube = [](const Point& q) { return IS::Shapes::box<Kernel>(Point(-0.5,-0.5,-0.5), Point(0.5,0.5,0.5), q); };
  auto cyl_and_cube = [&](const Point& q) { return IS::Shapes::shape_union<Kernel>(cyl, cube, q); };

  auto sphere = [](const Point& q) { return IS::Shapes::sphere<Kernel>(Point(0,0,0.5), 0.75, q); };
  return IS::Shapes::shape_difference<Kernel>(cyl_and_cube, sphere, q);
};

int main(int argc, char** argv)
{
  const FT isovalue = (argc > 1) ? std::stod(argv[1]) : 0.;
  std::cout << "Isovalue: " << isovalue << std::endl;

  // create bounding box and grid
  const CGAL::Bbox_3 bbox = {-2., -2., -2., 2., 2., 2.};
  Grid grid { bbox, CGAL::make_array<std::size_t>(50, 50, 50) };

  std::cout << "Span: " << grid.span() << std::endl;
  std::cout << "Cell dimensions: " << grid.spacing()[0] << " " << grid.spacing()[1] << " " << grid.spacing()[2] << std::endl;
  std::cout << "Cell #: " << grid.xdim() << ", " << grid.ydim() << ", " << grid.zdim() << std::endl;

  const FT step = CGAL::approximate_sqrt(grid.spacing().squared_length()) * 0.01;

  // fill up values and gradients
  Values values { implicit_function, grid };
  Gradients gradients { values, step };
  Domain domain { grid, values, gradients };

  const bool triangulate_faces = false;

  // unconstrained QEM Placement strategy (default)
  {
    CGAL::Real_timer timer;
    timer.start();

    Point_range points;
    Polygon_range triangles;

    std::cout << "--- Dual Contouring (QEM - unconstrained)" << std::endl;
    IS::internal::Dual_contourer<CGAL::Parallel_if_available_tag, Domain,
                                 IS::internal::DC_Strategy::QEM> contourer;
    contourer(domain, isovalue, points, triangles,
              CGAL::parameters::do_not_triangulate_faces(!triangulate_faces));

    timer.stop();

    std::cout << "Output #vertices: " << points.size() << std::endl;
    std::cout << "Output #triangles: " << triangles.size() << std::endl;
    std::cout << "Elapsed time: " << timer.time() << " seconds" << std::endl;
    CGAL::IO::write_polygon_soup("dual_contouring_QEM-unconstrained.off", points, triangles);
  }

  // constrained QEM Placement strategy
  {
    CGAL::Real_timer timer;
    timer.start();

    Point_range points;
    Polygon_range triangles;

    std::cout << "--- Dual Contouring (QEM - constrained)" << std::endl;
    IS::internal::Dual_contourer<CGAL::Parallel_if_available_tag, Domain,
                                 IS::internal::DC_Strategy::QEM> contourer;
    contourer(domain, isovalue, points, triangles,
              CGAL::parameters::do_not_triangulate_faces(!triangulate_faces)
                               .constrain_to_cell(true));

    timer.stop();

    std::cout << "Output #vertices: " << points.size() << std::endl;
    std::cout << "Output #triangles: " << triangles.size() << std::endl;
    std::cout << "Elapsed time: " << timer.time() << " seconds" << std::endl;
    CGAL::IO::write_polygon_soup("dual_contouring_QEM-constrained.off", points, triangles);
  }


  // Centroid of Edge Intersections strategy
  {
    CGAL::Real_timer timer;
    timer.start();

    Point_range points;
    Polygon_range triangles;

    std::cout << "--- Dual Contouring (Centroid of Edge Intersections)" << std::endl;
    IS::internal::Dual_contourer<CGAL::Sequential_tag, Domain,
                                 IS::internal::DC_Strategy::Centroid_of_edge_intersections> contourer;
    contourer(domain, isovalue, points, triangles,
              CGAL::parameters::do_not_triangulate_faces(!triangulate_faces));

    timer.stop();

    std::cout << "Output #vertices: " << points.size() << std::endl;
    std::cout << "Output #triangles: " << triangles.size() << std::endl;
    std::cout << "Elapsed time: " << timer.time() << " seconds" << std::endl;
    CGAL::IO::write_polygon_soup("dual_contouring_CEI.off", points, triangles);
  }

  // Cell Center strategy
  {
    CGAL::Real_timer timer;
    timer.start();

    Point_range points;
    Polygon_range triangles;

    std::cout << "--- Dual Contouring (Cell Center)" << std::endl;
    IS::internal::Dual_contourer<CGAL::Sequential_tag, Domain,
                                 IS::internal::DC_Strategy::Cell_center> contourer;
    contourer(domain, isovalue, points, triangles,
              CGAL::parameters::do_not_triangulate_faces(!triangulate_faces));

    timer.stop();

    std::cout << "Output #vertices: " << points.size() << std::endl;
    std::cout << "Output #triangles: " << triangles.size() << std::endl;
    std::cout << "Elapsed time: " << timer.time() << " seconds" << std::endl;
    CGAL::IO::write_polygon_soup("dual_contouring_CC.off", points, triangles);
  }

  std::cout << "Done" << std::endl;

  return EXIT_SUCCESS;
}
