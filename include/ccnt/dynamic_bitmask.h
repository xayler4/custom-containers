#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>
#include <type_traits>
#include "bitmask.h"

namespace ccnt {
    template<std::uint32_t TBitsGrowth = 32, typename TAllocator = std::allocator<Bitmask<TBitsGrowth>>>
    class DynamicBitmask {
    public:
        static constexpr std::uint32_t size() {
            float size = static_cast<float>(TBitsGrowth) / 64;
            return ceil(size);
        } 

        using uint_t = std::conditional_t<(TBitsGrowth <= 8), std::uint8_t,
                std::conditional_t<(TBitsGrowth <= 16), std::uint16_t,
                std::conditional_t<(TBitsGrowth <= 32), std::uint32_t,
                std::conditional_t<(TBitsGrowth <= 64), std::uint64_t,
                std::array<uint64_t, size()>>>>>;

    public:
        class Iterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = Bitmask<TBitsGrowth>::Proxy;
            using Reference = Bitmask<TBitsGrowth>::Proxy;;

        public:
            Iterator(DynamicBitmask<TBitsGrowth, TAllocator>* bitmask, std::uint32_t nbit) : m_bitmask(bitmask), m_nbit(nbit) {}
            ~Iterator() = default;

            Reference operator * () {
                return (*m_bitmask)[m_nbit];
            }

            Pointer operator -> () {
                return (*m_bitmask)[m_nbit];
            }

            void operator ++ () {
                m_nbit++;
            }

            bool operator != (const Iterator& it ) {
                return ((it.m_bitmask != m_bitmask) || (m_nbit != it.m_nbit));
            }

            bool operator == (const Iterator& it ) {
                return ((it.m_bitmask == m_bitmask) && (m_nbit == it.m_nbit));
            }

        protected:
            DynamicBitmask<TBitsGrowth, TAllocator>* m_bitmask;
            std::uint32_t m_nbit;
        };

        class ReverseIterator : public Iterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = Bitmask<TBitsGrowth>::Proxy;
            using Reference = Bitmask<TBitsGrowth>::Proxy;;

        public:
            ReverseIterator(DynamicBitmask<TBitsGrowth, TAllocator>* bitmask, std::uint32_t nbit) : Iterator(bitmask, nbit) {}
            ~ReverseIterator() = default;

            Reference operator * () {
                return (*Iterator::m_bitmask)[Iterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*Iterator::m_bitmask)[Iterator::m_nbit - 1];
            }

            void operator ++ () {
                Iterator::m_nbit--;
            }

            bool operator != (const ReverseIterator& it ) {
                return ((it.m_bitmask != Iterator::m_bitmask) || (Iterator::m_nbit != it.m_nbit));
            }

