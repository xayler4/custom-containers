#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>

namespace ccnt  {
    template<typename TValue, std::uint32_t TSize, typename TAllocator = std::allocator<TValue>>
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
        CircularArray() : m_next(m_data){
            m_data = m_allocator.allocate(TSize);
        };

        ~CircularArray() {
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::destroy_at(m_data[i]);
            }
            m_allocator.deallocate(m_data, TSize);
        }
        
        template<typename... TArgs>
        void emplace(TArgs... args) {
            *(m_next) = std::forward<TValue>(TValue(std::forward<TArgs>(args)...));
            if (m_next == m_data + TSize - 1) {
                m_next = m_data;
            }
            else {
                ++m_next;
            }
            if (!(m_count == TSize)) {
                m_count++; 
            }
        }

        void push(const TValue& value) {
            *(m_next) = std::move(value);
            if (m_next == m_data + TSize - 1) {
                m_next = m_data;
            }
            else {
                ++m_next;
            }
            if (!(m_count == TSize)) {
                m_count++; 
            }
        }

        void push(TValue&& value) {
            *(m_next) = std::forward<TValue>(value);
            if (m_next == m_data + TSize - 1) {
                m_next = m_data;
            }
            else {
                ++m_next;
            }
        }

        inline TValue& operator [] (std::uint32_t index) {
            assert(index >= TSize);
            return m_data[index];
        }

        inline std::uint32_t get_size() const {
            return TSize;
        }

        inline std::uint32_t get_count() const {
            return m_count;
        }

        Iterator begin() {
            return Iterator(m_data);
        }

        Iterator end() {
            return Iterator(m_data + TSize);
        }

    private:
        TAllocator m_allocator;
        TValue* m_data;
        TValue* m_next;
        std::uint32_t m_count;
    };
}
