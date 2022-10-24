#pragma once

#include <iterator>
#include <list>
#include <unordered_map>
#include <vector>

namespace Cache {

// Last recently used cache
template <typename T, typename KeyT = int>
struct LRUC_t
{
    private:

        size_t sz_;

        struct Cache_elem_t_
        {
            KeyT key;
            T data;

            Cache_elem_t_ (const KeyT& k, const T& d) : key(k), data(d) {}
        };
        std::list<Cache_elem_t_> cache_;

        using ListIt = typename std::list<Cache_elem_t_>::iterator;
        std::unordered_map<KeyT, ListIt> hash_;

    public:

        LRUC_t (size_t sz) : sz_(sz) {}

        bool full() const { return (cache_.size() == sz_); }

        typedef T(*get_page_func_t)(KeyT);
        bool lookup_update(KeyT key, get_page_func_t slow_get_page)
        {
            auto hit = hash_.find(key);

            if (hit == hash_.end())
            {
                if (full())
                {
                    hash_.erase(cache_.back().key);
                    cache_.pop_back();
                }

                cache_.emplace_front( key, slow_get_page(key) );
                hash_[key] = cache_.begin();

                return false;
            }

                auto eltit = hit->second;

                if (eltit != cache_.begin())
                    cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));

                return true;
        }

        void print (std::ostream& stream) const
        {
            stream << "[ ";
            for (auto it = cache_.begin(); it != cache_.end(); ++ it)
                stream << it -> key << ' ';
            stream << "]";
        }
};
    template<class T, class KeyT>
    std::ostream& operator<< (std::ostream& stream, const LRUC_t<T, KeyT>& c) { c.print(stream); return stream; }

} // namespace Cache
