#pragma once

#include <istream>
#include <iterator>
#include <list>
#include <unordered_map>

namespace Cache {

//Adaptive Replacement Cache
template <class T, class KeyT = int>
struct ARC_t
{
    private:
        size_t size_;
        std::list<T> cache_;

        enum List_num_t_
        {
            T1_NUM = 0,
            T2_NUM = 1,
            B1_NUM = 2,
            B2_NUM = 3
        };
        using DataIt = typename std::list<T>::iterator;
        struct List_elem_t_
        {
            List_num_t_ list_num;
            KeyT key;
            DataIt dataIt;
        };

        using List_t = typename std::list<List_elem_t_>;

        List_t T1_;
        List_t B1_;
        List_t T2_;
        List_t B2_;

        #define _GET_LIST_BY_NUM_(num)\
            (num < B1_NUM) ?          \
                (                     \
                    (num == T1_NUM) ? \
                        T1_           \
                        :             \
                        T2_           \
                )                     \
                :                     \
                (                     \
                    (num == B1_NUM) ? \
                        B1_           \
                    :                 \
                        B2_           \
                )

        size_t marker_ = 0;

        using ListIt = typename std::list<List_elem_t_>::iterator;
        std::unordered_map<KeyT, ListIt> hash_;

        void replace (bool hit_in_B2 = false)
        {
            if ( T1_.size() >= 1 && ( (hit_in_B2 && T1_.size() == marker_) || (T1_.size() > marker_) ) )
            {
                List_elem_t_ &back_elem = T1_.back();       //getting T1 LRU elem

                cache_.erase(back_elem.dataIt);             //remuving it from cache
                back_elem.dataIt = cache_.end();
                
                back_elem.list_num = B1_NUM;            
                B1_.splice(B1_.begin(), T1_, -- T1_.end()); //inserting to beginning of B1
            }
            else
            {
                List_elem_t_ &back_elem = T2_.back();       //getting T2 LRU elem

                cache_.erase(back_elem.dataIt);             //remuving it from cache
                back_elem.dataIt = cache_.end();
                
                back_elem.list_num = B2_NUM;            
                B2_.splice(B2_.begin(), T2_, -- T2_.end()); //inserting to beginning of B2
            }
        }

    public:

        ARC_t(size_t sz) : size_(sz), cache_(), T1_(), B1_(), T2_(), B2_(), hash_() {}
            
        bool full() const { return cache_.size() >= size_; };

        typedef T(*get_page_func_t)(KeyT);
        bool lookup_update (KeyT key, get_page_func_t slow_get_page)
        {
            auto hit = hash_.find(key);

            if (hit != hash_.end())             // Exists
            {
                auto list_it = hit -> second;

                List_num_t_ &list_num = list_it -> list_num;
                if (list_num == B1_NUM)
                {
                    marker_ = std::min (size_, marker_ + std::max<size_t> (B2_.size() / B1_.size(), 1));
                    replace();
                }
                else if (list_num == B2_NUM)
                {
                    size_t tmp = std::max<size_t> (B1_.size() / B2_.size(), 1);
                    marker_ = (marker_ >= tmp)? marker_ - tmp : 0;
                    replace(true);
                }

                T2_.splice (T2_.begin(), _GET_LIST_BY_NUM_(list_num), list_it);
                list_num = T2_NUM;

                if (list_it -> dataIt == cache_.end())
                {
                    cache_.push_front (slow_get_page (key));
                    list_it -> dataIt = cache_.begin();

                    return false;
                }

                return true;
            }
            else                                // No exists
            {
                if (T1_.size() + B1_.size() == size_)
                {
                    if (T1_.size() < size_)
                    {
                        hash_.erase(B1_.back().key);
                        B1_.pop_back();
                        replace();
                    }
                    else
                    {
                        hash_.erase(T1_.back().key);
                        cache_.erase(T1_.back().dataIt);
                        T1_.pop_back();
                    }
                }
                else if (T1_.size() + T2_.size() + B1_.size() + B2_.size() >= size_)
                {
                    if (T1_.size() + T2_.size() + B1_.size() + B2_.size() == 2 * size_)
                    {
                        hash_.erase(B2_.back().key);
                        B2_.pop_back();
                    }
                    replace();
                }

                cache_.push_front (slow_get_page(key));
                T1_.push_front (List_elem_t_ ({T1_NUM, key, cache_.begin()}));
                hash_[key] = T1_.begin();
                
                return false;
            }
        }

        friend std::ostream& operator<< (std::ostream& stream, const ARC_t<T, KeyT>& c)
        {
            stream << "[ ";
            for (auto it = c.B1_.rbegin(); it != c.B1_.rend(); it ++)
                stream << it -> key << ' ';
            stream << "[ ";
            for (auto it = c.T1_.rbegin(); it != c.T1_.rend(); it ++)
                stream << it -> key << ' ';
            stream << "! ";
            for (auto it = c.T2_.begin(); it != c.T2_.end(); it ++)
                stream << it -> key << ' ';
            stream << "] ";
            for (auto it = c.B2_.begin(); it != c.B2_.end(); it ++)
                stream << it -> key << ' ';
            stream << "] " << c.marker_;
            return stream;
        }
};

} // namespace Cache
