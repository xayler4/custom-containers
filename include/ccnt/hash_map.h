#pragma once

#include "doubly_linked_list.h"
#include <memory>
#include <cstdint>
#include <utility>
#include <cstring>
#include <assert.h>

namespace ccnt {
    template<typename TValue> 
    class HashNode {
    public:
        template<typename... TArgs>
        HashNode(std::nullptr_t dummy, std::uint32_t hash_code, TArgs... args) : m_hash_code(hash_code), m_value(std::forward<TArgs>(args)...) {}

        HashNode(std::uint32_t hash_code, TValue&& value) : m_value(std::forward<TValue>(value)), m_hash_code(hash_code) {}

        HashNode(std::uint32_t hash_code, TValue& value) : m_hash_code(hash_code), m_value(value) {}

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
    class HashMapOpenAddressing {
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

            void operator ++ () {
                while (!(m_data->get_hash_code())) {
                    m_data++;
                }
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
        HashMapOpenAddressing() : m_count(0), m_capacity(16) {
            m_data = m_allocator.allocate(m_capacity);
            std::memset(m_data, 0, m_capacity * sizeof(HashNode<TValue>));
        }

        ~HashMapOpenAddressing() {
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::uint32_t destroyed = 0;
                if (m_data[i].get_hash_code()) {
                    std::destroy_at(&m_data[i].get_value());
                    destroyed++;
                }
                if (destroyed == m_count) {
                    m_allocator.deallocate(m_data, m_capacity);
                    return;
                }
            }
        }

