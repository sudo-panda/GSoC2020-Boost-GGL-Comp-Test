#ifndef GIFT_WRAPPING_HPP
#define GIFT_WRAPPING_HPP

#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/mpl/assert.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <boost/geometry/algorithms/detail/for_each_range.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/equals.hpp>
#include <boost/geometry/algorithms/not_implemented.hpp>
#include <boost/geometry/arithmetic/cross_product.hpp>
#include <boost/geometry/core/assert.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_point.hpp>
#include <boost/geometry/policies/compare.hpp>
#include <boost/geometry/strategies/convex_hull.hpp>
#include <boost/geometry/views/detail/range_type.hpp>


namespace gift_wrapping
{

namespace detail {

template 
< 
    typename InputRange,
    typename Strategy
>
struct get_extreme
{
    typedef typename boost::geometry::point_type<InputRange>::type point_t;
    typedef typename boost::range_iterator
        <
            InputRange const
        >::type range_it_t;
    
    point_t out;

    Strategy strategy;

    inline void apply(InputRange const& range)
    {
        if (boost::size(range) == 0)
        {
            return;
        }

        range_it_t out_it = boost::begin(range);

        for (range_it_t it = boost::begin(range) + 1;
            it != boost::end(range);
            ++it)
        {
            if (strategy(*it, *out_it))
            {
                out_it = it;
            }
        }
        out = *out_it;
    }
};

struct on_left
{
    template < typename Point >
    static inline bool apply(Point check_pt, Point base_pt, Point ref_pt)
    {
        boost::geometry::subtract_point(check_pt, base_pt);
        boost::geometry::subtract_point(ref_pt, base_pt);
        return boost::geometry::get<0>(boost::geometry::cross_product(check_pt, ref_pt)) < 0;
    }
};

struct is_collinear
{
    template < typename Point >
    static inline bool apply(Point a, Point b, Point c)
    {
        boost::geometry::subtract_point(c, b);
        boost::geometry::subtract_point(a, b);
        return boost::geometry::get<0>(boost::geometry::cross_product(a, c)) == 0;
    }
};

struct is_farther
{
    template < typename Point>
    static inline bool apply(Point base_pt, Point ref_pt, Point check_pt)
    {
        return boost::geometry::comparable_distance(base_pt, ref_pt) < 
                boost::geometry::comparable_distance(base_pt, check_pt);
    }
};

template < typename InputRange >
struct get_next_on_hull
{
    typedef typename boost::geometry::point_type<InputRange>::type point_t;
    typedef typename boost::range_iterator
        <
            InputRange const
        >::type range_it_t;
    
    point_t prev;

    inline get_next_on_hull(point_t first_pt)
        : prev(first_pt)
    {}

    inline void apply(InputRange const& range)
    {
        if (boost::size(range) == 0)
        {
            return;
        }

        range_it_t curr_it = boost::begin(range);

        for (range_it_t it = boost::begin(range) + 1;
            it != boost::end(range);
            ++it)
        {
            if( boost::geometry::equals(*curr_it, prev) || on_left::apply(*it, prev, *curr_it))
                curr_it = it;
            else if (is_collinear::apply(*it, prev, *curr_it) && is_farther::apply(prev, *curr_it, *it))
                curr_it = it;
        }
       prev = *curr_it;
    }
};


struct wrap
{
    template < typename InGeometry, typename OutGeometry >
    static inline void apply(InGeometry const& input, OutGeometry& output)
    {
        typedef typename boost::geometry::point_type<InGeometry>::type point_t;
        typedef typename boost::geometry::detail::range_type< InGeometry >::type range_type;
        typedef typename boost::range_iterator
        <
            range_type const
        >::type range_it_t;
        
        point_t pt_on_hull, end_pt;

        detail::get_extreme
            <
                range_type,
                boost::geometry::less<point_t>
            > first_on_hull;
        
        boost::geometry::detail::for_each_range(input, first_on_hull);

        detail::get_next_on_hull
            <
                range_type
            > next_on_hull(first_on_hull.out);
            
        do 
        {
            boost::geometry::append(output, next_on_hull.prev);
            boost::geometry::detail::for_each_range(input, next_on_hull);
        } while (! boost::geometry::equals(next_on_hull.prev, first_on_hull.out));
    }

    template < typename Point >
    static inline void apply(boost::geometry::model::multi_point<Point> const& input, boost::geometry::model::polygon<Point>& output)
    {
        apply(input, output.outer());
    }
};

}

namespace dispatch {

template 
< 
    typename InGeometry,
    typename OutGeometry, 
    typename InTag=typename boost::geometry::tag<InGeometry>::type, 
    typename OutTag=typename boost::geometry::tag<OutGeometry>::type 
>
struct wrap
    : boost::geometry::not_implemented<InTag, OutTag>
{};

template < typename Point >
struct wrap 
<
    boost::geometry::model::multi_point<Point>,
    boost::geometry::model::polygon<Point>, 
    boost::geometry::multi_point_tag,
    boost::geometry::polygon_tag
>
    : detail::wrap
{};

template < typename Point >
struct wrap 
<
    boost::geometry::model::multi_point<Point>,
    boost::geometry::model::multi_point<Point>, 
    boost::geometry::multi_point_tag,
    boost::geometry::multi_point_tag
>
    : detail::wrap
{};

}


template < typename InGeometry, typename OutGeometry >
inline void wrap(InGeometry &in_geom, OutGeometry &out_geom)
{
    dispatch::wrap<InGeometry, OutGeometry>::apply(in_geom, out_geom);
}


}

#endif