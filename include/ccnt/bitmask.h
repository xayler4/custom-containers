#pragma once

#include <iostream>
#include <cmath>
#include <cstdint>
#include <utility>
#include <memory>
#include <array>
#include <assert.h>

namespace ccnt {
    template<std::uint32_t TBits>
    class Bitmask {
    public:
        static constexpr std::uint32_t size() {
            float i = (float)TBits / 64;
            return ceil(TBits);
        } 
        using uint_t = std::conditional_t<(TBits <= 8), std::uint8_t,
                std::conditional_t<(TBits <= 16), std::uint16_t,
                std::conditional_t<(TBits <= 32), std::uint32_t,
                std::conditional_t<(TBits <= 64), std::uint64_t,
                std::array<uint64_t, size()>>>>>;

        class Iterator {
        public:
            Iterator(Bitmask<TBits>* bitmask, std::uint32_t nbit) : m_bitmask(bitmask), m_nbit(nbit) {}
            ~Iterator() = default;

            bool operator * () {
                return (*m_bitmask)[m_nbit];
            }

            bool operator -> () {
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

        private:
            Bitmask<TBits>* m_bitmask;
            std::uint32_t m_nbit;
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
        inline Bitmask<TBits> operator & (uint_t bitmask) {
            return Bitmask<TBits>(m_bitmask & bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits>&& operator & (uint_t bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                bitmask[i] &= m_bitmask[i]; 
            } 
            return Bitmask<TBits>(bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (uint_t& bitmask) {
            m_bitmask &= bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (uint_t& bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitmask[i] &= bitmask[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator | (uint_t bitmask) {
            return Bitmask<TBits>(m_bitmask | bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits>&& operator | (uint_t bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                bitmask[i] |= m_bitmask[i]; 
            } 
            return Bitmask<TBits>(bitmask);
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (uint_t bitmask) {
            m_bitmask |= bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (uint_t& bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                m_bitmask[i] |= bitmask[i]; 
            } 
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (uint_t bitmask) {
            return m_bitmask == bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator == (uint_t& bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                if (m_bitmask[i] != bitmask[i]) {
                    return false;
                }
            } 
            return true;
        }

        template<typename T = uint_t, typename std::enable_if<!std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (uint_t bitmask) {
            return m_bitmask != bitmask;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator != (uint_t& bitmask) {
            for (std::uint32_t i = 0; i < size(); i++) {
                if (m_bitmask[i] != bitmask[i]) {
                    return true;
                }
            } 
            return false;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits>&& operator & (TBitmask& bitmask) {
            return Bitmask<TBits>(*this & bitmask.m_bitmask);
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (TBitmask& bitmask) {
            *this &= bitmask.m_bitmask;
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits>&& operator | (TBitmask& bitmask) {
            return Bitmask<TBits>(*this | bitmask.m_bitmask);
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (TBitmask& bitmask) {
            *this |= bitmask.m_bitmask;
        }
        
        template<typename TBitmask>
        inline bool operator == ( TBitmask& bitmask) {
            return *this == bitmask.m_bitmask;
        }

        template<typename TBitmask>
        inline bool operator != ( TBitmask& bitmask) {
            return *this != bitmask.m_bitmask;
        }
        
        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint8_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x80;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint16_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x8000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint32_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x80000000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint64_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit + (sizeof(uint_t) * 8 - TBits))) & 0x8000000000000000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::array<uint64_t, size()>>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint32_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask[nbit/64] << (nbit + (sizeof(uint_t) * 8 - (TBits - (nbit/64 * 64))))) & 0x8000000000000000;
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
            m_bitmask[index] |= ~(1ULL << ((TBits - 1) - nbit - (index * 64)));
        }

        Iterator begin() {
            return Iterator(this, 0);
        }

        Iterator end() {
            return Iterator(this, TBits);
        }

    private:
        uint_t m_bitmask;
    };
}
