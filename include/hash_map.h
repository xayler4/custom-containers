#pragma once

#include <memory>
#include <cstdint>
#include <utility>
#include <cstring>
#include <assert.h>

namespace ccnt {
    template<typename TValue> 
    class HashNode {
    public:
        HashNode(TValue&& value, std::uint32_t hash_code) : m_value(std::forward<TValue>(value)), m_hash_code(hash_code) {}

        HashNode(TValue& value, std::uint32_t hash_code) : m_value(value), m_hash_code(hash_code) {}

        inline operator TValue&() { return m_value; }
        inline std::uint32_t get_hash_code() { return m_hash_code; }
        inline TValue& get_value() { return m_value; }

    private:
        TValue m_value;
        std::uint32_t m_hash_code;
    };

    template<typename TKey>
    class HashCode {
    public:
        static std::uint32_t hash_code (const TKey& key) {
            return key;
        }
    };

    class DivisionHashIndex {
    public:
        static std::uint32_t hash_index (std::uint32_t hash_code, std::uint32_t capacity) {
            return hash_code % capacity;
        }
    };

    template<typename TValue, typename TKey, float TGrowthFactor = 0.75f, typename THashIndex = DivisionHashIndex, typename TAllocator = std::allocator<HashNode<TValue>>>
    class HashMapOpenAddressingLinearProbing {
    public:
        using Value = TValue;
        using Key   = TKey;

        class Iterator {
        public:
            using ValueType = HashNode<TValue>;
            using Pointer   = HashNode<TValue>*;
            using Reference = HashNode<TValue>&;

        public:
            Iterator(Pointer data) : m_data(data) {
            };
            ~Iterator() = default;

            Reference operator * () {
                while (!(m_data->get_hash_code())) {
                    m_data++;
                }
                return *m_data;
            }

            Pointer operator -> () {
                while (!(m_data->get_hash_code())) {
                    m_data++;
                }
                return m_data; 
            }

            Reference operator ++ () {
                while (!(m_data->get_hash_code())) {
                    m_data++;
                }

                return *m_data;
            }

            bool operator == (const Iterator& it) {
                while (m_data != it.m_data) {
                    m_data++;
                    if (m_data->get_hash_code()) {
                        return false;
                    }
                }

                return true;
            }

            bool operator != (const Iterator& it) {
                while (m_data != it.m_data) {
                    m_data++;
                    if (m_data->get_hash_code()) {
                        return true;
                    }
                }
                return false;
            }

        private:
            Pointer m_data;
        };

    public:
        HashMapOpenAddressingLinearProbing() : m_count(0), m_capacity(16) {
            m_data = m_allocator.allocate(m_capacity);
            std::memset(m_data, 0, m_capacity * sizeof(HashNode<TValue>));
        }

        ~HashMapOpenAddressingLinearProbing() {
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::uint32_t destroyed = 0;
                if (m_data[i].get_hash_code()) {
                    m_data[i].get_hash_code().~TValue();
                    destroyed++;
                }
                if (destroyed == m_count) {
                    m_allocator.deallocate(m_data, m_capacity * sizeof(HashNode<TValue>));
                    return;
                }
            }
        }

        template<typename... TArgs>
        HashNode<TValue>& emplace(const TKey& key, TArgs&&... args) {
            std::uint32_t hash_index = THashIndex::hash_index(HashCode<TValue>::hash_code(key), m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * TGrowthFactor));
                    return emplace(key, std::forward<TArgs>(args)...);
                }
            }
            m_data[hash_index] = HashNode<TValue>(T(std::forward<TArgs>(args)...));
            m_count++;

            return m_data[hash_index];
        }

        HashNode<TValue>& insert(const TKey& key, TValue&& value ) {
            std::uint32_t hash_code = HashCode<TValue>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * TGrowthFactor));
                    return insert(key, std::forward<TValue>(value));
                }
            }

            m_data[hash_index] = HashNode<TValue>(std::forward<TValue>(value), hash_code);
            m_count++;

            return m_data[hash_index];
        }

        HashNode<TValue>& insert(const TKey& key, TValue& value ) {
            std::uint32_t hash_code = HashCode<TValue>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * TGrowthFactor));
                    return insert(key, value);
                }
            }

            m_data[hash_index] = HashNode<TValue>(value, hash_code);
            m_count++;

            return m_data[hash_index];
        }

        HashNode<TValue>& operator [] (const TKey& key) {
            std::uint32_t hash_code = HashCode<TValue>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);
            
            while (m_data[hash_index].get_hash_code() != hash_code) {

                hash_index++;
                assert(hash_index != m_capacity);
            }

            return m_data[hash_index];
        }

        void remove(const TKey& key) {
            std::uint32_t hash_code = HashCode<TValue>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code() != hash_code) {
                hash_index++;
                assert(hash_index != m_capacity);
            }

            m_data[hash_index].~HashNode<TValue>();
            std::memset(m_data + hash_index, 0, sizeof(HashNode<TValue>));
        }

        Iterator begin() {
            return Iterator(m_data);
        } 

        Iterator end() {
            return Iterator(m_data + m_capacity);
        }

        HashMapOpenAddressingLinearProbing (const HashMapOpenAddressingLinearProbing&) = delete;
        HashMapOpenAddressingLinearProbing& operator = (const HashMapOpenAddressingLinearProbing&) = delete;

    private:
        void grow(std::uint32_t new_capacity) {
            std::uint32_t old_capacity = m_capacity;
            m_capacity = new_capacity;
            HashNode<TValue>* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            std::memset(m_data, 0, m_capacity * sizeof(HashNode<TValue>));

            for (std::uint32_t i = 0; i < old_capacity; i++) {
                if (tmp_data[i].get_hash_code()) {
                    std::uint32_t hash_index = THashIndex::hash_index(tmp_data[i].get_hash_code(), m_capacity);
                    while (m_data[hash_index].get_hash_code()) {
                        if (++hash_index == m_capacity) {
                            grow(m_capacity + (m_capacity * TGrowthFactor));
                            m_allocator.deallocate(tmp_data, old_capacity * sizeof(HashNode<TValue>));
                            return;
                        }
                    }
                    m_data[hash_index] = std::move(tmp_data[i]);
                }
            }
            m_allocator.deallocate(tmp_data, old_capacity * sizeof(HashNode<TValue>));
        }

    private:
        TAllocator m_allocator;
        HashNode<TValue>* m_data;
        std::uint32_t m_count;
        std::uint32_t m_capacity;

    };

}
