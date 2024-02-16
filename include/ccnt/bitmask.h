#pragma once

#include <cmath>
#include <cstdint>
#include <utility>
#include <memory>
#include <array>
#include <assert.h>
#include "algorithm.h"

namespace ccnt {
    template<std::uint32_t TBits>
    class Bitmask {
    public:
        static constexpr std::uint32_t size() {
            float size = static_cast<float>(TBits) / 64;
            return ceil(size);
        } 

        using uint_t = std::conditional_t<(TBits <= 8), std::uint8_t,
                std::conditional_t<(TBits <= 16), std::uint16_t,
                std::conditional_t<(TBits <= 32), std::uint32_t,
                std::conditional_t<(TBits <= 64), std::uint64_t,
                std::array<uint64_t, size()>>>>>;

    public:
        class Proxy {
        public:
            inline Proxy(uint_t* bitmask, std::uint32_t nbit) : m_bitmask(bitmask), m_nbit(nbit) {}
            inline Proxy& operator = (const Proxy& proxy) {
                if (proxy) {
                    reinterpret_cast<Bitmask<TBits>*>(m_bitmask)->set_bit(m_nbit); 
                }
                else {
                    reinterpret_cast<Bitmask<TBits>*>(m_bitmask)->unset_bit(m_nbit); 
                }

                return *this;
            }

            inline Proxy& operator = (bool value) {
                if (value) {
                    reinterpret_cast<Bitmask<TBits>*>(m_bitmask)->set_bit(m_nbit); 
                }
                else {
                    reinterpret_cast<Bitmask<TBits>*>(m_bitmask)->unset_bit(m_nbit); 
                }

                return *this;
            }

            inline bool operator < (const Proxy& proxy) {
                return static_cast<bool>(*this) < static_cast<bool>(proxy);
            }

            template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint8_t>::value, std::nullptr_t>::type = nullptr>
            inline operator bool() const {
                assert(m_nbit < TBits);
                return (*m_bitmask << (m_nbit + (sizeof(uint_t) * 8 - TBits))) & 0x80;
            }

            template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint16_t>::value, std::nullptr_t>::type = nullptr>
            inline operator bool() const {
                assert(m_nbit < TBits);
                return (*m_bitmask << (m_nbit + (sizeof(uint_t) * 8 - TBits))) & 0x8000;
            }

            template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint32_t>::value, std::nullptr_t>::type = nullptr>
            inline operator bool() const {
                assert(m_nbit < TBits);
                return (*m_bitmask << (m_nbit + (sizeof(uint_t) * 8 - TBits))) & 0x80000000;
            }

            template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint64_t>::value, std::nullptr_t>::type = nullptr>
            inline operator bool() const {
                assert(m_nbit < TBits);
                return (*m_bitmask << (m_nbit + (sizeof(uint_t) * 8 - TBits))) & 0x8000000000000000;
            }

            template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
            inline operator bool() const {
                assert(m_nbit < TBits);
                std::uint32_t index = m_nbit/64;
                return ((*m_bitmask)[index] << (m_nbit + (sizeof(uint_t) * 8 - (TBits - (index * 64))))) & 0x8000000000000000;
            }

        private:
            uint_t* m_bitmask;
            std::uint32_t m_nbit;
        };

    public:
        class Iterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = Proxy;
            using Reference = Proxy;

        public:
            Iterator(Bitmask<TBits>* bitmask, std::uint32_t nbit) : m_bitmask(bitmask), m_nbit(nbit) {}
            ~Iterator() = default;

            Reference operator * () {
                return (*m_bitmask)[m_nbit];
            }

            Pointer operator -> () {
                return (*m_bitmask)[m_nbit];
            }

            void operator ++ () {
                m_nbit++;
            }

            bool operator != (const Iterator& it ) {
                return ((it.m_bitmask != m_bitmask) || (m_nbit != it.m_nbit));
            }

