#include <iostream>
#include <iomanip>
#include <random>
#include <vector>
#include <algorithm>

#include "ARC.hpp"
#include "IdealCache.hpp"
#include "LRUC.hpp"

const unsigned int CACHE_SIZE = 10;
const unsigned int NUMBER_RANGE = 100;
const unsigned int NUMBER_OF_REQUESTS = 10000;

struct efficiency_t
{
    unsigned int ARC;
    unsigned int LRUC;
};

int slow_get_page_int(int key) { return key; }

efficiency_t getEfficiency (unsigned int freq_range, unsigned int freq_prob)
{
    std::random_device random;
    std::uniform_int_distribution<int> distrib_perc(0, 100);
    std::uniform_int_distribution<int> distrib_freq(0, NUMBER_RANGE * freq_range / 100 - 1);
    std::uniform_int_distribution<int> distrib_infreq(NUMBER_RANGE * freq_range / 100, NUMBER_RANGE);

    // imposible consitions
    if ( (freq_range == 100 && freq_prob == 0) || (freq_range == 0 && freq_prob == 100) )
        return efficiency_t { 0, 0 };

    unsigned int p0 = (freq_range * freq_prob * 100) / ( (100 - freq_prob) * (100 - freq_range) + freq_range * freq_prob );

    std::vector<int> history_of_requests (NUMBER_OF_REQUESTS);

    for (unsigned int i = 0; i < NUMBER_OF_REQUESTS; i ++)
    {
        unsigned int p = distrib_perc(random);
        if (p <= p0)
            history_of_requests[i] = distrib_freq(random);
        else
            history_of_requests[i] = distrib_infreq(random);
    }

    Cache::ARC_t<int> arc (CACHE_SIZE);
    unsigned int ARC_hits = 0;

    Cache::LRUC_t<int> lruc (CACHE_SIZE);
    unsigned int LRUC_hits = 0;

    Cache::Ideal_cache_t<int> ic (CACHE_SIZE, history_of_requests);
    unsigned int IC_hits = 0;

    for (unsigned int i = 0; i < NUMBER_OF_REQUESTS; i ++)
    {
        if ( arc.lookup_update(history_of_requests[i], slow_get_page_int) )
            ARC_hits ++;

        if ( lruc.lookup_update(history_of_requests[i], slow_get_page_int) )
            LRUC_hits ++;

        if ( ic.lookup_update(history_of_requests[i], slow_get_page_int) )
            IC_hits ++;
    }

    if (IC_hits == 0)
        return efficiency_t { 100, 100 };
    else
        return efficiency_t { (ARC_hits * 100) / IC_hits, (LRUC_hits * 100) / IC_hits };
}

int main (int argc, char *argv[])
{
    unsigned int max_x = 100;
    unsigned int x_step = 5;

    unsigned int max_y = 100;
    unsigned int y_step = 5;

    unsigned int table_w = max_x / x_step + 1;
    unsigned int table_h = (max_y - 50) / y_step + 1;

    efficiency_t table [table_w] [table_h];

    std::cout << "It is believed that the cache efficiency on a given set of requests is number_of_hits / number_of_hits_in_ideal_cache" << std::endl;
    std::cout << "Note: all values will be shown in percents" << std::endl << std::endl;

    std::cout << "Making simulation" << std::endl;

    for (unsigned int xi = 0; xi < table_w; ++ xi)
    {
        for (unsigned int yi = 0; yi < table_h; ++ yi)
        {
            table [xi] [yi] = getEfficiency (xi * x_step, yi * y_step + 50);

            std::cout << "\rProgress:" << ( (xi * table_h + yi + 1) * 100 ) / (table_w * table_h) << '%';
            std::cout.flush();
        }
    }
    std::cout << std::endl << std::endl;

    std::cout << "x -- part of requested values that are frequent" << std::endl;
    std::cout << "y -- chance of a frequent request" << std::endl;

    std::cout << std::endl << std::endl;

    std::cout << "    Efficiency of ARC" << std::endl << std::endl;

    std::cout.fill(' ');
    std::cout << "y\\x|";
    for (unsigned int xi = 0; xi < table_w; ++ xi)
    {
        std::cout.flush();
        std::cout << std::setw(3) << xi * x_step << '|';
    }
    std::cout << std::endl;
    std::cout.fill('-');
    std::cout << std::setw(table_w * 4) << '-' << std::endl;
    std::cout.fill(' ');
    for (unsigned int yi = 0; yi < table_h; ++ yi)
    {
        std::cout << std::setw(3) << yi * y_step + 50 << '|';
        for (unsigned int xi = 0; xi < table_w; ++ xi)
            std::cout << std::setw(3) << table [xi] [yi] .ARC << '|';
        std::cout << std::endl;
    }


    std::cout << std::endl << std::endl;

    std::cout << "    (Efficiency of ARC) - (efficiency of LRUC)" << std::endl << std::endl;

    std::cout.fill(' ');
    std::cout << "y\\x|";
    for (unsigned int xi = 0; xi < table_w; ++ xi)
    {
        std::cout.flush();
        std::cout << std::setw(3) << xi * x_step << '|';
    }
    std::cout << std::endl;
    std::cout.fill('-');
    std::cout << std::setw( (table_w + 1) * 4) << '-' << std::endl;
    std::cout.fill(' ');
    for (unsigned int yi = 0; yi < table_h; ++ yi)
    {
        std::cout << std::setw(3) << yi * y_step + 50 << '|';
        for (unsigned int xi = 0; xi < table_w; ++ xi)
            std::cout << std::setw(3) << int(table [xi] [yi] .ARC) - int(table [xi] [yi] .LRUC) << '|';
        std::cout << std::endl;
    }

    return 0;
}
