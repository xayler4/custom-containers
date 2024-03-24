#pragma once

#include <cstdint>
#include <utility>
#include <memory>
#include <array>
#include <assert.h>
#include <bit>
#include "algorithm.h"

namespace ccnt {
    template<std::uint32_t TBits>
    class Bitset {
    public:
        static constexpr std::uint32_t size() {
            float size = static_cast<float>(TBits) / 64;
            return size + 1;
        } 

        using uint_t = std::conditional_t<(TBits <= 8), std::uint8_t,
                std::conditional_t<(TBits <= 16), std::uint16_t,
                std::conditional_t<(TBits <= 32), std::uint32_t,
                std::conditional_t<(TBits <= 64), std::uint64_t,
                std::array<uint64_t, size()>>>>>;

    public:
        class Proxy {
        public:
            inline Proxy(uint_t* bitset, std::uint32_t nbit) : m_bitset(bitset), m_nbit(nbit) {}
            inline Proxy& operator = (const Proxy& proxy) {
                if (proxy) {
                    reinterpret_cast<Bitset<TBits>*>(m_bitset)->set_bit(m_nbit); 
                }
                else {
                    reinterpret_cast<Bitset<TBits>*>(m_bitset)->unset_bit(m_nbit); 
                }

                return *this;
            }

            inline Proxy& operator = (bool value) {
                if (value) {
                    reinterpret_cast<Bitset<TBits>*>(m_bitset)->set_bit(m_nbit); 
                }
                else {
                    reinterpret_cast<Bitset<TBits>*>(m_bitset)->unset_bit(m_nbit); 
                }

                return *this;
            }

            inline bool operator < (const Proxy& proxy) {
                return static_cast<bool>(*this) < static_cast<bool>(proxy);
            }

            template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
            inline operator bool() const {
                assert(m_nbit < TBits);
                return (*m_bitset) & (static_cast<uint_t>(1)) << ((TBits - 1) - m_nbit);
            }

            template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
            inline operator bool() const {
                assert(m_nbit < TBits);
                std::uint32_t index = m_nbit/64;
                return ((*m_bitset)[index]) & (static_cast<std::uint64_t>(1)) << ((index + 1 == size()) ? (TBits - 1 - m_nbit - index * 64) : (63 - m_nbit));
            }

            inline operator std::uint32_t() const {
                assert(m_nbit < TBits);
                return (static_cast<bool>(*this));
            }

        private:
            uint_t* m_bitset;
            std::uint32_t m_nbit;
        };

    public:
        class Iterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = Proxy;
            using Reference = Proxy;

        public:
            Iterator(Bitset<TBits>* bitset, std::uint32_t nbit) : m_bitset(bitset), m_nbit(nbit) {}
            ~Iterator() = default;

            Reference operator * () {
                return (*m_bitset)[m_nbit];
            }

            Pointer operator -> () {
                return (*m_bitset)[m_nbit];
            }

            Iterator operator + (std::uint32_t index) {
                return Iterator(m_bitset, m_nbit + index);
            }

            Iterator& operator += (std::uint32_t index) {
                m_nbit += index;

                return *this;
            }

            Iterator operator - (std::uint32_t index) {
                return Iterator(m_bitset, m_nbit - index);
            }

            Iterator& operator -= (std::uint32_t index) {
                m_nbit -= index;

                return *this;
            }

            std::int32_t operator - (const Iterator& iterator) {
                return m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                m_nbit++;
            }

            void operator -- () {
                m_nbit--;
            }

            bool operator != (const Iterator& it ) {
                return ((it.m_bitset != m_bitset) || (m_nbit != it.m_nbit));
            }

            bool operator == (const Iterator& it ) {
                return ((it.m_bitset == m_bitset) && (m_nbit == it.m_nbit));
            }

        protected:
            Bitset<TBits>* m_bitset;
            std::uint32_t m_nbit;
        };

        class ReverseIterator : public Iterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = Proxy;
            using Reference = Proxy;

        public:
            ReverseIterator(Bitset<TBits>* bitset, std::uint32_t nbit) : Iterator(bitset, nbit) {}
            ~ReverseIterator() = default;

            Reference operator * () {
                return (*Iterator::m_bitset)[Iterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*Iterator::m_bitset)[Iterator::m_nbit - 1];
            }

            ReverseIterator operator + (std::uint32_t index) {
                return ReverseIterator(Iterator::m_bitset, Iterator::m_nbit - index);
            }

            ReverseIterator& operator += (std::uint32_t index) {
                Iterator::m_nbit -= index;

                return *this;
            }

            ReverseIterator operator - (std::uint32_t index) {
                return ReverseIterator(Iterator::m_bitset, Iterator::m_nbit + index);
            }

            ReverseIterator& operator -= (std::uint32_t index) {
                Iterator::m_nbit += index;

                return *this;
            }

            std::int32_t operator - (const ReverseIterator& iterator) {
                return Iterator::m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                Iterator::m_nbit--;
            }

