#include "../include/gift_wrapping.hpp"
#include <boost/geometry.hpp>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h> 
#include <chrono> 

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

    int no_of_points = 100;
    int max_range = 100;

    for(int i = 0; i < no_of_points; i++)
    {
        srand (i*time(NULL));
        bg::append(input, point_t(rand() % (100 * max_range) / 100.0, rand() % (100 * max_range) / 100.0));
    }

    std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    gift_wrapping::wrap(input, output);
    std::chrono::_V2::system_clock::time_point stop = std::chrono::high_resolution_clock::now();

    std::cout<<bg::wkt(input)<<std::endl;
    std::cout<<bg::wkt(output)<<std::endl;
    std::cout<<"Time Taken: "<<std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()<<" milliseconds"<<std::endl;

    mlstr_t axes;

    std::ofstream svg("graph.svg");
    boost::geometry::svg_mapper<point_t> mapper(svg, 40 * max_range, 40 * max_range);

    axes.resize(( max_range + 1 ) * 2);
    for(int i = 0; i <= max_range; i++)
    {
        bg::append(axes[i*2], point_t(-1, i));
        bg::append(axes[i*2], point_t(max_range + 1, i));
        bg::append(axes[i*2+1], point_t(i, -1));
        bg::append(axes[i*2+1], point_t(i, max_range + 1));
    }

    mapper.add(axes);
    mapper.add(input);
    mapper.add(output);

    mapper.map(axes, "stroke-opacity:1;stroke:rgb(0,0,0);stroke-width:0.1");
    mapper.map(output, "fill-opacity:0.5;fill:rgb(100,204,0);stroke:rgb(100,204,0);stroke-width:"+std::to_string(max_range / 10), 2);
    mapper.map(input, "fill-opacity:0.3;fill:rgb(51,51,153);stroke:rgb(51,51,153);stroke-width:"+std::to_string(max_range / 10), max_range / 10);
    
    for(int i = 0; i <= max_range; i++)
    {
        mapper.text(point_t(-1,i), std::to_string(i), "fill-opacity:0.5;fill:rgb(0,0,0);", 0, -1, 0.1);
        mapper.text(point_t(i,-1), std::to_string(i), "fill-opacity:0.5;fill:rgb(0,0,0);", 1, 0, 0.1);
    }
}