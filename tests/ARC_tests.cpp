#include <catch2/catch_test_macros.hpp>

#include <random>

#include "../src/ARC.hpp"

const int CACHE_SIZE = 10;
const int NUMBER_RANGE = 100;
const int NUMBER_OF_REQUESTS = 100000;

int slow_get_page_int(int key) { return key; }

TEST_CASE( "ARC is testing", "[ARC]" )
{
    Cache::ARC_t<int> arc (CACHE_SIZE);
    int hits = 0;

    SECTION( "Making same requests" )
    {
        for (int i = 0; i < NUMBER_OF_REQUESTS; i ++)
        {
            if (arc.lookup_update(1, slow_get_page_int) )
                hits ++;
        }

        REQUIRE( hits == NUMBER_OF_REQUESTS - 1 );
    }
    SECTION( "Making complitely different requests" )
    {
        for (int i = 0; i < NUMBER_OF_REQUESTS; i ++)
        {
            if (arc.lookup_update(i, slow_get_page_int) )
                hits ++;
        }

        REQUIRE( hits == 0 );
    }
    SECTION( "Making random requests" )
    {
        std::random_device random;
        std::uniform_int_distribution<int> distrib(0, NUMBER_RANGE);

        for (int i = 0; i < NUMBER_OF_REQUESTS; i ++)
        {
            if (arc.lookup_update( distrib(random), slow_get_page_int) )
                hits ++;
        }

        REQUIRE( hits < NUMBER_OF_REQUESTS );
    }
}
