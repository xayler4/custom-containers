#pragma once

#include <memory>
#include <cstdint>
#include <utility>
#include <cstring>
#include <assert.h>
#include "algorithm.h"

namespace ccnt {
    template<typename TKey, typename TValue> 
    class HashNode {
    public:
        template<typename... TArgs>
        HashNode(std::uint32_t hash_code, const TKey& key, TArgs&&... args) : m_hash_code(hash_code), m_key(std::move(key)), m_value(std::forward<TArgs>(args)...) {
        }

        HashNode(std::uint32_t hash_code, const TKey& key, const TValue& value) : m_hash_code(hash_code), m_key(std::move(key)), m_value(std::move(value)) {
        }

        HashNode(HashNode<TKey, TValue>&& hash_node) : m_hash_code(hash_node.m_hash_code), m_key(std::move(hash_node.m_key)), m_value(std::move(hash_node.m_value)) {
        }

        ~HashNode() {
            m_hash_code = 0;
        }

        inline HashNode<TKey, TValue>& operator = (HashNode<TKey, TValue>&& hash_node) {
            m_value = std::move(hash_node.m_value);

            return *this;
        }

        inline HashNode<TKey, TValue>& operator = (TValue&& value) {
            m_value = std::move(value);

            return *this;
        }

        inline void set_hash_code(std::uint32_t hash_code) {
            m_hash_code = hash_code;
        }

        inline operator TValue&() { 
            return m_value; 
        }

        inline operator const TValue&() const { 
            return m_value; 
        }

        inline std::uint32_t get_hash_code() const { 
            return m_hash_code; 
        }

        inline const TKey& get_key() const { 
            return m_key; 
        }

        inline TValue& get_value() { 
            return m_value; 
        }

        inline const TValue& get_value() const { 
            return m_value; 
        }

    private:
        TValue m_value;
        TKey m_key;
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

    template<typename TKey, typename TValue, typename THashIndex = DivisionHashIndex, typename TAllocator = std::allocator<HashNode<TKey, TValue>>>
    class HashMap {
    public:
        using Value = TValue;
        using Key   = TKey;

        class Iterator {
        public:
            using Type = SparseIterator;
            using ValueType = HashNode<TKey, TValue>;
            using Pointer   = HashNode<TKey, TValue>*;
            using Reference = HashNode<TKey, TValue>&;

        public:
            Iterator(Pointer data, const Pointer& head) : m_data(data), m_head(head) {
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
                do {
                    m_data++;
                    if (m_data == m_head) {
                        break;
                    }
                } while (!(m_data->get_hash_code()));
            }

            bool operator == (const Iterator& it) {
                if (m_data == it.m_data) {
                    return true;
                }
                while (!(m_data->get_hash_code())) {
                    m_data++;
                    if (m_data == it.m_data) {
                        return true;
                    }
                }
                return false;
            }

            bool operator != (const Iterator& it) {
                if (m_data == it.m_data) {
                    return false;
                }
                while (!(m_data->get_hash_code())) {
                    m_data++;
                    if (m_data == it.m_data) {
                        return false;
                    }
                }
                return true;
            }

        public:
            Pointer m_data;
            const Pointer& m_head;
        };

        class ReverseIterator {
        public:
            using Type = SparseIterator;
            using ValueType = HashNode<TKey, TValue>;
            using Pointer   = HashNode<TKey, TValue>*;
            using Reference = HashNode<TKey, TValue>&;

        public:
            ReverseIterator(Pointer data, const Pointer& head) : m_data(data), m_head(head) {
            };
            ~ReverseIterator() = default;

            Reference operator * () {
                while (!(m_data->get_hash_code())) {
                    m_data--;
                }
                return *m_data;
            }

            Pointer operator -> () {
                while (!(m_data->get_hash_code())) {
                    m_data--;
                }
                return m_data; 
            }

            void operator ++ () {
                while (!(m_data->get_hash_code())) {
                    m_data--;
                }
                do {
                    m_data--;
                    if (m_data == m_head) {
                        break;
                    }
                } while (!(m_data->get_hash_code()));
            }

            bool operator == (const ReverseIterator& it) {
                if (m_data == it.m_data) {
                    return true;
                }
                while (!(m_data->get_hash_code())) {
                    m_data--;
                    if (m_data == it.m_data) {
                        return true;
                    }
                }
                return false;
            }

            bool operator != (const ReverseIterator& it) {
                if (m_data == it.m_data) {
                    return false;
                }
                while (!(m_data->get_hash_code())) {
                    m_data--;
                    if (m_data == it.m_data) {
                        return false;
                    }
                }
                return true;
            }

        private:
            Pointer m_data;
            const Pointer& m_head;
        };

