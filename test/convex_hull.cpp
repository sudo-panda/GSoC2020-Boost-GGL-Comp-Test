#define BOOST_TEST_MODULE example
#include "../include/gift_wrapping.hpp"
#include <boost/geometry/io/io.hpp>
#include <boost/geometry/geometries/multi_point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/test/included/unit_test.hpp>

namespace bg = boost::geometry;
namespace bu = boost::unit_test;

template < typename InGeometry , typename OutGeometry >
std::string gift_wrapping_wkt(std::string const& in_wkt)
{

    InGeometry in_geometry;
    bg::read_wkt(in_wkt, in_geometry);
    OutGeometry out_geometry;
    gift_wrapping::wrap(in_geometry,out_geometry);
    std::ostringstream out_wkt;
    out_wkt << bg::wkt(out_geometry);

    return out_wkt.str();
}

BOOST_AUTO_TEST_CASE( test_wrap )
{
    typedef bg::model::d2::point_xy<double> point_t;
    typedef bg::model::multi_point<point_t> mpoint_t;
    typedef bg::model::polygon<point_t> polygon_t;
    std::string out;
    out = gift_wrapping_wkt< mpoint_t, polygon_t >("MULTIPOINT((0 0),(0 1),(0 2),(0 3),(4 4),(5 5),(6 6))");
    BOOST_CHECK_EQUAL(out, "POLYGON((0 0,0 3,6 6,0 0))");
    out = gift_wrapping_wkt< mpoint_t, polygon_t >("MULTIPOINT((8.86 3.83),(3.18 0.59),(5.87 6.25),(5.9 7.44),(3.45 1.4),(1.97 3),(3.21 1.74),(3.34 0.25),(2.98 9.68),(2.23 4.91))");
    BOOST_CHECK_EQUAL(out, "POLYGON((1.97 3,2.23 4.91,2.98 9.68,5.9 7.44,8.86 3.83,3.34 0.25,1.97 3))");
}

BOOST_AUTO_TEST_CASE( test_wrap2 )
{
    typedef bg::model::d2::point_xy<double> point_t;
    typedef bg::model::multi_point<point_t> mpoint_t;
    typedef bg::model::polygon<point_t> polygon_t;
    std::string out;
    out = gift_wrapping_wkt< mpoint_t, polygon_t >("MULTIPOINT((0 0),(0 1),(0 2),(0 3),(4 4),(5 5),(6 6))");
    BOOST_CHECK_EQUAL(out, "POLYGON((0 0,0 3,0 0))");
    out = gift_wrapping_wkt< mpoint_t, polygon_t >("MULTIPOINT((8.86 3.83),(3.18 0.59),(5.87 6.25),(5.9 7.44),(3.45 1.4),(1.97 3),(3.21 1.74),(3.34 0.25),(2.98 9.68),(2.23 4.91))");
    BOOST_CHECK_EQUAL(out, "POLYGON((1.97 3,2.23 4.91,2.98 9.68,5.9 7.44,8.86 3.83,3.34 0.25,1.97 3))");
}
