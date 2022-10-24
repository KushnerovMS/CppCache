#pragma once

#include <unordered_map>
#include <list>
#include <vector>
#include <cassert>
#include <iostream>

namespace Cache {

template <class T, class KeyT = int>
struct Ideal_cache_t
{
    private:
        size_t size_;

        struct Cache_elem_t_
        {
            size_t pos_of_next;
            KeyT key;
            T data;

            Cache_elem_t_ (const size_t& p, const KeyT& k, const T& d) : pos_of_next(p), key(k), data(d) {}
        };
        std::list<Cache_elem_t_> cache_;

        using ListIt = typename std::list<Cache_elem_t_>::iterator;
        std::unordered_map<KeyT, ListIt> hash_;

        struct Request_t
        {
            size_t pos_of_next;
            KeyT key;

            Request_t (const size_t& p, const KeyT& k) : pos_of_next(p), key(k) {}
        };
        std::list<Request_t> line_of_requests_;

    public:

        Ideal_cache_t (size_t size, std::vector<KeyT> line_of_requests) : size_(size), cache_ (), line_of_requests_ (), hash_ ()
        {
            size_t i = 0;

            using Line_of_requests_it = typename std::list<Request_t>::iterator;
            std::unordered_map<KeyT, Line_of_requests_it> temp_table;

            for (const auto& item : line_of_requests)
            {
                auto hit = temp_table.find(item);
                if ( hit != temp_table.end() )
                    hit -> second -> pos_of_next = i;
                
                line_of_requests_.emplace_back( static_cast<size_t>(-1), item );
                temp_table [item] = std::prev( line_of_requests_.end() );

                i ++;
            }
        }

        bool full() const { return cache_.size() >= size_; }

        typedef T(*get_page_func_t)(KeyT);
        bool lookup_update (KeyT, get_page_func_t slow_get_page)
        {
            assert ( line_of_requests_.empty() == 0 );
            Request_t req = line_of_requests_.front();
            line_of_requests_.pop_front();

            auto hit = hash_.find( req.key );

            if ( hit != hash_.end() )
            {
                hit -> second -> pos_of_next = req.pos_of_next;

                return true;
            }
            else
            {
                if (full ())
                {
                    hash_.erase( cache_.front().key );
                    cache_.pop_front();
                }

                for (auto it = cache_.begin(); ; it ++)
                {
                    if (it == cache_.end() || it -> pos_of_next < req.pos_of_next )
                    {
                        cache_.emplace( it, req.pos_of_next,
                                           req.key,
                                           slow_get_page(req.key) );

                        hash_[req.key] = std::prev(it);

                        break;
                    }
                }
                
                return false;
            }
            
        }

        void print (std::ostream& stream) const
        {
            stream << "[ ";
            for (auto it = cache_.begin(); it != cache_.end(); ++ it)
                if (it -> pos_of_next != size_t(-1))
                    stream << "{" << it -> key << ", " << it -> pos_of_next << "} ";
                else
                    stream << "{" << it -> key << ", " << "never"           << "} ";
            stream << "]";
        }
};

    template<class T, class KeyT>
    std::ostream& operator<< (std::ostream& stream, const Ideal_cache_t<T, KeyT>& c) { c.print(stream); return stream; }

}// namespace Cache