        class ConstIterator {
        public:
            using Type = SparseIterator;
            using ValueType = const HashNode<TKey, TValue>;
            using Pointer   = const HashNode<TKey, TValue>*;
            using Reference = const HashNode<TKey, TValue>&;

        public:
            ConstIterator(HashNode<TKey, TValue>* data, const Pointer& head) : m_data(data), m_head(head) {
            };
            ~ConstIterator() = default;

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
                do {
                    m_data++;
                    if (m_data == m_head) {
                        break;
                    }
                } while (!(m_data->get_hash_code()));
            }

            bool operator == (const ConstIterator& it) {
                if (m_data == it.m_data) {
                    return true;
                }
                while (!(m_data->get_hash_code())) {
                    m_data++;
                    if (m_data == it.m_data) {
                        return true;
                    }
                }
                return false;
            }

            bool operator != (const ConstIterator& it) {
                if (m_data == it.m_data) {
                    return false;
                }
                while (!(m_data->get_hash_code())) {
                    m_data++;
                    if (m_data == it.m_data) {
                        return false;
                    }
                }
                return true;
            }

        protected:
            HashNode<TKey, TValue>* m_data;
            Pointer& m_head;
        };


        class ConstReverseIterator {
        public:
            using Type = SparseIterator;
            using ValueType = const HashNode<TKey, TValue>;
            using Pointer   = const HashNode<TKey, TValue>*;
            using Reference = const HashNode<TKey, TValue>&;

        public:
            ConstReverseIterator(HashNode<TKey, TValue>* data, const Pointer& head) : m_data(data), m_head(head) {
            };
            ~ConstReverseIterator() = default;

            Reference operator * () {
                while (!(m_data->get_hash_code())) {
                    m_data--;
                }
                return *m_data;
            }

            Pointer operator -> () {
                while (!(m_data->get_hash_code())) {
                    m_data--;
                }
                return m_data; 
            }

            void operator ++ () {
                while (!(m_data->get_hash_code())) {
                    m_data--;
                }
                do {
                    m_data--;
                    if (m_data == m_head) {
                        break;
                    }
                } while (!(m_data->get_hash_code()));
            }

            bool operator == (const ConstReverseIterator& it) {
                if (m_data == it.m_data) {
                    return true;
                }
                while (!(m_data->get_hash_code())) {
                    m_data--;
                    if (m_data == it.m_data) {
                        return true;
                    }
                }
                return false;
            }

            bool operator != (const ConstReverseIterator& it) {
                if (m_data == it.m_data) {
                    return false;
                }
                while (!(m_data->get_hash_code())) {
                    m_data--;
                    if (m_data == it.m_data) {
                        return false;
                    }
                }
                return true;
            }

        private:
            HashNode<TKey, TValue>* m_data;
            Pointer& m_head;
        };

    public:
        HashMap() : m_count(0), m_capacity(16) {
            m_data = m_allocator.allocate(m_capacity);
            m_head = m_data + m_capacity;
            clear_hash_codes();
        }

        HashMap(HashMap&& hash_map) : m_data(hash_map.m_data), m_head(hash_map.m_head), m_count(hash_map.m_count), m_allocator(hash_map.m_allocator){
            hash_map.m_data = nullptr;
            hash_map.m_head = nullptr;
            hash_map.m_count = 0;
            hash_map.m_capacity = 0;
        }

        ~HashMap() {
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
            m_allocator.deallocate(m_data, m_capacity);
        }

