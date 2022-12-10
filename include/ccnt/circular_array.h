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

        };

        ~CircularArray() = default;
        
        template<typename... TArgs>
        void emplace(TArgs... args) {
            *(m_next) = std::forward<TValue>(TValue(std::forward<TArgs>(args)...));
            if (m_next == m_data + TSize - 1) {
                m_next = m_data;
            }
            else {
                ++m_next;
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

        inline TValue& operator [] (std::uint32_t index) const {
            assert(index >= TSize);
            return m_data[index];
        }

        Iterator begin() {
            return Iterator(m_data);
        }

        Iterator end() {
            return Iterator(m_data + TSize);
        }

    private:
        TValue m_data[TSize];
        TValue* m_next;
    };
}
