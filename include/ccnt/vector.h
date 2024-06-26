#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>
#include "algorithm.h"

namespace ccnt {
    template<typename TValue, typename TAllocator = std::allocator<TValue>>
    class Vector {
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
        Vector() : m_count(0), m_capacity(2) {
            m_data = m_allocator.allocate(2);
        }

        Vector(std::uint32_t capacity) : m_count(0), m_capacity(capacity) {
            assert(m_capacity != 0);
            m_data = m_allocator.allocate(m_capacity);
        }

        Vector(Vector&& vector) : m_data(vector.m_data), m_capacity(vector.m_capacity), m_count(vector.m_count), m_allocator(vector.m_allocator){
            vector.m_data = nullptr;
            vector.m_count = 0;
            vector.m_capacity = 0;
        }

        ~Vector() {
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::destroy_at(m_data + i);
            }
            m_allocator.deallocate(m_data, m_capacity); 
        }

        template<typename... Args>
        inline TValue& emplace_back(Args&&... args) {
            if (m_capacity == m_count) {
                grow();
            }
            std::construct_at(m_data + m_count, std::forward<Args>(args)...);
            m_count++;
            return m_data[m_count - 1];
        }

        inline TValue& push_back(const TValue& value) {
            if (m_capacity == m_count) {
                grow();
            }
            std::construct_at(m_data + m_count, std::move(value));
            m_count++;
            return m_data[m_count - 1];
        }

        template<typename... Args>
        inline TValue& emplace_front(Args&&... args) {
            if (m_capacity == m_count) {
                grow_front();
            }
            else {
                if (m_count != 0) {
                    std::construct_at(m_data + m_count, std::move(m_data[m_count - 1]));
                    for (std::uint32_t i = m_count - 1; i != 0; i--) {
                        m_data[i] = std::move(m_data[i - 1]); 
                    }
                }
            }
            std::construct_at(m_data, std::forward<Args>(args)...);
            m_count++;
            return *m_data;
        }

        inline TValue& push_front(const TValue& value) {
            if (m_capacity == m_count) {
                grow_front();
            }
            else {
                if (m_count != 0) {
                    std::construct_at(m_data + m_count, std::move(m_data[m_count - 1]));
                    for (std::uint32_t i = m_count - 1; i != 0; i--) {
                        m_data[i] = std::move(m_data[i - 1]); 
                    }
                }
            }
            std::construct_at(m_data, std::move(value));
            m_count++;
            return *m_data;
        }

        inline void pop_back() {
            assert(m_count != 0);
            m_count--;
            std::destroy_at(m_data + m_count);
        }

        inline void swap_and_pop(const TValue& value) {
            assert(m_count != 0);
            for (std::uint32_t i = 0; i < m_count; i++) {
                if (m_data[i] == value) {
                    m_data[i] = std::move(m_data[m_count - 1]);
                    return pop_back();
                }
            }
        }

        inline void swap_and_pop_at(std::uint32_t index) {
            assert(index < m_count);
            m_data[index] = std::move(m_data[m_count - 1]);
            return pop_back();
        }

        inline void erase(const TValue& value) {
            assert(m_count != 0);
            for (std::uint32_t i = 0; i < m_count; i++) {
                if (m_data[i] == value) {
                    std::destroy_at(m_data + i);
                    shrink(i);
                    m_count--;
                    return;
                }
            }
        }

        inline void erase_at(std::uint32_t index) {
            assert(index < m_count);
            std::destroy_at(m_data + index);
            shrink(index);
            m_count--;
        }

        inline void reserve(std::uint32_t capacity) {
            assert(capacity > m_capacity);
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(capacity);
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::construct_at(m_data + i, std::move(tmp_data[i]));
                std::destroy_at(tmp_data + i);
            }
            m_allocator.deallocate(tmp_data, m_capacity);
            m_capacity = capacity;
        }

        inline void resize(std::uint32_t size, const TValue& v) {
            if (m_capacity < size) {
                reserve(size);
            }
            for (std::uint32_t i = m_count; i < size; i++) {
                std::construct_at(m_data + i, v);
            }
            for (std::uint32_t i = size; i < m_count; i++) {
                std::destroy_at(m_data + i);
            }
            m_count = size;
        }

        inline void resize(std::uint32_t size) {
            if (m_capacity < size) {
                reserve(size);
            }
            for (std::uint32_t i = m_count; i < size; i++) {
                std::construct_at(m_data + i);
            }
            for (std::uint32_t i = size; i < m_count; i++) {
                std::destroy_at(m_data + i);
            }
            m_count = size;
        }

        inline void clear() {
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::destroy_at(m_data + i);
            }
            m_count = 0;
        }

        inline Vector& operator = (Vector&& vector) {
            m_data = vector.m_data;
            m_count = vector.m_count;
            m_capacity = vector.m_capacity;
            m_allocator = vector.m_allocator;

            vector.m_data = nullptr;
            vector.m_count = 0;
            vector.m_capacity = 0;

            return *this;
        }

        inline TValue& operator[] (std::uint32_t index) {
            assert(index >= 0 && index < m_count);
            return m_data[index];   
        }

        inline const TValue& operator[] (std::uint32_t index) const {
            assert(index >= 0 && index < m_count);
            return m_data[index];   
        }

        inline TValue* const get_data() const {
            return m_data;  
        }

        inline std::uint32_t get_count() const {
            return m_count;
        }

        inline std::uint32_t get_capacity() const {
            return m_capacity;
        }

        Iterator begin() {
            return Iterator(m_data);
        }

        Iterator end() {
            return Iterator(m_data + m_count);
        }

        ConstIterator begin() const {
            return ConstIterator(m_data);
        }

        ConstIterator end() const {
            return ConstIterator(m_data + m_count);
        }

        ConstIterator cbegin() const {
            return ConstIterator(m_data);
        }

        ConstIterator cend() const {
            return ConstIterator(m_data);
        }

        ReverseIterator rbegin() {
            return ReverseIterator(m_data + m_count);
        }

        ReverseIterator rend() {
            return ReverseIterator(m_data);
        }

        ConstReverseIterator crbegin() const {
            return ConstReverseIterator(m_data + m_count);
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(m_data);
        }

        Vector (const Vector&) = delete;
        Vector& operator= (const Vector&) = default;

    private:
        inline void grow() {
            std::uint32_t old_capacity = m_capacity;
            m_capacity *= 2;
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::construct_at(m_data + i, std::move(tmp_data[i]));
                std::destroy_at(tmp_data + i);
            }
            m_allocator.deallocate(tmp_data, old_capacity);
        }

        inline void grow_front() {
            std::uint32_t old_capacity = m_capacity;
            m_capacity *= 2;
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::construct_at(m_data + i + 1, std::move(tmp_data[i]));
                std::destroy_at(tmp_data + i);
            }
            m_allocator.deallocate(tmp_data, old_capacity);
        }

        inline void shrink(std::uint32_t index) {
            m_capacity--;
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < index; i++) {
                std::construct_at(m_data + i, std::move(tmp_data[i]));
                std::destroy_at(tmp_data + i);
            }
            for (std::uint32_t i = index + 1; i < m_count; i++) {
                std::construct_at(m_data + i  - 1, std::move(tmp_data[i]));
                std::destroy_at(tmp_data + i);
            }
            m_allocator.deallocate(tmp_data, m_capacity + 1);
        }

    private:
        TValue* m_data;
        std::uint32_t m_count;
        std::uint32_t m_capacity;
        TAllocator m_allocator;
    };
}
