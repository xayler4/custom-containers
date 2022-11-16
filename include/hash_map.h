#pragma once

template<typename TValue>
class HashNode {
    HashNode(TValue&& value, const unsigned int hash_code) : m_value(value), m_hash_code(hash_code) {}

    inline operator TValue&() { return m_value; }

private:
    TValue m_value;
    const unsigned int m_hash_code;
};

template<typename TKey>
class HashCode {
    static unsigned int hash_code (const TKey&& key) {
        return key;
    }
};

class DivisionHashIndex {
    static unsigned int hash_index (unsigned int hash_code, unsigned int size) {
        return hash_code % size;
    }
};

class OpenAddressing {
public:
    template<typename TValue, typename TKey>
    class LinearProbing {
    public:
        using T = TValue;
        using K = TKey;

    public:
        LinearProbing() = default;
        ~LinearProbing() = default;

        template<typename... TArgs>
        HashNode<TValue> emplace(unsigned int hash_index, TArgs&&... args) {
            
        }

        HashNode<TValue>& insert(unsigned int hash_index, TValue&& value) {
            
        }

        HashNode<TValue>& search(unsigned int hash_index) {

        }

        void remove(unsigned int hash_index, unsigned int hash_code) {

        }

        LinearProbing (const LinearProbing&) = delete;
        LinearProbing& operator = (const LinearProbing&) = delete;

    };

public:
    OpenAddressing() = delete;
    OpenAddressing(const OpenAddressing&) = delete;
    OpenAddressing& operator = (const OpenAddressing&) = delete;
};

template<typename TCollisionHandler, typename THashIndex = DivisionHashIndex>
class HashMap {
public:
    HashMap() = default;
    ~HashMap() = default;
    
    template<typename... TArgs>
    inline HashNode& emplace(typename TCollisionHandler::K&& key, TArgs&&... args) {
        return m_collision_handler.emplace(key, std::forward<TArgs>(args)...);
    }

    inline HashNode& insert(typename TCollisionHandler::K&& key, typename TCollisionHandler::T&& value ) {
        return m_collision_handler.insert(THashIndex::hash_index(HashCode<T>::hash_code(key)), value);
    }

    inline HashNode& operator[] (typename TCollisionHandler::K&& key) { 
        return m_collision_handler.search(THashIndex::hash_index(HashCode<T>::hash_code(key))); 
    }

private:
    TCollisionHandler m_collision_handler;
};
