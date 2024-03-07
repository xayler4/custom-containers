#pragma once

#include <type_traits>
#include <cstdint>
#include <utility> 

namespace ccnt {
    class IndexableIterator {

    };

    class ReverseIndexableIterator {

    };

    class SparseIterator {

    };

    class ListIterator {

    };

    template<typename TIterator, typename std::enable_if<std::is_same<SparseIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last) {
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
    

    template<typename TIterator, typename std::enable_if<std::is_same<IndexableIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last) {
        typename TIterator::Pointer data = &(*first);
        typename TIterator::Pointer data_end = &(*last);
        assert(data < data_end);

        std::uint32_t count = data_end - data;
        for (std::uint32_t i = 1; i < count; i++) {
            typename TIterator::ValueType key = std::move(data[i]);
            std::int32_t j = i - 1;

            while (j >= 0 && key < data[j]) {
                data[j + 1] = std::move(data[j]);
                j--;
            }
            data[j + 1] = std::move(key);
        }
    }

    template<typename TIterator, typename TCompare, typename std::enable_if<std::is_same<IndexableIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last, TCompare cmp) {
        typename TIterator::Pointer data = &(*first);
        typename TIterator::Pointer data_end = &(*last);
        assert(data < data_end);

        std::uint32_t count = data_end - data;
        for (std::uint32_t i = 1; i < count; i++) {
            typename TIterator::ValueType key = std::move(data[i]);
            std::int32_t j = i - 1;

            while (j >= 0 && cmp(key, data[j])) {
                data[j + 1] = std::move(data[j]);
                j--;
            }
            data[j + 1] = std::move(key);
        }
    }

    template<typename TIterator, typename std::enable_if<std::is_same<ReverseIndexableIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last) {
        typename TIterator::Pointer data_end = &(*first);
        typename TIterator::Pointer data = &(*last);
        assert(data_end < data);

        std::uint32_t count = data_end - data;
        for (std::int32_t i = count - 2; i >= 0; i--) {
            typename TIterator::ValueType key = std::move(data[i]);
            std::int32_t j = i + 1;

            while (j < count && key < data[j]) {
                data[j - 1] = std::move(data[j]);
                j++;
            }
            data[j - 1] = std::move(key);
        }
    }

    template<typename TIterator, typename TCompare, typename std::enable_if<std::is_same<ReverseIndexableIterator, typename TIterator::Type>::value, std::nullptr_t>::type = nullptr>
    inline void sort(TIterator first, TIterator last, TCompare cmp) {
        typename TIterator::Pointer data_end = &(*first);
        typename TIterator::Pointer data = &(*last);
        assert(data_end < data);

        std::uint32_t count = data_end - data;
        for (std::int32_t i = count - 2; i >= 0; i--) {
            typename TIterator::ValueType key = std::move(data[i]);
            std::int32_t j = i + 1;

            while (j < count && cmp(key, data[j])) {
                data[j - 1] = std::move(data[j]);
                j++;
            }
            data[j - 1] = std::move(key);
        }
    }
}
