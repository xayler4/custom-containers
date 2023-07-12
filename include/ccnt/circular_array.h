#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>

namespace ccnt  {
    template<typename TValue, std::uint32_t TCapacity, typename TAllocator = std::allocator<TValue>>
    class CircularArray {
    public:
        class Iterator {
        public:
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

            void operator ++ () {
                m_data++;
            }

            bool operator == (const Iterator& it) {
                return m_data == it.m_data;
            }

            bool operator != (const Iterator& it) {
                return m_data != it.m_data;
            }

        private:
            Pointer m_data;
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
        void emplace(TArgs&&... args) {
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

        void push(const TValue& value) {
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

        void push(TValue&& value) {
            if (m_current == m_data + TCapacity - 1) {
                m_current = m_data;
            }
            else {
                ++m_current;
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

    private:
        TAllocator m_allocator;
        TValue* m_data;
        TValue* m_current;
        std::uint32_t m_count;
    };
}