        template<typename... TArgs>
        inline HashNode<TKey, TValue>& emplace(const TKey& key, TArgs&&... args) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            assert(hash_code != 0);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * 2));
                    return emplace(key, std::forward<TArgs>(args)...);
                }
            }

            std::construct_at(m_data + hash_index, hash_code, key, std::forward<TArgs>(args)...);
            m_count++;

            return m_data[hash_index];
        }

        inline HashNode<TKey, TValue>& insert(const TKey& key, const TValue& value ) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            assert(hash_code != 0);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            while (m_data[hash_index].get_hash_code()) {
                if (++hash_index == m_capacity) {
                    grow(m_capacity + (m_capacity * 2));
                    return insert(key, value);
                }
            }

            std::construct_at(m_data + hash_index, hash_code, key, std::move(value));
            m_count++;

            return m_data[hash_index];
        }

        inline HashMap& operator = (HashMap&& hash_map) {
            hash_map.m_data = hash_map.m_data;
            hash_map.m_head = hash_map.m_head;
            hash_map.m_count = hash_map.m_count;
            hash_map.m_capacity = hash_map.m_capacity;

            hash_map.m_data = nullptr;
            hash_map.m_head = nullptr;
            hash_map.m_count = 0;
            hash_map.m_capacity = 0;
        }

        inline HashNode<TKey, TValue>& operator [] (const TKey& key) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            assert(hash_code != 0);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);
            
            do {
                assert(hash_index != m_capacity);
                while (m_data[hash_index].get_hash_code() != hash_code) {

                    hash_index++;
                    assert(hash_index != m_capacity);
                }
            } while (m_data[hash_index++].get_key() != key);

            return m_data[hash_index - 1];
        }

        inline const HashNode<TKey, TValue>& operator [] (const TKey& key) const {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            assert(hash_code != 0);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);
            
            do {
                assert(hash_index != m_capacity);
                while (m_data[hash_index].get_hash_code() != hash_code) {

                    hash_index++;
                    assert(hash_index != m_capacity);
                }
            } while (m_data[hash_index++].get_key() != key);

            return m_data[hash_index - 1];
        }

        inline void remove(const TKey& key) {
            std::uint32_t hash_code = HashCode<TKey>::hash_code(key);
            assert(hash_code != 0);
            std::uint32_t hash_index = THashIndex::hash_index(hash_code, m_capacity);

            do {
                assert(hash_index != m_capacity);
                while (m_data[hash_index].get_hash_code() != hash_code) {

                    hash_index++;
                    assert(hash_index != m_capacity);
                }
            } while (m_data[hash_index++].get_key() != key);

            std::destroy_at(m_data + hash_index - 1);
        }

        inline void clear() {
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                std::uint32_t destroyed = 0;
                if (m_data[i].get_hash_code()) {
                    std::destroy_at(&m_data[i].get_value());
                    destroyed++;
                }
                if (destroyed == m_count) {
                    clear_hash_codes();
                    m_count = 0;
                    return;
                }
            }
        }

        Iterator begin() {
            if (m_count == 0) {
                return Iterator(m_head, m_head);
            }
            auto* data = m_data;
            while (!data->get_hash_code()) {
                data++;
            }
            return Iterator(data, m_head);
        } 

        Iterator end() {
            return Iterator(m_data + m_capacity, m_head);
        }

        ConstIterator begin() const {
            if (m_count == 0) {
                return Iterator(m_head, m_head);
            }
            auto* data = m_data;
            while (!data->get_hash_code()) {
                data++;
            }
            return ConstIterator(data, m_head);
        } 

        ConstIterator end() const {
            return ConstIterator(m_data + m_capacity, m_head);
        }

        ConstIterator cbegin() const {
            auto* data = m_data;
            while (!data->get_hash_code()) {
                data++;
            }
            return ConstIterator(data, m_head);
        } 

        ConstIterator cend() const {
            return ConstIterator(m_data + m_capacity, m_head);
        }

        ReverseIterator rbegin() {
            if (m_count == 0) {
                return Iterator(m_head, m_data);
            }
            auto* data = m_head;
            while (!data->get_hash_code()) {
                data--;
            }
            return ReverseIterator(data, m_data);
        } 

        ReverseIterator rend() {
            return ReverseIterator(m_data, m_data);
        }

        ConstReverseIterator crbegin() const {
            if (m_count == 0) {
                return Iterator(m_head, m_data);
            }
            auto* data = m_head;
            while (!data->get_hash_code()) {
                data--;
            }
            return ConstReverseIterator(data, m_data);
        } 

        ConstReverseIterator crend() const {
            return ConstReverseIterator(m_data, m_data);
        }

        HashMap (const HashMap&) = delete;
        HashMap& operator= (const HashMap&) = default;

    private:
        inline void grow(std::uint32_t new_capacity) {
            std::uint32_t old_capacity = m_capacity;
            m_capacity = new_capacity;
            HashNode<TKey, TValue>* tmp_data = m_data;
            m_data = m_allocator.allocate(m_capacity);
            m_head = m_data + m_capacity;
            clear_hash_codes();

            for (std::uint32_t i = 0; i < old_capacity; i++) {
                if (tmp_data[i].get_hash_code()) {
                    std::uint32_t hash_index = THashIndex::hash_index(tmp_data[i].get_hash_code(), m_capacity);
                    while (m_data[hash_index].get_hash_code()) {
                        if (++hash_index == m_capacity) {
                            grow(m_capacity * 2);
                            m_allocator.deallocate(tmp_data, old_capacity);
                            return;
                        }
                    }
                    std::construct_at(m_data + hash_index, std::move(tmp_data[i]));
                    std::destroy_at(tmp_data + i);
                }
            }
            m_allocator.deallocate(tmp_data, old_capacity);
        }

        inline void clear_hash_codes() {
            for (std::uint32_t i = 0; i < m_capacity; i++) {
                m_data[i].set_hash_code(0);
            } 
        }

    private:
        HashNode<TKey, TValue>* m_data;
        HashNode<TKey, TValue>* m_head;
        std::uint32_t m_count;
        std::uint32_t m_capacity;
        TAllocator m_allocator;
    };
}
