#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>
#include <type_traits>
#include "bitset.h"

namespace ccnt {
    template<std::uint32_t TBitsGrowth = 32, typename TAllocator = std::allocator<Bitset<TBitsGrowth>>>
    class DynamicBitset {
    public:
        static constexpr std::uint32_t size() {
            float size = static_cast<float>(TBitsGrowth) / 64;
            return size + 1;
        } 

        using uint_t = std::conditional_t<(TBitsGrowth <= 8), std::uint8_t,
                std::conditional_t<(TBitsGrowth <= 16), std::uint16_t,
                std::conditional_t<(TBitsGrowth <= 32), std::uint32_t,
                std::conditional_t<(TBitsGrowth <= 64), std::uint64_t,
                std::array<uint64_t, size()>>>>>;

    public:
        class Iterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = Bitset<TBitsGrowth>::Proxy;
            using Reference = Bitset<TBitsGrowth>::Proxy;;

        public:
            Iterator(DynamicBitset<TBitsGrowth, TAllocator>* bitset, std::uint32_t nbit) : m_bitset(bitset), m_nbit(nbit) {}
            ~Iterator() = default;

            Reference operator * () {
                return (*m_bitset)[m_nbit];
            }

            Pointer operator -> () {
                return (*m_bitset)[m_nbit];
            }

            Iterator operator + (std::uint32_t index) {
                return Iterator(m_bitset, m_nbit + index);
            }

            Iterator& operator += (std::uint32_t index) {
                m_nbit += index;

                return *this;
            }

            Iterator operator - (std::uint32_t index) {
                return Iterator(m_bitset, m_nbit - index);
            }

            Iterator& operator -= (std::uint32_t index) {
                m_nbit -= index;

                return *this;
            }

            std::int32_t operator - (const Iterator& iterator) {
                return m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                m_nbit++;
            }

            void operator -- () {
                m_nbit--;
            }

            bool operator != (const Iterator& it ) {
                return ((it.m_bitset != m_bitset) || (m_nbit != it.m_nbit));
            }

            bool operator == (const Iterator& it ) {
                return ((it.m_bitset == m_bitset) && (m_nbit == it.m_nbit));
            }

        protected:
            DynamicBitset<TBitsGrowth, TAllocator>* m_bitset;
            std::uint32_t m_nbit;
        };

        class ReverseIterator : public Iterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = Bitset<TBitsGrowth>::Proxy;
            using Reference = Bitset<TBitsGrowth>::Proxy;;

        public:
            ReverseIterator(DynamicBitset<TBitsGrowth, TAllocator>* bitset, std::uint32_t nbit) : Iterator(bitset, nbit) {}
            ~ReverseIterator() = default;

            Reference operator * () {
                return (*Iterator::m_bitset)[Iterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*Iterator::m_bitset)[Iterator::m_nbit - 1];
            }

            ReverseIterator operator + (std::uint32_t index) {
                return ReverseIterator(Iterator::m_bitset, Iterator::m_nbit - index);
            }

            ReverseIterator& operator += (std::uint32_t index) {
                Iterator::m_nbit -= index;

                return *this;
            }

            ReverseIterator operator - (std::uint32_t index) {
                return ReverseIterator(Iterator::m_bitset, Iterator::m_nbit + index);
            }

            ReverseIterator& operator -= (std::uint32_t index) {
                Iterator::m_nbit += index;

                return *this;
            }

            std::int32_t operator - (const ReverseIterator& iterator) {
                return Iterator::m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                Iterator::m_nbit--;
            }

            void operator -- () {
                Iterator::m_nbit++;
            }

            bool operator != (const ReverseIterator& it ) {
                return ((it.m_bitset != Iterator::m_bitset) || (Iterator::m_nbit != it.m_nbit));
            }

