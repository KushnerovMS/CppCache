#include <catch2/catch_test_macros.hpp>

#include <random>
#include <vector>
#include <algorithm>

#include "../src/IdealCache.hpp"

const int CACHE_SIZE = 10;
const int NUMBER_RANGE = 100;
const int NUMBER_OF_REQUESTS = 100000;

int slow_get_page_int(int key) { return key; }

TEST_CASE( "Ideal Cache is testing", "[IdealCache]" )
{
    int hits = 0;

    SECTION( "Making same requests" )
    {
        std::vector<int> history_of_requests (NUMBER_OF_REQUESTS, 1);
            
        Cache::Ideal_cache_t<int> ic (CACHE_SIZE, history_of_requests);

        for (int i = 0; i < NUMBER_OF_REQUESTS; i ++)
        {
            if (ic.lookup_update(1, slow_get_page_int) )
                hits ++ ;
        }

        REQUIRE( hits == NUMBER_OF_REQUESTS - 1 );
    }
    SECTION( "Making complitely different requests" )
    {
        std::vector<int> history_of_requests (NUMBER_OF_REQUESTS);

        std::generate (history_of_requests.begin(), history_of_requests.end(),
                [i=0] () mutable { return i ++ ; } );

        Cache::Ideal_cache_t<int> ic (CACHE_SIZE, history_of_requests);

        for (int i = 0; i < NUMBER_OF_REQUESTS; i ++)
        {
            if (ic.lookup_update(i, slow_get_page_int) )
                hits ++;
        }

        REQUIRE( hits == 0 );
    }
    SECTION( "Making random requests" )
    {
        std::random_device random;
        std::uniform_int_distribution<int> distrib(0, NUMBER_RANGE);

        std::vector<int> history_of_requests (NUMBER_OF_REQUESTS);

        std::generate (history_of_requests.begin(), history_of_requests.end(),
                [&random, &distrib] () { return distrib(random); } );

        Cache::Ideal_cache_t<int> ic (CACHE_SIZE, history_of_requests);


        for (int i = 0; i < NUMBER_OF_REQUESTS; i ++)
        {
            if (ic.lookup_update( 1, slow_get_page_int) )
                hits ++;
        }

        REQUIRE( hits < NUMBER_OF_REQUESTS );
    }
}
