#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>

namespace ccnt {
    template<typename TValue, float TGrowthFactor = 0.75f, typename TAllocator = std::allocator<TValue>>
    class Vector  {
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

        Vector() : m_count(0), m_capacity(2) {
            m_data = m_allocator.allocate(2);
        }

        ~Vector() {
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::destroy_at(m_data + i);
            }
            m_allocator.deallocate(m_data, sizeof(TValue) * m_capacity); 
        }

        template<typename... Args>
        TValue& emplace_back(Args&&... args) {
            if (m_capacity == m_count) {
                grow();
            }
            std::construct_at(m_data + m_count, std::forward<Args>(args)...);
            m_count++;
            return m_data[m_count - 1];
        }

        TValue& push_back(TValue&& value) {
            if (m_capacity == m_count) {
                grow();
            }
            std::construct_at(m_data + m_count, std::forward<TValue>(value));
            m_count++;
            return m_data[m_count - 1];
        }

        TValue& push_back(const TValue& value) {
            if (m_capacity == m_count) {
                grow();
            }
            std::construct_at(m_data + m_count, std::move(value));
            m_count++;
            return m_data[m_count - 1];
        }

        template<typename... Args>
        TValue& emplace_front(Args&&... args) {
            if (m_capacity == m_count) {
                grow_front();
            }
            else {
                if (m_count != 0) {
                    for (std::uint32_t i = m_count; i != 0; i--) {
                        m_data[i] = std::move(m_data[i - 1]); 
                    }
                }
            }
            std::construct_at(m_data, std::forward<Args>(args)...);
            m_count++;
            return *m_data;
        }

        TValue& push_front(TValue&& value) {
            if (m_capacity == m_count) {
                grow_front();
            }
            else {
                if (m_count != 0) {
                    for (std::uint32_t i = m_count; i != 0; i--) {
                        m_data[i] = std::move(m_data[i - 1]); 
                    }
                }
            }
            std::construct_at(m_data, std::forward<TValue>(value));
            m_count++;
            return *m_data;
        }

        TValue& push_front(const TValue& value) {
            if (m_capacity == m_count) {
                grow_front();
            }
            else {
                if (m_count != 0) {
                    for (std::uint32_t i = m_count; i != 0; i--) {
                        m_data[i] = std::move(m_data[i - 1]); 
                    }
                }
            }
            std::construct_at(m_data, std::move(value));
            m_count++;
            return *m_data;
        }

        void pop_back() {
            assert(m_count != 0);
            m_count--;
            std::destroy_at(m_data + m_count);
        }

        void remove(TValue&& value) {
            assert(m_count != 0);
            if (m_count == 1) {
                return pop_back(); 
            }
            for (std::uint32_t i = 0; i < m_count; i++) {
                if (m_data[i] == value) {
                    std::destroy_at(m_data + i);
                    m_data[i] = std::move(m_data[m_count - 1]);
                    return pop_back();
                }
            }
        }

        void remove(const TValue& value) {
            assert(m_count != 0);
            if (m_count == 1) {
                return pop_back(); 
            }
            for (std::uint32_t i = 0; i < m_count; i++) {
                if (m_data[i] == value) {
                    std::destroy_at(m_data + i);
                    m_data[i] = std::move(m_data[m_count - 1]);
                    return pop_back();
                }
            }
        }

        void erase(TValue&& value) {
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

        void erase(const TValue& value) {
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

        void reserve(std::uint32_t capacity) {
            assert(capacity <= m_capacity);
            m_capacity = capacity;
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_count; i++) {
                m_data[i] = std::move(tmp_data[i]);
            }
            m_allocator.deallocate(tmp_data, sizeof(TValue) * m_capacity);
        }

        void clear() {
            for (std::uint32_t i = 0; i < m_count; i++) {
                std::destroy_at(m_data + i);
            }
            m_count = 0;
        }

        inline TValue& operator[] (std::uint32_t index) {
            assert(index >= 0 && index < m_count);
            return m_data[index];   
        }

        inline const TValue* get_data() const {
            return m_data;  
        }

        inline std::uint32_t get_count() const {
            return m_count;
        }

        inline std::uint32_t get_capacity() const {
            return m_capacity;
        }

        Iterator begin() const {
            return Iterator(m_data);
        }

        Iterator end() const {
            return Iterator(m_data + m_count);
        }

        Vector(const Vector<TValue&, TGrowthFactor, TAllocator>& ) = delete;
        Vector operator = (const Vector<TValue&, TGrowthFactor, TAllocator>&) = delete;

    private:
        void grow() {
            m_capacity += m_capacity * TGrowthFactor;
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_count; i++) {
                m_data[i] = std::move(tmp_data[i]);
            }
            m_allocator.deallocate(tmp_data, sizeof(TValue) * m_capacity - (m_capacity * TGrowthFactor));
        }

        void grow_front() {
            m_capacity += m_capacity * TGrowthFactor;
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < m_count; i++) {
                m_data[i + 1] = std::move(tmp_data[i]);
            }
            m_allocator.deallocate(tmp_data, sizeof(TValue) * m_capacity - (m_capacity * TGrowthFactor));
        }

        void shrink(std::uint32_t index) {
            m_capacity--;
            TValue* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            for (std::uint32_t i = 0; i < index; i++) {
                m_data[i] = std::move(tmp_data[i]);
            }
            for (std::uint32_t i = index + 1; i < m_count; i++) {
                m_data[i - 1] = std::move(tmp_data[i]);
            }
            m_allocator.deallocate(tmp_data, sizeof(TValue) * (m_capacity + 1));
        }

        TValue* m_data;
        std::uint32_t m_count;
        std::uint32_t m_capacity;
        TAllocator m_allocator;
    };
}