            bool operator == (const ReverseIterator& it ) {
                return ((it.m_bitset == Iterator::m_bitset) && (Iterator::m_nbit == it.m_nbit));
            }
        };

        class ConstIterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;;

        public:
            ConstIterator(const DynamicBitset<TBitsGrowth, TAllocator>* bitset, std::uint32_t nbit) : m_bitset(bitset), m_nbit(nbit) {}
            ~ConstIterator() = default;

            Reference operator * () const {
                return (*m_bitset)[m_nbit];
            }

            Pointer operator -> () const {
                return (*m_bitset)[m_nbit];
            }

            ConstIterator operator + (std::uint32_t index) {
                return ConstIterator(m_bitset, m_nbit + index);
            }

            ConstIterator& operator += (std::uint32_t index) {
                m_nbit += index;

                return *this;
            }

            ConstIterator operator - (std::uint32_t index) {
                return ConstIterator(m_bitset, m_nbit - index);
            }

            ConstIterator& operator -= (std::uint32_t index) {
                m_nbit -= index;

                return *this;
            }

            std::int32_t operator - (const ConstIterator& iterator) {
                return m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                m_nbit++;
            }

            void operator -- () {
                m_nbit--;
            }

            bool operator != (const ConstIterator& it ) const {
                return ((it.m_bitset != m_bitset) || (m_nbit != it.m_nbit));
            }

            bool operator == (const ConstIterator& it ) const {
                return ((it.m_bitset == m_bitset) && (m_nbit == it.m_nbit));
            }

        protected:
            const DynamicBitset<TBitsGrowth, TAllocator>* m_bitset;
            std::uint32_t m_nbit;
        };

        class ConstReverseIterator : public ConstIterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;

        public:
            ConstReverseIterator(const DynamicBitset<TBitsGrowth, TAllocator>* bitset, std::uint32_t nbit) : ConstIterator(bitset, nbit) {}
            ~ConstReverseIterator() = default;

            Reference operator * () {
                return (*ConstIterator::m_bitset)[ConstIterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*ConstIterator::m_bitset)[ConstIterator::m_nbit - 1];
            }

            ConstReverseIterator operator + (std::uint32_t index) {
                return ConstReverseIterator(ConstIterator::m_bitset, ConstIterator::m_nbit - index);
            }

            ConstReverseIterator& operator += (std::uint32_t index) {
                ConstIterator::m_nbit -= index;

                return *this;
            }

            ConstReverseIterator operator - (std::uint32_t index) {
                return ConstReverseIterator(ConstIterator::m_bitset, ConstIterator::m_nbit + index);
            }

            ConstReverseIterator operator -= (std::uint32_t index) {
                ConstIterator::m_nbit += index;
            }

            std::int32_t operator - (const ConstReverseIterator& iterator) {
                return ConstIterator::m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                ConstIterator::m_nbit--;
            }

            void operator -- () {
                ConstIterator::m_nbit++;
            }
        };

    public:
        DynamicBitset() : m_count(0), m_capacity(1) {
            static_assert(TBitsGrowth == sizeof(std::uint8_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint16_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint32_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * size() * 8
                        );

            m_bitsets = m_allocator.allocate(1);
            std::construct_at(m_bitsets);
        }

        explicit DynamicBitset(std::uint32_t nbits_capacity) : m_count(0), m_capacity(nbits_capacity/TBitsGrowth + 1){
            static_assert(TBitsGrowth == sizeof(std::uint8_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint16_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint32_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * size() * 8
                        );

            m_bitsets = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::construct_at(m_bitsets + i);
            }
        }

        DynamicBitset(const DynamicBitset<TBitsGrowth>& bitset) : m_count(bitset.m_count), m_capacity(bitset.m_capacity), m_allocator(bitset.m_allocator) {
            m_bitsets = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                m_bitsets[i] = bitset.m_bitsets[i];
            }
        }

        DynamicBitset(DynamicBitset<TBitsGrowth>&& bitset) : m_bitsets(bitset.m_bitsets), m_count(bitset.m_count), m_capacity(bitset.m_capacity), m_allocator(bitset.m_allocator) {
            bitset.m_bitsets = nullptr;
            bitset.m_count = 0;
            bitset.m_capacity = 0;
        }

        ~DynamicBitset() {
            m_allocator.deallocate(m_bitsets, m_capacity);
        }

        inline void set_all_bits() {
            std::uint32_t index = m_count/TBitsGrowth;
            std::uint32_t relative_bit = m_count - index * TBitsGrowth;

            for (std::uint32_t i = 0; i < index; i++) {
                m_bitsets[i].set_all_bits();
            }
            for (std::uint32_t i = 0; i < relative_bit; i++) {
                m_bitsets[index].set_bit(relative_bit);
            }
        }

        inline void unset_all_bits() {
            std::uint32_t index = m_count/TBitsGrowth;

            for (std::uint32_t i = 0; i < index + 1; i++) {
                m_bitsets[i].unset_all_bits();
            }
        }

        inline void push_bit(bool value) {
            std::uint32_t index = m_count/TBitsGrowth;
            std::uint32_t relative_bit = m_count - index * TBitsGrowth;

            if (m_capacity <= index) {
                grow();
            } 
            if (value) {
                m_bitsets[index].set_bit(relative_bit);
            }
            else {
                m_bitsets[index].unset_bit(relative_bit);
            }
            m_count++;
        }

        inline void pop_bit() {
            assert(m_count != 0);

            std::uint32_t index = m_count/TBitsGrowth;
            std::uint32_t relative_bit = m_count - index * TBitsGrowth;

            m_bitsets[index].unset_bit(relative_bit);
            m_count--;
        }

        inline void set_bit(std::uint32_t nbit) {
            assert(nbit < m_count);

            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;
            
            m_bitsets[index].set_bit(relative_bit);
        }

        inline void unset_bit(std::uint32_t nbit) {
            assert(nbit < m_count);

            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;
            
            m_bitsets[index].unset_bit(relative_bit);
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
                    std::construct_at(m_bitsets + i);
                }

                for (; relative_bit < TBitsGrowth; relative_bit++) {
                    m_bitsets[index].unset_bit(relative_bit);
                }
            } 
            else {
                if (m_capacity <= index) {
                    reserve(count);
                } 

                if (value){
                    for (; relative_bit < TBitsGrowth; relative_bit++) {
                        m_bitsets[index].set_bit(relative_bit);
                    }
                }
            }


            m_count = count;
        }

        inline void reserve(std::uint32_t nbits) {
            std::uint32_t capacity = nbits/TBitsGrowth + 1;
            assert(m_capacity < capacity);

            Bitset<TBitsGrowth>* tmp_bitset = m_bitsets;
            m_bitsets = m_allocator.allocate(capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::construct_at(m_bitsets + i, tmp_bitset[i]);
            }
            for (std::uint32_t i = m_capacity; i < capacity; i++) {
                std::construct_at(m_bitsets + i);
            }
            m_allocator.deallocate(tmp_bitset, m_capacity);
            m_capacity = capacity;
        }

        inline void clear() {
            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;
            for (std::uint32_t i = 0; i < nbitsets; i++) {
                std::construct_at(m_bitsets + i);
            }
            m_count = 0;
        }

        inline Bitset<TBitsGrowth>::Proxy operator[] (std::uint32_t nbit) {
            assert(nbit < m_count);
            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;

            return m_bitsets[index][relative_bit];
        }

        inline DynamicBitset<TBitsGrowth>& operator = (const DynamicBitset<TBitsGrowth>& bitset) {
            m_count = bitset.m_count;
            m_capacity = bitset.m_capacity;
            m_allocator = bitset.m_allocator;

            m_bitsets = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                m_bitsets[i] = bitset.m_bitsets[i];
            }

            return *this;
        }

        inline DynamicBitset<TBitsGrowth>& operator = (DynamicBitset<TBitsGrowth>&& bitset) {
            m_bitsets = bitset.m_bitsets;
            m_count = bitset.m_count;
            m_capacity = bitset.m_capacity;
            m_allocator = bitset.m_allocator;

            bitset.m_bitsets = nullptr;
            bitset.m_count = 0;
            bitset.m_capacity = 0;

            return *this;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>

        inline DynamicBitset<TBitsGrowth>& operator = (const TBitset& bitset) {
            assert(m_count != 0);
            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] = bitset;
            }

            return *this;
        }

        inline DynamicBitset<TBitsGrowth>& operator = (const uint_t& bitset) {
            assert(m_count != 0);
            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] = bitset;
            }

            return *this;
        }

        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < m_count);
            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;

            return m_bitsets[index][relative_bit];
        }

        inline DynamicBitset<TBitsGrowth> operator & (const DynamicBitset<TBitsGrowth>& bitset) const {
            assert(bitset.m_count <= m_count);
            assert(bitset.m_count != 0);

            std::uint32_t nbitsets = bitset.m_count/TBitsGrowth + 1;

            DynamicBitset out_bitset(bitset.m_count);
            out_bitset.resize(bitset.m_count);

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                out_bitset.m_bitsets[i] = m_bitsets[i] & bitset.m_bitsets[i];
            }

            return out_bitset;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitset<TBitsGrowth> operator & (const TBitset& bitset) const {
            assert(m_count != 0);

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            DynamicBitset out_bitset(m_count);
            out_bitset.resize(m_count);

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                out_bitset.m_bitsets[i] = m_bitsets[i] & bitset.m_bitsets[i];
            }

            return out_bitset;
        }

        inline DynamicBitset<TBitsGrowth>& operator &= (const DynamicBitset<TBitsGrowth>& bitset) {
            assert(bitset.m_count <= m_count);
            assert(bitset.m_count != 0);

            std::uint32_t nbitsets = bitset.m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] &= bitset.m_bitsets[i];
            }

            return *this;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitset<TBitsGrowth>& operator &= (const TBitset& bitset) {
            assert(m_count != 0);
            
            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] &= bitset;
            }

            return *this;
        }

        inline DynamicBitset<TBitsGrowth> operator ^ (const DynamicBitset<TBitsGrowth>& bitset) const {
            assert(bitset.m_count <= m_count);
            assert(bitset.m_count != 0);

            std::uint32_t nbitsets = bitset.m_count/TBitsGrowth + 1;

            DynamicBitset out_bitset(bitset.m_count);
            out_bitset.resize(bitset.m_count);

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                out_bitset.m_bitsets[i] = m_bitsets[i] ^ bitset.m_bitsets[i];
            }

            return out_bitset;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitset<TBitsGrowth> operator ^ (const TBitset& bitset) const {
            assert(m_count != 0);

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            DynamicBitset out_bitset(m_count);
            out_bitset.resize(m_count);

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                out_bitset.m_bitsets[i] = m_bitsets[i] ^ bitset.m_bitsets[i];
            }

            return out_bitset;
        }

        inline DynamicBitset<TBitsGrowth>& operator ^= (const DynamicBitset<TBitsGrowth>& bitset) {
            assert(bitset.m_count <= m_count);
            assert(bitset.m_count != 0);

            std::uint32_t nbitsets = bitset.m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] ^= bitset.m_bitsets[i];
            }

            return *this;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitset<TBitsGrowth>& operator ^= (const TBitset& bitset) {
            assert(m_count != 0);
            
            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] ^= bitset;
            }

            return *this;
        }

        inline DynamicBitset<TBitsGrowth> operator | (const DynamicBitset<TBitsGrowth>& bitset) const {
            assert(bitset.m_count <= m_count);
            assert(bitset.m_count != 0);

            std::uint32_t nbitsets = bitset.m_count/TBitsGrowth + 1;

            DynamicBitset out_bitset(bitset.m_count);
            out_bitset.resize(bitset.m_count);

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                out_bitset.m_bitsets[i] = m_bitsets[i] | bitset.m_bitsets[i];
            }

            return out_bitset;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitset<TBitsGrowth> operator | (const TBitset& bitset) const {
            assert(m_count != 0);

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            DynamicBitset out_bitset(m_count);
            out_bitset.resize(m_count);

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                out_bitset.m_bitsets[i] = m_bitsets[i] | bitset.m_bitsets[i];
            }

            return out_bitset;
        }

        inline DynamicBitset<TBitsGrowth>& operator |= (const DynamicBitset<TBitsGrowth>& bitset) {
            assert(bitset.m_count <= m_count);
            assert(bitset.m_count != 0);

            std::uint32_t nbitsets = bitset.m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] |= bitset.m_bitsets[i];
            }

            return *this;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline DynamicBitset<TBitsGrowth>& operator |= (const TBitset& bitset) {
            assert(m_count != 0);
            
            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                m_bitsets[i] &= bitset;
            }

            return *this;
        }


        inline bool operator == (const DynamicBitset<TBitsGrowth>& bitset) const {
            if (bitset.m_count != m_count || m_count == 0) {
                return false;
            }

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                if (m_bitsets[i] != bitset.m_bitsets[i]) {
                    return false;
                }
            }

            return true;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const TBitset& bitset) const {
            if (m_count == 0) {
                return false;
            }

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                if (m_bitsets[i] != bitset) {
                    return false;
                }
            }

            return true;
        }

        inline bool operator == (const uint_t& bitset) const {
            if (m_count == 0) {
                return false;
            }

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                if (m_bitsets[i] != bitset) {
                    return false;
                }
            }

            return true;
        }

        inline bool operator != (const DynamicBitset<TBitsGrowth>& bitset) const {
            if (bitset.m_count != m_count || m_count == 0) {
                return true;
            }

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                if (m_bitsets[i] == bitset.m_bitsets[i]) {
                    return false;
                }
            }

            return true;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const TBitset& bitset) const {
            if (m_count == 0) {
                return true;
            }

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                if (m_bitsets[i] == bitset) {
                    return false;
                }
            }

            return true;
        }

        inline bool operator != (const uint_t& bitset) const {
            if (m_count == 0) {
                return true;
            }

            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitsets; i++) {
                if (m_bitsets[i] == bitset) {
                    return false;
                }
            }

            return true;
        }

        inline std::uint32_t pop_count() const {
            std::uint32_t nbitsets = m_count/TBitsGrowth + 1;

            std::uint32_t count = 0;
            for (std::uint32_t i = 0; i < nbitsets; i++) {
                count += m_bitsets[i].pop_count();
            }

            return count;
        }

        inline Bitset<TBitsGrowth>* const get_data() const {
            return m_bitsets;
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
            Bitset<TBitsGrowth>* tmp_bitset = m_bitsets;
            m_bitsets = m_allocator.allocate(m_capacity + 1);
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::construct_at(m_bitsets + i, tmp_bitset[i]);
            }
            std::construct_at(m_bitsets + m_capacity);
            m_allocator.deallocate(tmp_bitset, m_capacity);
            m_capacity++;
        }

    private:
        Bitset<TBitsGrowth>* m_bitsets;
        std::uint32_t m_count;
        std::uint32_t m_capacity;
        TAllocator m_allocator;
    };
}