            bool operator == (const Iterator& it ) {
                return ((it.m_bitmask == m_bitmask) && (m_nbit == it.m_nbit));
            }

        protected:
            Bitmask<TBits>* m_bitmask;
            std::uint32_t m_nbit;
        };

        class ReverseIterator : public Iterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = Proxy;
            using Reference = Proxy;

        public:
            ReverseIterator(Bitmask<TBits>* bitmask, std::uint32_t nbit) : Iterator(bitmask, nbit) {}
            ~ReverseIterator() = default;

            Reference operator * () {
                return (*Iterator::m_bitmask)[Iterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*Iterator::m_bitmask)[Iterator::m_nbit - 1];
            }

            void operator ++ () {
                Iterator::m_nbit--;
            }

            bool operator != (const ReverseIterator& it ) const {
                return ((it.m_bitmask != Iterator::m_bitmask) || (Iterator::m_nbit != it.m_nbit));
            }

            bool operator == (const ReverseIterator& it ) const {
                return ((it.m_bitmask == Iterator::m_bitmask) && (Iterator::m_nbit == it.m_nbit));
            }
        };

        class ConstIterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;

        public:
            ConstIterator(const Bitmask<TBits>* bitmask, std::uint32_t nbit) : m_bitmask(bitmask), m_nbit(nbit) {}
            ~ConstIterator() = default;

            Reference operator * () const {
                return (*m_bitmask)[m_nbit];
            }

            Pointer operator -> () const {
                return (*m_bitmask)[m_nbit];
            }

            void operator ++ () {
                m_nbit++;
            }

            bool operator != (const ConstIterator& it ) const {
                return ((it.m_bitmask != m_bitmask) || (m_nbit != it.m_nbit));
            }

            bool operator == (const ConstIterator& it ) const {
                return ((it.m_bitmask == m_bitmask) && (m_nbit == it.m_nbit));
            }

        protected:
            const Bitmask<TBits>* m_bitmask;
            std::uint32_t m_nbit;
        };

        class ConstReverseIterator : public ConstIterator {
        public:
            using Type = SparseIterator;
            using ValueType = bool;
            using Pointer   = bool;
            using Reference = bool;

        public:
            ConstReverseIterator(const Bitmask<TBits>* bitmask, std::uint32_t nbit) : ConstIterator(bitmask, nbit) {}
            ~ConstReverseIterator() = default;

            Reference operator * () {
                return (*ConstIterator::m_bitmask)[ConstIterator::m_nbit - 1];
            }

            Pointer operator -> () {
                return (*ConstIterator::m_bitmask)[ConstIterator::m_nbit - 1];
            }

            void operator ++ () {
                ConstIterator::m_nbit--;
            }

            bool operator != (const ConstReverseIterator& it ) const {
                return ((it.m_bitmask != ConstIterator::m_bitmask) || (ConstIterator::m_nbit != it.m_nbit));
            }

            bool operator == (const ConstReverseIterator& it ) const {
                return ((it.m_bitmask == ConstIterator::m_bitmask) && (ConstIterator::m_nbit == it.m_nbit));
            }
        };

    public:
        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        Bitmask() : m_bitmask(0) {}

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        Bitmask() : m_bitmask{0} {}

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        Bitmask(uint_t bitmask) : m_bitmask(bitmask) {}

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        Bitmask(uint_t& bitmask) {
            for (uint32_t i = 0; i < size(); i++) {
                m_bitmask[i] = bitmask[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator & (const uint_t& bitmask) const {
            return Bitmask<TBits>(m_bitmask & bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator & (const uint_t& bitmask) const {
            uint_t out_bitmask;
            for (std::uint32_t i = 0; i < size(); i++) {
                out_bitmask[i] = m_bitmask[i] & bitmask[i]; 
            } 
            return Bitmask<TBits>(out_bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (const uint_t& bitmask) {
            m_bitmask &= bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (const uint_t& bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitmask[i] &= bitmask[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator | (const uint_t& bitmask) const {
            return Bitmask<TBits>(m_bitmask | bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator | (const uint_t& bitmask) const {
            uint_t out_bitmask;
            for (std::uint32_t i = 0; i < size(); i++) {
                out_bitmask[i] = m_bitmask[i] | bitmask[i]; 
            } 
            return Bitmask<TBits>(out_bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (const uint_t& bitmask) {
            m_bitmask |= bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (const uint_t& bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitmask[i] |= bitmask[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const uint_t& bitmask) const {
            return m_bitmask == bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const uint_t& bitmask) const {
            for (std::uint32_t i = 0; i < size(); i++) {
                if (m_bitmask[i] != bitmask[i]) {
                    return false;
                }
            } 
            return true;
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const uint_t& bitmask) const {
            return m_bitmask != bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const uint_t& bitmask) const {
            for (std::uint32_t i = 0; i < size(); i++) {
                if (m_bitmask[i] != bitmask[i]) {
                    return true;
                }
            } 
            return false;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator & (const TBitmask& bitmask) const {
            return Bitmask<TBits>(*this & bitmask.m_bitmask);
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (const TBitmask& bitmask) {
            *this &= bitmask.m_bitmask;
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator | (const TBitmask& bitmask) const {
            return Bitmask<TBits>(*this | bitmask.m_bitmask);
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (const TBitmask& bitmask) {
            *this |= bitmask.m_bitmask;
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (const TBitmask& bitmask) const {
            return *this == bitmask.m_bitmask;
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<uint_t, typename TBitmask::uint_t>::value || !std::is_same<typename TBitmask::uint_t, std::array<std::uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (const TBitmask& bitmask) const {
            return *this != bitmask.m_bitmask;
        }
        
        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint8_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x80;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint16_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x8000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint32_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x80000000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint64_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x8000000000000000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator[] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            std::uint32_t index = nbit / 64;
            return (m_bitmask[index] << (nbit + (sizeof(uint_t) * 8 - (TBits - (index * 64))))) & 0x8000000000000000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint8_t>::value, std::nullptr_t>::type = nullptr>
        inline Proxy operator [] (std::uint32_t nbit) {
            assert(nbit < TBits);
            return Proxy(&m_bitmask, nbit);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint16_t>::value, std::nullptr_t>::type = nullptr>
        inline Proxy operator [] (std::uint32_t nbit) {
            assert(nbit < TBits);
            return Proxy(&m_bitmask, nbit);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint32_t>::value, std::nullptr_t>::type = nullptr>
        inline Proxy operator [] (std::uint32_t nbit) {
            assert(nbit < TBits);
            return Proxy(&m_bitmask, nbit);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint64_t>::value, std::nullptr_t>::type = nullptr>
        inline Proxy operator [] (std::uint32_t nbit) {
            assert(nbit < TBits);
            return Proxy(&m_bitmask, nbit);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Proxy operator [] (std::uint32_t nbit) {
            assert(nbit < TBits);
            return Proxy(&m_bitmask, nbit);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void set_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            m_bitmask |= 1ULL << ((TBits - 1) - nbit); 
        }
        
        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void set_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            std::uint32_t index = nbit/64;
            m_bitmask[index] |= 1ULL << ((TBits - 1) - nbit - (index * 64)); 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void unset_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            m_bitmask &= ~(1ULL << ((TBits - 1) - nbit));
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void unset_bit(std::uint32_t nbit) {
            assert(nbit < TBits);
            std::uint32_t index = nbit/64;
            m_bitmask[index] &= ~(1ULL << ((TBits - 1) - nbit - (index * 64)));
        }

        inline const uint_t& get_data() const {
            return m_bitmask;
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
        uint_t m_bitmask;

        template<std::uint32_t TBits1>
        friend class Bitmask;
    };
}
