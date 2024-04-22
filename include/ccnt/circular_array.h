#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>
#include "algorithm.h"

namespace ccnt  {
    template<typename TValue, std::uint32_t TCapacity, typename TAllocator = std::allocator<TValue>>
    class CircularArray {
    public:
        class Iterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = TValue;
            using Pointer   = TValue*;
            using Reference = TValue&;

        public:
            Iterator(Pointer data) : m_data(data) {
            };

            ~Iterator() = default;

            Reference operator * () {
                return *m_data;
            }

            Pointer operator -> () {
                return m_data; 
            }

            Iterator operator + (std::uint32_t index) {
                return Iterator(m_data + index);
            }

            Iterator& operator += (std::uint32_t index) {
                m_data += index;

                return *this;
            }

            Iterator operator - (std::uint32_t index) {
                return Iterator(m_data - index);
            }

            Iterator& operator -= (std::uint32_t index) {
                m_data -= index;

                return *this;
            }

            std::int32_t operator - (const Iterator& iterator) {
                return m_data - iterator.m_data;
            }

            void operator ++ () {
                m_data++;
            }

            void operator -- () {
                m_data--;
            }

            bool operator == (const Iterator& it) {
                return m_data == it.m_data;
            }

            bool operator != (const Iterator& it) {
                return m_data != it.m_data;
            }

        protected:
            Pointer m_data;
        };

        class ReverseIterator : public Iterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = TValue;
            using Pointer   = TValue*;
            using Reference = TValue&;

        public:
            ReverseIterator(Pointer pointer) : Iterator(pointer) {}
            ~ReverseIterator() = default;

            Reference operator * () {
                return *(Iterator::m_data - 1);
            }

            Pointer operator -> () {
                return Iterator::m_data - 1; 
            }

            ReverseIterator operator + (std::uint32_t index) {
                return ReverseIterator(Iterator::m_data - index);
            }

            ReverseIterator& operator += (std::uint32_t index) {
                Iterator::m_data -= index;

                return *this;
            }

            ReverseIterator operator - (std::uint32_t index) {
                return ReverseIterator(Iterator::m_data + index);
            }

            ReverseIterator& operator -= (std::uint32_t index) {
                Iterator::m_data += index;

                return *this;
            }

            std::int32_t operator - (const ReverseIterator& iterator) {
                return Iterator::m_data - iterator.m_data;
            }

            void operator ++ () {
                Iterator::m_data--;
            }

            void operator -- () {
                Iterator::m_data++;
            }

            bool operator == (const ReverseIterator& it) {
                return Iterator::m_data == it.m_data;
            }

            bool operator != (const ReverseIterator& it) {
                return Iterator::m_data != it.m_data;
            }
        };

        class ConstIterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = const TValue;
            using Pointer   = const TValue*;
            using Reference = const TValue&;

        public:
            ConstIterator(TValue* data) : m_data(data) {
            };

            ~ConstIterator() = default;

            Reference operator * () const {
                return *m_data;
            }

            Pointer operator -> () const {
                return m_data; 
            }

            ConstIterator operator + (std::uint32_t index) {
                return ConstIterator(m_data + index);
            }

            ConstIterator& operator += (std::uint32_t index) {
                m_data += index;

                return *this;
            }

            ConstIterator operator - (std::uint32_t index) {
                return ConstIterator(m_data - index);
            }

            ConstIterator& operator -= (std::uint32_t index) {
                m_data -= index;

                return *this;
            }

            std::int32_t operator - (const ConstIterator& iterator) {
                return m_data - iterator.m_data;
            }

            void operator ++ () {
                m_data++;
            }

            void operator -- () {
                m_data--;
            }

            bool operator == (const ConstIterator& it) const {
                return m_data == it.m_data;
            }

            bool operator != (const ConstIterator& it) const {
                return m_data != it.m_data;
            }

        protected:
            TValue* m_data;
        };

        class ConstReverseIterator : public ConstIterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = const TValue;
            using Pointer   = const TValue*;
            using Reference = const TValue&;

        public:
            ConstReverseIterator(TValue* data) : ConstIterator(data) {}
            ~ConstReverseIterator() = default;

            Reference operator * () {
                return *(ConstIterator::m_data - 1);
            }

            Pointer operator -> () {
                return ConstIterator::m_data - 1; 
            }

            ConstReverseIterator operator + (std::uint32_t index) {
                return ConstReverseIterator(ConstIterator::m_data - index);
            }

            ConstReverseIterator& operator += (std::uint32_t index) {
                ConstIterator::m_data -= index;

                return *this;
            }

            ConstReverseIterator operator - (std::uint32_t index) {
                return ConstReverseIterator(ConstIterator::m_data + index);
            }

            ConstReverseIterator& operator -= (std::uint32_t index) {
                ConstIterator::m_data += index;

                return *this;
            }

            void operator ++ () {
                ConstIterator::m_data--;
            }

            void operator -- () {
                ConstIterator::m_data++;
            }

            bool operator == (const ConstReverseIterator& it) const {
                return ConstIterator::m_data == it.m_data;
            }

            bool operator != (const ConstReverseIterator& it) const {
                return ConstIterator::m_data != it.m_data;
            }
        };

    public:
        CircularArray() : m_count(0) {
            m_data = m_allocator.allocate(TCapacity);
            m_current = m_data;
        };

        ~CircularArray() {
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::destroy_at(m_data + i);
            }
            m_allocator.deallocate(m_data, TCapacity);
        }

        CircularArray(const CircularArray&) = delete;
        CircularArray& operator= (const CircularArray&) = default;
        
        template<typename... TArgs>
        inline void emplace(TArgs&&... args) {
            if (m_current == m_data + TCapacity - 1) {
                m_current = m_data;
            }
            else {
                ++m_current;
            }
            if (!(m_count == TCapacity)) {
                m_count++; 
            }
            *(m_current) = std::move<TValue>(TValue(std::forward<TArgs>(args)...));
        }

        inline void push(const TValue& value) {
            if (m_current == m_data + TCapacity - 1) {
                m_current = m_data;
            }
            else {
                ++m_current;
            }
            if (!(m_count == TCapacity)) {
                m_count++; 
            }
            *(m_current) = std::move(value);
        }

        inline TValue& operator [] (std::uint32_t index) {
            assert(index < TCapacity);
            return m_data[index];
        }

        inline std::uint32_t get_capacity() const {
            return TCapacity;
        }

        inline std::uint32_t get_count() const {
            return m_count;
        }

        inline const TValue* get_data() const {
            return m_data;
        }

        inline TValue* const get_current() {
            assert(m_count);
            return m_current;
        }

        Iterator begin() {
            return Iterator(m_data);
        }

        Iterator end() {
            return Iterator(m_data + TCapacity);
        }

        ConstIterator begin() const {
            return ConstIterator(m_data);
        }

        ConstIterator end() const {
            return ConstIterator(m_data + TCapacity);
        }

        ConstIterator cbegin() const {
            return ConstIterator(m_data);
        }

        ConstIterator cend() const {
            return ConstIterator(m_data + TCapacity);
        }

        ReverseIterator rbegin() {
            return ReverseIterator(m_data + TCapacity);
        }

        ReverseIterator rend() {
            return ReverseIterator(m_data);
        }

        ConstReverseIterator crbegin() const {
            return ConstReverseIterator(m_data + TCapacity);
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(m_data);
        }

    private:
        TAllocator m_allocator;
        TValue* m_data;
        TValue* m_current;
        std::uint32_t m_count;
    };
}