            void operator -- () {
                Iterator::m_nbit++;
            }

            bool operator != (const ReverseIterator& it ) {
                return ((it.m_bitset != Iterator::m_bitset) || (Iterator::m_nbit != it.m_nbit));
            }

            bool operator == (const ReverseIterator& it ) {
                return ((it.m_bitset == Iterator::m_bitset) && (Iterator::m_nbit == it.m_nbit));
            }
        };

        class ConstIterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;

        public:
            ConstIterator(const Bitset<TBits>* bitset, std::uint32_t nbit) : m_bitset(bitset), m_nbit(nbit) {}
            ~ConstIterator() = default;

            Reference operator * () const {
                return (*m_bitset)[m_nbit];
            }

            Pointer operator -> () const {
                return (*m_bitset)[m_nbit];
            }

            ConstIterator operator + (std::uint32_t index) {
                return ConstIterator(m_bitset, m_nbit + index);
            }

            ConstIterator& operator += (std::uint32_t index) {
                m_nbit += index;

                return *this;
            }

            ConstIterator operator - (std::uint32_t index) {
                return ConstIterator(m_bitset, m_nbit - index);
            }

            ConstIterator& operator -= (std::uint32_t index) {
                m_nbit -= index;

                return *this;
            }

            std::int32_t operator - (const ConstIterator& iterator) {
                return m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                m_nbit++;
            }

            void operator -- () {
                m_nbit--;
            }

            bool operator != (const ConstIterator& it ) const {
                return ((it.m_bitset != m_bitset) || (m_nbit != it.m_nbit));
            }

            bool operator == (const ConstIterator& it ) const {
                return ((it.m_bitset == m_bitset) && (m_nbit == it.m_nbit));
            }

        protected:
            const Bitset<TBits>* m_bitset;
            std::uint32_t m_nbit;
        };

        class ConstReverseIterator : public ConstIterator {
        public:
            using Type = RandomAccessIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;

        public:
            ConstReverseIterator(const Bitset<TBits>* bitset, std::uint32_t nbit) : ConstIterator(bitset, nbit) {}
            ~ConstReverseIterator() = default;

            Reference operator * () {
                return (*ConstIterator::m_bitset)[ConstIterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*ConstIterator::m_bitset)[ConstIterator::m_nbit - 1];
            }

            ConstReverseIterator operator + (std::uint32_t index) {
                return ConstReverseIterator(ConstIterator::m_bitset, ConstIterator::m_nbit - index);
            }

            ConstReverseIterator& operator += (std::uint32_t index) {
                ConstIterator::m_nbit -= index;

                return *this;
            }

            ConstReverseIterator operator - (std::uint32_t index) {
                return ConstReverseIterator(ConstIterator::m_bitset, ConstIterator::m_nbit + index);
            }

            ConstReverseIterator operator -= (std::uint32_t index) {
                ConstIterator::m_nbit += index;
            }

            std::int32_t operator - (const ConstReverseIterator& iterator) {
                return ConstIterator::m_nbit - iterator.m_nbit;
            }

            void operator ++ () {
                ConstIterator::m_nbit--;
            }

            void operator -- () {
                ConstIterator::m_nbit++;
            }
        };

    public:
        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        Bitset(bool value = false) {
            if (!value) {
                m_bitset = 0;
            } 
            else {
                m_bitset = -1;
            }
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        Bitset(bool value = false) {
            if (!value) {
                for (std::uint32_t i = 0; i < size(); i++) {
                    m_bitset[i] = 0;
                }
            } 
            else {
                for (std::uint32_t i = 0; i < size(); i++) {
                    m_bitset[i] = -1;
                }
            }
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        explicit Bitset(uint_t bitset) : m_bitset(bitset) {}

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        explicit Bitset(uint_t& bitset) {
            for (uint32_t i = 0; i < size(); i++) {
                m_bitset[i] = bitset[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void unset_all_bits() {
            m_bitset = 0;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void unset_all_bits() {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitset[i] = 0;
            }
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void set_all_bits() {
            m_bitset = -1;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void set_all_bits() {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitset[i] = -1;
            }
        }

        inline Bitset<TBits>& operator = (const uint_t& bitset) {
            m_bitset = bitset;

            return *this;
        }

        inline Bitset<TBits> operator & (const uint_t& bitset) const {
            return Bitset<TBits>(m_bitset & bitset);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits> operator & (const uint_t& bitset) const {
            uint_t out_bitset;
            for (std::uint32_t i = 0; i < size(); i++) {
                out_bitset[i] = m_bitset[i] & bitset[i]; 
            } 
            return Bitset<TBits>(out_bitset);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (const uint_t& bitset) {
            m_bitset &= bitset;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (const uint_t& bitset) {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitset[i] &= bitset[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits> operator | (const uint_t& bitset) const {
            return Bitset<TBits>(m_bitset | bitset);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits> operator | (const uint_t& bitset) const {
            uint_t out_bitset;
            for (std::uint32_t i = 0; i < size(); i++) {
                out_bitset[i] = m_bitset[i] | bitset[i]; 
            } 
            return Bitset<TBits>(out_bitset);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (const uint_t& bitset) {
            m_bitset |= bitset;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (const uint_t& bitset) {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitset[i] |= bitset[i]; 
            } 
        }

        inline Bitset<TBits> operator ^ (const uint_t& bitset) const {
            return Bitset<TBits>(m_bitset ^ bitset);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits> operator ^ (const uint_t& bitset) const {
            uint_t out_bitset;
            for (std::uint32_t i = 0; i < size(); i++) {
                out_bitset[i] = m_bitset[i] ^ bitset[i]; 
            } 
            return Bitset<TBits>(out_bitset);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator ^= (const uint_t& bitset) {
            m_bitset ^= bitset;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator ^= (const uint_t& bitset) {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitset[i] ^= bitset[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const uint_t& bitset) const {
            return m_bitset == bitset;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const uint_t& bitset) const {
            for (std::uint32_t i = 0; i < size(); i++) {
                if (m_bitset[i] != bitset[i]) {
                    return false;
                }
            } 
            return true;
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const uint_t& bitset) const {
            return m_bitset != bitset;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const uint_t& bitset) const {
            for (std::uint32_t i = 0; i < size(); i++) {
                if (m_bitset[i] != bitset[i]) {
                    return true;
                }
            } 
            return false;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits>& operator = (const TBitset& bitset) {
            m_bitset = bitset.m_bitset;

            return *this;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits> operator & (const TBitset& bitset) const {
            return Bitset<TBits>(*this & bitset.m_bitset);
        }
        
        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (const TBitset& bitset) {
            *this &= bitset.m_bitset;
        }
        
        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits> operator | (const TBitset& bitset) const {
            return Bitset<TBits>(*this | bitset.m_bitset);
        }
        
        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (const TBitset& bitset) {
            *this |= bitset.m_bitset;
        }

        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitset<TBits> operator ^ (const TBitset& bitset) const {
            return Bitset<TBits>(*this ^ bitset.m_bitset);
        }
        
        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator ^= (const TBitset& bitset) {
            *this ^= bitset.m_bitset;
        }
        
        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const TBitset& bitset) const {
            return *this == bitset.m_bitset;
        }
        
        template<typename TBitset, typename std::enable_if<std::is_same<uint_t, typename TBitset::uint_t>::value || !std::is_same<typename TBitset::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const TBitset& bitset) const {
            return *this != bitset.m_bitset;
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return m_bitset & ((static_cast<uint_t>(1)) << ((TBits - 1) - nbit));
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            std::uint32_t index = nbit / 64;
            return m_bitset[index] & ((static_cast<uint_t>(1)) << ((index + 1 == size()) ? (TBits - 1 - nbit - index * 64) : (63 - nbit))); 
        }

        inline Proxy operator [] (std::uint32_t nbit) {
            assert(nbit < TBits);
            return Proxy(&m_bitset, nbit);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void set_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            m_bitset |= (static_cast<uint_t>(1)) << ((TBits - 1) - nbit); 
        }
        
        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void set_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            std::uint32_t index = nbit/64;
            m_bitset[index] |= (static_cast<std::uint64_t>(1)) << ((index + 1 == size()) ? (TBits - 1 - nbit - index * 64) : (63 - nbit)); 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void unset_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            m_bitset &= ~((static_cast<uint_t>(1)) << ((TBits - 1) - nbit));
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void unset_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            std::uint32_t index = nbit/64;
            m_bitset[index] &= ~((static_cast<std::uint64_t>(1)) << ((index + 1 == size()) ? (TBits - 1 - nbit - index * 64) : (63 - nbit)));
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline std::uint32_t pop_count() const {
            return std::popcount(m_bitset);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr> 
        inline std::uint32_t pop_count() const {
            std::uint32_t count = 0;
            for (std::uint32_t i = 0; i < size(); i++) {
                count += std::popcount(m_bitset[i]);
            }

            return count;
        }

        inline const uint_t& get_data() const {
            return m_bitset;
        } 

        Iterator begin() {
            return Iterator(this, 0);
        }

        Iterator end() {
            return Iterator(this, TBits);
        }

        ConstIterator begin() const {
            return ConstIterator(this, 0);
        }

        ConstIterator end() const {
            return ConstIterator(this, TBits);
        }

        ConstIterator cbegin() const {
            return ConstIterator(this, 0);
        }

        ConstIterator cend() const {
            return ConstIterator(this, TBits);
        }

        ReverseIterator rbegin() {
            return ReverseIterator(this, TBits);
        }

        ReverseIterator rend() {
            return ReverseIterator(this, 0);
        }

        ConstReverseIterator crbegin() const {
            return ConstReverseIterator(this, TBits);
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(this, 0);
        }

    private:
        uint_t m_bitset;

        template<std::uint32_t TBits1>
        friend class Bitset;
    };
}
