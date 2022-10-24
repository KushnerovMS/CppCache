#include <iostream>
#include <cassert>
#include <list>
#include <vector>
#include "ARC.hpp"
#include "IdealCache.hpp"


int slow_get_page_int(int key) { return key; }

int main ()
{
    int hits = 0;
    int n;
    size_t m;

    std::cin >> m >> n;
    assert(std::cin.good());
    Cache::ARC_t<int> arc{m};

    std::vector<int> history_of_requests (n);

    for (int i = 0; i < n; ++i)
    {
        int q;
        std::cin >> q;
        assert(std::cin.good());
        if ( arc.lookup_update(q, slow_get_page_int) )
        {
            hits += 1;
        }

        std::cout << arc << std::endl;

        history_of_requests[i] = q;
    }

    std::cout << "Hits in ARC: " << hits << std::endl;

    hits = 0;

    Cache::Ideal_cache_t<int> ic (m, history_of_requests);

    for (int i = 0; i < n; i ++)
    {
        if ( ic.lookup_update(1, slow_get_page_int) )
            hits += 1;
    }

    std::cout << "Hits in Ideal cache: " << hits << std::endl;
    
    return 0;
}
