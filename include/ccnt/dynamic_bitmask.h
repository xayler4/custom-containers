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
        }

        DynamicBitmask(std::uint32_t capacity) : m_count(0), m_capacity(capacity) {
            assert(capacity != 0);
            static_assert(TBitsGrowth == sizeof(std::uint8_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint16_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint32_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * 8 ||
                          TBitsGrowth == sizeof(std::uint64_t) * size() * 8
                        );

            m_bitmasks = m_allocator.allocate(m_capacity);
        }

        DynamicBitmask(DynamicBitmask<TBitsGrowth>&& bitmask) : m_bitmasks(bitmask.m_bitmasks), m_count(bitmask.m_count), m_capacity(bitmask.m_capacity), m_allocator(bitmask.m_allocator) {
            bitmask.m_bitmasks = nullptr;
            bitmask.m_count = 0;
            bitmask.m_capacity = 0;
        }

        ~DynamicBitmask() {
            m_allocator.deallocate(m_bitmasks, m_capacity);
        }

        void push_bit(bool value) {
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

        void pop_bit() {
            std::uint32_t index = m_count/TBitsGrowth;
            std::uint32_t relative_bit = m_count - index * TBitsGrowth;

            m_bitmasks[index].unset_bit(relative_bit);
            m_count--;
        }

        void resize(std::uint32_t count) {
            m_count = count;
            std::uint32_t index = m_count/TBitsGrowth;

            if (m_capacity <= index) {
                reserve(m_count);
            } 
        }

        void reserve(std::uint32_t nbits) {
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

        void clear() {
            m_count = 0;
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

        inline Bitmask<TBitsGrowth>::Proxy operator[] (std::uint32_t nbit) {
            assert(nbit < m_count);
            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;

            return m_bitmasks[index][relative_bit];
        }

        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < m_count);
            std::uint32_t index = nbit/TBitsGrowth;
            std::uint32_t relative_bit = nbit - index * TBitsGrowth;

            return m_bitmasks[index][relative_bit];
        }

        inline DynamicBitmask<TBitsGrowth> operator & (const DynamicBitmask<TBitsGrowth>& bitmask) {
            std::uint32_t min_count = (m_count < bitmask.m_count) ? m_count : bitmask.m_count;
            std::uint32_t nbitmasks = min_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(nbitmasks);
            out_bitmask.resize(min_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask.m_bitmasks[i] &= bitmask.m_bitmasks[i];
            }

            return out_bitmask;
        }

        inline DynamicBitmask<TBitsGrowth>& operator &= (const DynamicBitmask<TBitsGrowth>& bitmask) {
            std::uint32_t min_count = (m_count < bitmask.m_count) ? m_count : bitmask.m_count;
            std::uint32_t nbitmasks = min_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] &= bitmask.m_bitmasks[i];
            }

            return *this;
        }

        inline DynamicBitmask<TBitsGrowth> operator | (const DynamicBitmask<TBitsGrowth>& bitmask) {
            std::uint32_t min_count = (m_count < bitmask.m_count) ? m_count : bitmask.m_count;
            std::uint32_t nbitmasks = min_count/TBitsGrowth + 1;

            DynamicBitmask out_bitmask(nbitmasks);
            out_bitmask.resize(min_count);

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                out_bitmask[i] |= bitmask[i];
            }

            return out_bitmask;
        }

        inline DynamicBitmask<TBitsGrowth>& operator |= (const DynamicBitmask<TBitsGrowth>& bitmask) {
            std::uint32_t min_count = (m_count < bitmask.m_count) ? m_count : bitmask.m_count;
            std::uint32_t nbitmasks = min_count/TBitsGrowth + 1;

            for (std::uint32_t i = 0; i < nbitmasks; i++) {
                m_bitmasks[i] |= bitmask[i];
            }

            return *this;
        }

        inline Bitmask<TBitsGrowth>* const get_bitmasks() const {
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
        void grow() {
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