            bool operator == (const ReverseIterator& it ) {
                return ((it.m_bitmask == Iterator::m_bitmask) && (Iterator::m_nbit == it.m_nbit));
            }
        };

        class ConstIterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;;

        public:
            ConstIterator(const DynamicBitmask<TBitsGrowth, TAllocator>* bitmask, std::uint32_t nbit) : m_bitmask(bitmask), m_nbit(nbit) {}
            ~ConstIterator() = default;

            Reference operator * () {
                return (*m_bitmask)[m_nbit];
            }

            Pointer operator -> () {
                return (*m_bitmask)[m_nbit];
            }

            void operator ++ () {
                m_nbit++;
            }

            bool operator != (const ConstIterator& it ) {
                return ((it.m_bitmask != m_bitmask) || (m_nbit != it.m_nbit));
            }

            bool operator == (const ConstIterator& it ) {
                return ((it.m_bitmask == m_bitmask) && (m_nbit == it.m_nbit));
            }

        protected:
            const DynamicBitmask<TBitsGrowth, TAllocator>* m_bitmask;
            std::uint32_t m_nbit;
        };

        class ConstReverseIterator : public ConstIterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;

        public:
            ConstReverseIterator(const DynamicBitmask<TBitsGrowth, TAllocator>* bitmask, std::uint32_t nbit) : ConstIterator(bitmask, nbit) {}
            ~ConstReverseIterator() = default;

            Reference operator * () {
                return (*ConstIterator::m_bitmask)[ConstIterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*ConstIterator::m_bitmask)[ConstIterator::m_nbit - 1];
            }

            void operator ++ () {
                ConstIterator::m_nbit--;
            }

            bool operator != (const ConstReverseIterator& it ) {
                return ((it.m_bitmask != ConstIterator::m_bitmask) || (ConstIterator::m_nbit != it.m_nbit));
            }

            bool operator == (const ConstReverseIterator& it ) {
                return ((it.m_bitmask == ConstIterator::m_bitmask) && (ConstIterator::m_nbit == it.m_nbit));
            }
        };

    public:
        DynamicBitmask() : m_count(0), m_capacity(1) {
            static_assert(TBitsGrowth == sizeof(std::uint8_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint16_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint32_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * size() * 8
                        );

            m_bitmasks = m_allocator.allocate(1);
            std::construct_at(m_bitmasks);
        }

        explicit DynamicBitmask(std::uint32_t nbits_capacity) : m_count(0), m_capacity(nbits_capacity/TBitsGrowth + 1){
            static_assert(TBitsGrowth == sizeof(std::uint8_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint16_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint32_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * size() * 8
                        );

            m_bitmasks = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::construct_at(m_bitmasks + i);
            }
        }

        DynamicBitmask(const DynamicBitmask<TBitsGrowth>& bitmask) : m_count(bitmask.m_count), m_capacity(bitmask.m_capacity), m_allocator(bitmask.m_allocator) {
            m_bitmasks = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                m_bitmasks[i] = bitmask.m_bitmasks[i];
            }
        }

        DynamicBitmask(DynamicBitmask<TBitsGrowth>&& bitmask) : m_bitmasks(bitmask.m_bitmasks), m_count(bitmask.m_count), m_capacity(bitmask.m_capacity), m_allocator(bitmask.m_allocator) {
            bitmask.m_bitmasks = nullptr;
            bitmask.m_count = 0;
            bitmask.m_capacity = 0;
        }

        ~DynamicBitmask() {
            m_allocator.deallocate(m_bitmasks, m_capacity);
        }

        inline void set_all_bits() {
            std::uint32_t index = m_count/TBitsGrowth;
            std::uint32_t relative_bit = m_count - index * TBitsGrowth;

            for (std::uint32_t i = 0; i < index; i++) {
                m_bitmasks[i].set_all_bits();
            }
            for (std::uint32_t i = 0; i < relative_bit; i++) {
                m_bitmasks[index].set_bit(relative_bit);
            }
        }

        inline void unset_all_bits() {
            std::uint32_t index = m_count/TBitsGrowth;

            for (std::uint32_t i = 0; i < index + 1; i++) {
                m_bitmasks[i].unset_all_bits();
            }
        }

        inline void push_bit(bool value) {
            std::uint32_t index = m_count/TBitsGrowth;
            std::uint32_t relative_bit = m_count - index * TBitsGrowth;

            if (m_capacity <= index) {
                grow();
            } 
            if (value) {
                m_bitmasks[index].set_bit(relative_bit);
            }
            else {
                m_bitmasks[index].unset_bit(relative_bit);
            }
            m_count++;
        }

        inline void pop_bit() {
            assert(m_count != 0);

            std::uint32_t index = m_count/TBitsGrowth;
            std::uint32_t relative_bit = m_count - index * TBitsGrowth;

            m_bitmasks[index].unset_bit(relative_bit);
            m_count--;
        }

        inline void set_bit(std::uint32_t nbit) {
            assert(nbit < m_count);

            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;
            
            m_bitmasks[index].set_bit(relative_bit);
        }

        inline void unset_bit(std::uint32_t nbit) {
            assert(nbit < m_count);

            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;
            
            m_bitmasks[index].unset_bit(relative_bit);
        }

        inline void resize(std::uint32_t count, bool value = false) {
            std::uint32_t index = count/TBitsGrowth;
            std::uint32_t relative_bit = count - (index) * TBitsGrowth;

            if (count == 0) {
                return clear();
            }
            if (count < m_count) {
                std::uint32_t old_index = m_count/TBitsGrowth;
                
                for (std::uint32_t i = index; i < old_index; i++) {
                    std::construct_at(m_bitmasks + i);
                }

                for (; relative_bit < TBitsGrowth; relative_bit++) {
                    m_bitmasks[index].unset_bit(relative_bit);
                }
            } 
            else {
                if (m_capacity <= index) {
                    reserve(count);
                } 

                if (value){
                    for (; relative_bit < TBitsGrowth; relative_bit++) {
                        m_bitmasks[index].set_bit(relative_bit);
                    }
                }
            }


            m_count = count;
        }

        inline void reserve(std::uint32_t nbits) {
            std::uint32_t capacity = nbits/TBitsGrowth + 1;
            assert(m_capacity < capacity);

            Bitmask<TBitsGrowth>* tmp_bitmask = m_bitmasks;
            m_bitmasks = m_allocator.allocate(capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::construct_at(m_bitmasks + i, tmp_bitmask[i]);
            }
            for (std::uint32_t i = m_capacity; i < capacity; i++) {
                std::construct_at(m_bitmasks + i);
            }
            m_allocator.deallocate(tmp_bitmask, m_capacity);
            m_capacity = capacity;
        }

        inline void clear() {
            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;
            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                std::construct_at(m_bitmasks + i);
            }
            m_count = 0;
        }

        inline Bitmask<TBitsGrowth>::Proxy operator[] (std::uint32_t nbit) {
            assert(nbit < m_count);
            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;

            return m_bitmasks[index][relative_bit];
        }

        inline DynamicBitmask<TBitsGrowth>& operator = (const DynamicBitmask<TBitsGrowth>& bitmask) {
            m_count = bitmask.m_count;
            m_capacity = bitmask.m_capacity;
            m_allocator = bitmask.m_allocator;

            m_bitmasks = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                m_bitmasks[i] = bitmask.m_bitmasks[i];
            }

            return *this;
        }

        inline DynamicBitmask<TBitsGrowth>& operator = (DynamicBitmask<TBitsGrowth>&& bitmask) {
            m_bitmasks = bitmask.m_bitmasks;
            m_count = bitmask.m_count;
            m_capacity = bitmask.m_capacity;
            m_allocator = bitmask.m_allocator;

            bitmask.m_bitmasks = nullptr;
            bitmask.m_count = 0;
            bitmask.m_capacity = 0;

            return *this;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>

        inline DynamicBitmask<TBitsGrowth>& operator = (const TBitmask& bitmask) {
            assert(m_count != 0);
            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] = bitmask;
            }

            return *this;
        }

        inline DynamicBitmask<TBitsGrowth>& operator = (const uint_t& bitmask) {
            assert(m_count != 0);
            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] = bitmask;
            }

            return *this;
        }

        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < m_count);
            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;

            return m_bitmasks[index][relative_bit];
        }

        inline DynamicBitmask<TBitsGrowth> operator & (const DynamicBitmask<TBitsGrowth>& bitmask) const {
            assert(bitmask.m_count <= m_count);
            assert(bitmask.m_count != 0);

            std::uint32_t nbitmasks = bitmask.m_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(bitmask.m_count);
            out_bitmask.resize(bitmask.m_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask.m_bitmasks[i] &= bitmask.m_bitmasks[i];
            }

            return out_bitmask;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitmask<TBitsGrowth> operator & (const TBitmask& bitmask) const {
            assert(m_count != 0);

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(m_count);
            out_bitmask.resize(m_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask.m_bitmasks[i] &= bitmask;
            }

            return out_bitmask;
        }

        inline DynamicBitmask<TBitsGrowth>& operator &= (const DynamicBitmask<TBitsGrowth>& bitmask) {
            assert(bitmask.m_count <= m_count);
            assert(bitmask.m_count != 0);

            std::uint32_t nbitmasks = bitmask.m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] &= bitmask.m_bitmasks[i];
            }

            return *this;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitmask<TBitsGrowth>& operator &= (const TBitmask& bitmask) {
            assert(m_count != 0);
            
            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] &= bitmask;
            }

            return *this;
        }

        inline DynamicBitmask<TBitsGrowth> operator ^ (const DynamicBitmask<TBitsGrowth>& bitmask) const {
            assert(bitmask.m_count <= m_count);
            assert(bitmask.m_count != 0);

            std::uint32_t nbitmasks = bitmask.m_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(bitmask.m_count);
            out_bitmask.resize(bitmask.m_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask.m_bitmasks[i] ^= bitmask.m_bitmasks[i];
            }

            return out_bitmask;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitmask<TBitsGrowth> operator ^ (const TBitmask& bitmask) const {
            assert(m_count != 0);

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(m_count);
            out_bitmask.resize(m_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask.m_bitmasks[i] ^= bitmask;
            }

            return out_bitmask;
        }

        inline DynamicBitmask<TBitsGrowth>& operator ^= (const DynamicBitmask<TBitsGrowth>& bitmask) {
            assert(bitmask.m_count <= m_count);
            assert(bitmask.m_count != 0);

            std::uint32_t nbitmasks = bitmask.m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] ^= bitmask.m_bitmasks[i];
            }

            return *this;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitmask<TBitsGrowth>& operator ^= (const TBitmask& bitmask) {
            assert(m_count != 0);
            
            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] ^= bitmask;
            }

            return *this;
        }

        inline DynamicBitmask<TBitsGrowth> operator | (const DynamicBitmask<TBitsGrowth>& bitmask) const {
            assert(bitmask.m_count <= m_count);
            assert(bitmask.m_count != 0);

            std::uint32_t nbitmasks = bitmask.m_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(bitmask.m_count);
            out_bitmask.resize(bitmask.m_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask.m_bitmasks[i] |= bitmask.m_bitmasks[i];
            }

            return out_bitmask;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitmask<TBitsGrowth> operator | (const TBitmask& bitmask) const {
            assert(m_count != 0);

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(m_count);
            out_bitmask.resize(m_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask.m_bitmasks[i] |= bitmask;
            }

            return out_bitmask;
        }

        inline DynamicBitmask<TBitsGrowth>& operator |= (const DynamicBitmask<TBitsGrowth>& bitmask) {
            assert(bitmask.m_count <= m_count);
            assert(bitmask.m_count != 0);

            std::uint32_t nbitmasks = bitmask.m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] |= bitmask.m_bitmasks[i];
            }

            return *this;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitmask<TBitsGrowth>& operator |= (const TBitmask& bitmask) {
            assert(m_count != 0);
            
            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] &= bitmask;
            }

            return *this;
        }


        inline bool operator == (const DynamicBitmask<TBitsGrowth>& bitmask) const {
            if (bitmask.m_count != m_count || m_count == 0) {
                return false;
            }

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                if (m_bitmasks[i] != bitmask.m_bitmasks[i]) {
                    return false;
                }
            }

            return true;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const TBitmask& bitmask) const {
            if (m_count == 0) {
                return false;
            }

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                if (m_bitmasks[i] != bitmask) {
                    return false;
                }
            }

            return true;
        }

        inline bool operator == (const uint_t& bitmask) const {
            if (m_count == 0) {
                return false;
            }

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                if (m_bitmasks[i] != bitmask) {
                    return false;
                }
            }

            return true;
        }

        inline bool operator != (const DynamicBitmask<TBitsGrowth>& bitmask) const {
            if (bitmask.m_count != m_count || m_count == 0) {
                return true;
            }

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                if (m_bitmasks[i] == bitmask.m_bitmasks[i]) {
                    return false;
                }
            }

            return true;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const TBitmask& bitmask) const {
            if (m_count == 0) {
                return true;
            }

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                if (m_bitmasks[i] == bitmask) {
                    return false;
                }
            }

            return true;
        }

        inline bool operator != (const uint_t& bitmask) const {
            if (m_count == 0) {
                return true;
            }

            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                if (m_bitmasks[i] == bitmask) {
                    return false;
                }
            }

            return true;
        }

        inline std::uint32_t pop_count() const {
            std::uint32_t nbitmasks = m_count/TBitsGrowth + 1;

            std::uint32_t count = 0;
            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                count += m_bitmasks[i].pop_count();
            }

            return count;
        }

        inline Bitmask<TBitsGrowth>* const get_data() const {
            return m_bitmasks;
        }

        inline std::uint32_t get_count() const {
            return m_count;
        }

        inline std::uint32_t get_capacity() const {
            return m_capacity;
        }

        Iterator begin() {
            return Iterator(this, 0);
        }

        Iterator end() {
            return Iterator(this, m_count);
        }

        ConstIterator begin() const {
            return ConstIterator(this, 0);
        }

        ConstIterator end() const {
            return ConstIterator(this, m_count);
        }

        ConstIterator cbegin() const {
            return ConstIterator(this, 0);
        }

        ConstIterator cend() const {
            return ConstIterator(this, m_count);
        }

        ReverseIterator rbegin() {
            return ReverseIterator(this, m_count);
        }

        ReverseIterator rend() {
            return ReverseIterator(this, 0);
        }

        ConstReverseIterator crbegin() const {
            return ConstReverseIterator(this, m_count);
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(this, 0);
        }

    private:
        inline void grow() {
            Bitmask<TBitsGrowth>* tmp_bitmask = m_bitmasks;
            m_bitmasks = m_allocator.allocate(m_capacity + 1);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::construct_at(m_bitmasks + i, tmp_bitmask[i]);
            }
            std::construct_at(m_bitmasks + m_capacity);
            m_allocator.deallocate(tmp_bitmask, m_capacity);
            m_capacity++;
        }

    private:
        Bitmask<TBitsGrowth>* m_bitmasks;
        std::uint32_t m_count;
        std::uint32_t m_capacity;
        TAllocator m_allocator;
    };
}