        template<typename... TArgs>
        HashNode<TValue>& emplace(const TKey& key, TArgs&&... args) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * TGrowthFactor));
                    return emplace(key, std::forward<TArgs>(args)...);
                }
            }

            m_data[hash_index] = HashNode<TValue>(hash_code, std::forward<TArgs>(args)...);
            m_count++;

            return m_data[hash_index];
        }

        HashNode<TValue>& insert(const TKey& key, TValue&& value ) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * TGrowthFactor));
                    return insert(key, std::forward<TValue>(value));
                }
            }

            m_data[hash_index] = HashNode<TValue>(hash_code, std::forward<TValue>(value));
            m_count++;

            return m_data[hash_index];
        }

        HashNode<TValue>& insert(const TKey& key, TValue& value ) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * TGrowthFactor));
                    return insert(key, value);
                }
            }

            m_data[hash_index] = HashNode<TValue>(hash_code, value);
            m_count++;

            return m_data[hash_index];
        }

        HashNode<TValue>& operator [] (const TKey& key) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);
            
            while (m_data[hash_index].get_hash_code() != hash_code) {

                hash_index++;
                assert(hash_index != m_capacity);
            }

            return m_data[hash_index];
        }

        void remove(const TKey& key) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code() != hash_code) {
                hash_index++;
                assert(hash_index != m_capacity);
            }

            std::destroy_at(m_data + hash_index);
            std::memset(m_data + hash_index, 0, sizeof(HashNode<TValue>));
        }

        Iterator begin() {
            return Iterator(m_data);
        } 

        Iterator end() {
            return Iterator(m_data + m_capacity);
        }

        HashMapOpenAddressing (const HashMapOpenAddressing&) = delete;
        HashMapOpenAddressing& operator= (const HashMapOpenAddressing&) = default;

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
                            m_allocator.deallocate(tmp_data, old_capacity);
                            return;
                        }
                    }
                    m_data[hash_index] = std::move(tmp_data[i]);
                }
            }
            m_allocator.deallocate(tmp_data, old_capacity);
        }

    private:
        TAllocator m_allocator;
        HashNode<TValue>* m_data;
        std::uint32_t m_count;
        std::uint32_t m_capacity;

    };

    template<typename TValue, typename TKey, std::uint32_t TSize, typename THashIndex = DivisionHashIndex, typename TAllocator1 = std::allocator<HashNode<TValue>>, typename TAllocator2 = std::allocator<Node<HashNode<TValue>>>>
    class HashMapSeparateChaining {
    public:
        using Value = TValue;
        using Key   = TKey;

        class Iterator {
        public:
            using ValueType = HashNode<TValue>;
            using Pointer   = HashNode<TValue>*;
            using Reference = HashNode<TValue>&;

        public:
            Iterator(DoublyLinkedList<ValueType, TAllocator2>* data, DoublyLinkedList<ValueType, TAllocator2>::Iterator it) : m_data(data), m_it(it) {
                
            };
            ~Iterator() = default;

            Reference operator * () {
                return *m_it;
            }

            Pointer operator -> () {
                return m_it;
            }

            void operator ++ () {
                if (m_it == m_data->end()) {
                    ++m_data;
                    m_it = typename DoublyLinkedList<ValueType, TAllocator2>::Iterator(m_data->begin());
                }
                else {
                    ++m_it;
                }
            }

            bool operator == (const Iterator& it) {
                while (!m_data->get_length()) {
                    m_it = typename DoublyLinkedList<ValueType, TAllocator2>::Iterator(m_data->begin());
                    if (m_it == it.m_it && m_data == it.m_data) {
                        return true;
                    }
                    ++m_data;
                }

                return ((m_it == it.m_it) && (m_data == it.m_data));
            }

            bool operator != (const Iterator& it) {
                while (m_it == m_data->end()) {
                    if (m_it == it.m_it && m_data == it.m_data) {
                        return false;
                    }

                    ++m_data;
                    m_it = typename DoublyLinkedList<ValueType, TAllocator2>::Iterator(m_data->begin());
                }

                return ((m_it != it.m_it) || (m_data != it.m_data));
            }

        private:
            DoublyLinkedList<ValueType, TAllocator2>* m_data;
            DoublyLinkedList<ValueType, TAllocator2>::Iterator m_it;
        };

    public:
        HashMapSeparateChaining() : m_count(0) {
        }

        ~HashMapSeparateChaining() = default;

        template<typename... TArgs>
        HashNode<TValue>& emplace(const TKey& key, TArgs&&... args) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(HashCode<TKey>::hash_code(key), TSize);
            HashNode<TValue>& node = m_data[hash_index].emplace_at_tail(hash_code, std::forward<TArgs>(args)...);
            m_count++;
            return node;
        }

        HashNode<TValue>& insert(const TKey& key, TValue&& value ) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(HashCode<TKey>::hash_code(key), TSize);
            HashNode<TValue>& node = m_data[hash_index].emplace_at_tail(hash_code, std::forward<TValue>(value));
            m_count++;
            return node;
        }

        HashNode<TValue>& insert(const TKey& key, TValue& value ) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(HashCode<TKey>::hash_code(key), TSize);
            HashNode<TValue>& node = m_data[hash_index].emplace_at_tail(hash_code, value);
            m_count++;
            return node;
        }

        HashNode<TValue>& operator [] (const TKey& key) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(HashCode<TKey>::hash_code(key), TSize);
            for (HashNode<TValue>& node : m_data[hash_index]) {
                if (node.get_hash_code() == hash_code) {
                    return node;
                }
            }
            assert(0);
        }

        void remove(const TKey& key) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            std::uint32_t hash_index = THashIndex::hash_index(HashCode<TKey>::hash_code(key), TSize);
            for (Node<HashNode<TValue>>& node : m_data[hash_index]) {
                if (node.get_hash_code() == hash_code) {
                    m_data[hash_index].erase(node);
                } 
            }
        }

        inline std::uint32_t get_size() {
            return TSize;
        }

        Iterator begin() {
            return Iterator(m_data, m_data[0].begin());
        } 

        Iterator end() {
            return Iterator(m_data + TSize - 1, m_data[TSize - 1].end());
        }

        HashMapSeparateChaining (const HashMapSeparateChaining&) = delete;
        HashMapSeparateChaining& operator= (const HashMapSeparateChaining&) = default;

    private:
        TAllocator1 m_allocator;
        DoublyLinkedList<HashNode<TValue>, TAllocator2> m_data[TSize];
        std::uint32_t m_count;
    };
}
