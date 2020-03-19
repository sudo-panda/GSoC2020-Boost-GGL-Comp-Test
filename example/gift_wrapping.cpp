#include "../include/gift_wrapping.hpp"
#include <boost/geometry.hpp>
#include <fstream>
#include <iostream>

namespace bg = boost::geometry;

int main()
{
    typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
    typedef bg::model::multi_point<point_t> mpoint_t;
    typedef bg::model::polygon<point_t> polygon_t;
    typedef bg::model::linestring<point_t> lstr_t;
    typedef bg::model::multi_linestring<lstr_t> mlstr_t;

    mpoint_t input;
    polygon_t output;

    bg::read_wkt("MULTIPOINT((8.86 3.83),(8.31 9.57),(7.19 3.72),(2.36 8.83),(4.91 4.37),"
    "(1.41 4.76),(3.8 8.67),(2.55 1.62),(6.06 2.45),(1.86 5.66))", input);

    gift_wrapping::wrap(input, output);

    std::cout<<"Output:\t"<<bg::wkt(output)<<std::endl;
    // Expected Output: "POLYGON((1.41 4.76,2.36 8.83,8.31 9.57,8.86 3.83,6.06 2.45,2.55 1.62,1.41 4.76))"

    mlstr_t axes;

    std::ofstream svg("graph.svg");
    boost::geometry::svg_mapper<point_t> mapper(svg, 400, 400);

    axes.resize(22);
    for(int i = 0; i <= 10; i++)
    {
        bg::append(axes[i*2], point_t(-1, i));
        bg::append(axes[i*2], point_t(11, i));
        bg::append(axes[i*2+1], point_t(i, -1));
        bg::append(axes[i*2+1], point_t(i, 11));
    }

    mapper.add(axes);
    mapper.add(input);
    mapper.add(output);

    mapper.map(axes, "stroke-opacity:1;stroke:rgb(0,0,0);stroke-width:0.1");
    mapper.map(output, "fill-opacity:0.5;fill:rgb(100,204,0);stroke:rgb(100,204,0);stroke-width:1", 2);
    mapper.map(input, "fill-opacity:0.3;fill:rgb(51,51,153);stroke:rgb(51,51,153);stroke-width:1", 1);
    
    for(int i = 0; i <= 10; i++)
    {
        mapper.text(point_t(-1,i), std::to_string(i), "fill-opacity:0.5;fill:rgb(0,0,0);", 0, -1, 0.1);
        mapper.text(point_t(i,-1), std::to_string(i), "fill-opacity:0.5;fill:rgb(0,0,0);", 1, 0, 0.1);
    }
}