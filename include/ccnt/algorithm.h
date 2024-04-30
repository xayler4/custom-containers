#pragma once

#include <type_traits>
#include <cstdint>
#include <utility> 

namespace ccnt {
    class RandomAccessIterator {

    };

    class SparseIterator {

    };

    class ListIterator {

    };

    template<typename TIterator, typename std::enable_if<std::is_same<SparseIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last) {
        assert(first != last);
        assert(first + 1 != last);

        std::uint32_t i = 0;
        std::uint32_t max;
        bool sorted;

        do {
            sorted = true;
            std::uint32_t j = i;

            TIterator previous = first;
            TIterator current = previous;
            for (++current; (current != last && ((i) ? (j != max) : true)); ++current) {
                typename TIterator::Reference value = *current;
                typename TIterator::Reference previous_value = *previous;

                if (value < previous_value) {
                    typename TIterator::ValueType tmp_value = std::move(previous_value);
                    previous_value = std::move(value);
                    value = std::move(tmp_value);
                    sorted = false;
                }

                ++previous;
                ++j;
            }

            if (i == 0) {
                max = j;
            }
        } while (i++ != max && !sorted);
    }

    template<typename TIterator, typename TCompare, typename std::enable_if<std::is_same<SparseIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last, TCompare cmp) {
        assert(first != last);
        assert(first + 1 != last);

        std::uint32_t i = 0;
        std::uint32_t max;
        bool sorted;

        do {
            sorted = true;
            std::uint32_t j = i;

            TIterator previous = first;
            TIterator current = previous;
            for (++current; (current != last && ((i) ? (j != max) : true)); ++current) {
                typename TIterator::Reference value = *current;
                typename TIterator::Reference previous_value = *previous;

                if (cmp(value, previous_value)) {
                    typename TIterator::ValueType tmp_value = std::move(previous_value);
                    previous_value = std::move(value);
                    value = std::move(tmp_value);
                    sorted = false;
                }

                ++previous;
                ++j;
            }

            if (i == 0) {
                max = j;
            }
        } while (i++ != max && !sorted);
    }

    template<typename TIterator, typename std::enable_if<std::is_same<ListIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last) {
        assert(first != last);
        assert(first + 1 != last);

        std::uint32_t i = 0;
        std::uint32_t max;
        bool sorted;

        do {
            sorted = true;
            std::uint32_t j = i;

            TIterator previous = first;
            TIterator current = previous;
            for (++current; (current != last && ((i) ? (j != max) : true)); ++current) {
                typename TIterator::Reference value = *current;
                typename TIterator::Reference previous_value = *previous;

                if (value < previous_value) {
                    value.swap(previous_value);
                    sorted = false;

                    current = TIterator(&previous_value);
                    previous = TIterator(&value);
                }

                ++previous;
                ++j;
            }

            if (i == 0) {
                max = j;
            }
        } while (i++ != max && !sorted);
    }
    
    template<typename TIterator, typename TCompare, typename std::enable_if<std::is_same<ListIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last, TCompare cmp) {
        assert(first != last);
        assert(first + 1 != last);

        std::uint32_t i = 0;
        std::uint32_t max;
        bool sorted;

        do {
            sorted = true;
            std::uint32_t j = i;

            TIterator previous = first;
            TIterator current = previous;
            for (++current; (current != last && ((i) ? (j != max) : true)); ++current) {
                typename TIterator::Reference value = *current;
                typename TIterator::Reference previous_value = *previous;

                if (cmp(value, previous_value)) {
                    value.swap(previous_value);
                    sorted = false;

                    current = TIterator(&previous_value);
                    previous = TIterator(&value);
                }

                ++previous;
                ++j;
            }

            if (i == 0) {
                max = j;
            }
        } while (i++ != max && !sorted);
    }

    template<typename TIterator, typename std::enable_if<std::is_same<RandomAccessIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last) {
        assert(last - first > 2);

        TIterator i = first + 1;
        for (; i != last; ++i) {
            typename TIterator::ValueType key = std::move(*i);
            TIterator j = i - 1;

            while (j != (first - 1) && key < *j) {
                TIterator j_plus_1 = j + 1;
                *j_plus_1 = std::move(*j);
                --j;
            }
            TIterator j_plus_1 = j + 1;
            *j_plus_1 = std::move(key);
        }
    }
    
    template<typename TIterator, typename TCompare, typename std::enable_if<std::is_same<RandomAccessIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last, TCompare cmp) {
        assert(last - first > 2);

        TIterator i = first + 1;
        for (; i != last; ++i) {
            typename TIterator::ValueType key = std::move(*i);
            TIterator j = i - 1;

            while (abs(j - first) >= 0 && cmp(key, *j)) {
                TIterator j_plus_1 = j + 1;
                *j_plus_1 = std::move(*j);
                --j;
            }
            TIterator j_plus_1 = j + 1;
            *j_plus_1 = std::move(key);
        }
    }

    template<typename TIterator>
    inline void inplace_merge(TIterator first, TIterator middle, TIterator last) {
        assert(first != last);
        assert(first + 1 != last);

        while (first != middle && middle != last) {
            if (*first < *middle) {
                ++first;
            }
            else {
                typename TIterator::ValueType key = *middle;
                for (TIterator it = middle; middle != first; --middle) {
                    *it = *(middle - 1);
                }

                *first = key;
                ++first;
                ++middle;
            }
        }
    }

    template<typename TIterator, typename TCompare>
    inline void inplace_merge(TIterator first, TIterator middle, TIterator last, TCompare cmp) {
        assert(first != last);
        assert(first + 1 != last);

        while (first != middle && middle != last) {
            if (cmp(*first, *middle)) {
                ++first;
            }
            else {
                typename TIterator::ValueType key = *middle;
                for (TIterator it = middle; middle != first; --middle) {
                    *it = *(middle - 1);
                }

                *first = key;
                ++first;
                ++middle;
            }
        }
    }
}
