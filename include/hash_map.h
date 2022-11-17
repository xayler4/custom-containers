#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <cstring>
#include <assert.h>

template<typename TValue>
class HashNode {
public:
    HashNode(TValue&& value, unsigned int hash_code) : m_value(std::move(value)), m_hash_code(hash_code) {}

    HashNode(TValue& value, unsigned int hash_code) : m_value(value), m_hash_code(hash_code) {}

    inline operator TValue&() { return m_value; }
    inline unsigned int get_hash_code() { return m_hash_code; }
    inline TValue& get_value() { return m_value; }

private:
    TValue m_value;
    unsigned int m_hash_code;
};

template<typename TKey>
class HashCode {
public:
    static unsigned int hash_code (const TKey& key) {
        return key;
    }
};

class DivisionHashIndex {
public:
    static unsigned int hash_index (unsigned int hash_code, unsigned int size) {
        return hash_code % size;
    }
};

template<typename TValue, typename TKey, float TGrowthFactor = 0.75f, typename THashIndex = DivisionHashIndex, typename TAllocator = std::allocator<HashNode<TValue>>>
class HashMapOpenAddressingLinearProbing {
public:
    HashMapOpenAddressingLinearProbing() : m_count(0), m_size(2){
        m_data = m_allocator.allocate(m_size);
        std::memset(m_data, 0, m_size * sizeof(HashNode<TValue>));
    }

    ~HashMapOpenAddressingLinearProbing() = default;

    template<typename... TArgs>
    HashNode<TValue>& emplace(TKey& key, TArgs&&... args) {
        unsigned int hash_index = THashIndex::hash_index(HashCode<TValue>::hash_code(key), m_size);

        while (m_data[hash_index].get_hash_code()) {
            if (++hash_index == m_size) {
                grow(m_size + (m_size * TGrowthFactor));
                return emplace(key, std::forward<TArgs>(args)...);
            }
        }
        m_data[hash_index] = HashNode<TValue>(T(std::forward<TArgs>(args)...));
        m_count++;

        return m_data[hash_index];
    }

    HashNode<TValue>& insert(const TKey& key, TValue&& value ) {
        unsigned int hash_code = HashCode<TValue>::hash_code(key);
        unsigned int hash_index = THashIndex::hash_index(hash_code, m_size);

        while (m_data[hash_index].get_hash_code()) {
            if (++hash_index == m_size) {
                grow(m_size + (m_size * TGrowthFactor));
                return insert(key, std::forward<TValue>(value));
            }
        }
        m_data[hash_index] = HashNode<TValue>(std::forward<TValue>(value), hash_code);
        m_count++;

        return m_data[hash_index];
    }

    HashNode<TValue>& insert(const TKey& key, TValue& value ) {
        unsigned int hash_code = HashCode<TValue>::hash_code(key);
        unsigned int hash_index = THashIndex::hash_index(hash_code, m_size);

        while (m_data[hash_index].get_hash_code()) {
            if (++hash_index == m_size) {
                grow(m_size + (m_size * TGrowthFactor));
                return insert(key, value);
            }
        }
        m_data[hash_index] = HashNode<TValue>(value, hash_code);
        m_count++;

        return m_data[hash_index];
    }

    HashNode<TValue>& operator [] (const TKey& key) {
        unsigned int hash_code = HashCode<TValue>::hash_code(key);
        unsigned int hash_index = THashIndex::hash_index(hash_code, m_size);
        
        while (m_data[hash_index].get_hash_code() != hash_code) {

            hash_index++;
            assert(hash_index != m_size);
        }

        return m_data[hash_index];
    }

    void remove(const TKey& key) {
        unsigned int hash_code = HashCode<TValue>::hash_code(key);
        unsigned int hash_index = THashIndex::hash_index(hash_code, m_size);

        while (m_data[hash_index].get_hash_code() != hash_code) {
            hash_index++;
            assert(hash_index != m_size);
        }

        m_data[hash_index].~HashNode<TValue>();
        std::memset(m_data + hash_index, 0, sizeof(HashNode<TValue>));
    }


    void print() {
        for (unsigned int i = 0; i < m_size; i++) {
            std::cout << m_data[i] << " ";
        }

        std::cout << "\n";
    }

    HashMapOpenAddressingLinearProbing (const HashMapOpenAddressingLinearProbing&) = delete;
    HashMapOpenAddressingLinearProbing& operator = (const HashMapOpenAddressingLinearProbing&) = delete;

private:
    void grow(unsigned int new_size) {
        unsigned int old_size = m_size;
        m_size = new_size;
        HashNode<TValue>* tmp_data = m_data;
        m_data = m_allocator.allocate(m_size);
        std::memset(m_data, 0, m_size * sizeof(HashNode<TValue>));

        for (unsigned int i = 0; i < old_size; i++) {
            if (tmp_data[i].get_hash_code()) {
                unsigned int hash_index = THashIndex::hash_index(tmp_data[i].get_hash_code(), m_size);
                while (m_data[hash_index].get_hash_code()) {
                    if (++hash_index == m_size) {
                        grow(m_size + (m_size * TGrowthFactor));
                        m_allocator.deallocate(tmp_data, old_size * sizeof(HashNode<TValue>));
                        return;
                    }
                }
                m_data[hash_index] = std::move(tmp_data[i]);
            }
        }
        m_allocator.deallocate(tmp_data, old_size * sizeof(HashNode<TValue>));
    }

private:
    TAllocator m_allocator;
    HashNode<TValue>* m_data;
    unsigned int m_count;
    unsigned int m_size;

};

