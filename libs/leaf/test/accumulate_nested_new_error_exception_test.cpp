// Copyright 2018-2022 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#ifdef BOOST_LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#else

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/on_error.hpp>
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/exception.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int A>
struct info
{
    int value;
};

void f0()
{
    auto load = leaf::on_error( info<0>{-1} );
    throw leaf::exception(info<1>{-1});
}

void f1()
{
    auto load = leaf::on_error( []( info<0> & x ) { }, []( info<1> & x ) { ++x.value; }, []( info<2> & x ) { ++x.value; } );
    try { f0(); } catch(...) { }
    throw leaf::exception();
}

leaf::error_id f2()
{
    try
    {
        f1();
        BOOST_TEST(false);
    }
    catch( leaf::error_id const & err )
    {
        err.load( info<3>{3} );
        throw;
    }
    catch(...)
    {
        BOOST_TEST(false);
    }
    return leaf::new_error();
}

int main()
{
    int r = leaf::try_catch(
        []
        {
            f2();
            return 0;
        },
        []( info<0> i0, info<1> i1, info<2> i2, info<3> i3 )
        {
            BOOST_TEST_EQ(i0.value, 0);
            BOOST_TEST_EQ(i1.value, 1);
            BOOST_TEST_EQ(i2.value, 1);
            BOOST_TEST_EQ(i3.value, 3);
            return 1;
        },
        []
        {
            return 2;
        } );
    BOOST_TEST_EQ(r, 1);

    return boost::report_errors();
}

#endif
