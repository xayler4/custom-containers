#pragma once

#include <cstdint>
#include <utility>
#include <memory>
#include <assert.h>

namespace ccnt {
    template<std::uint8_t TBits>
    class Bitmask {
    public:
        using uint_t = std::conditional_t<(TBits <= 8), std::uint8_t,
                std::conditional_t<(TBits <= 16), std::uint16_t,
                std::conditional_t<(TBits <= 32), std::uint32_t,
                std::conditional_t<(TBits <= 64), std::uint64_t,
                std::nullptr_t>>>>;

        class Iterator {
        public:
            Iterator(Bitmask<TBits>& bitmask, std::uint8_t nbit) : m_bitmask(bitmask), m_nbit(nbit) {}
            ~Iterator() = default;

            bool operator * () {
                return m_bitmask[m_nbit];
            }

            bool operator -> () {
                return m_bitmask[m_nbit];
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
            Bitmask<TBits>& m_bitmask;
            std::uint8_t m_nbit;
        };

    public:
        Bitmask() : m_bitmask(0) {}
        Bitmask(uint_t bitmask) : m_bitmask(bitmask) {}

        inline Bitmask<TBits> operator & (uint_t bitmask) {
            return Bitmask<TBits>(m_bitmask & bitmask);
        }

        inline void operator &= (uint_t bitmask) {
            m_bitmask &= bitmask;
        }

        inline Bitmask<TBits> operator | (uint_t bitmask) {
            return Bitmask<TBits>(m_bitmask | bitmask);
        }

        inline void operator |= (uint_t bitmask) {
            m_bitmask |= bitmask;
        }

        inline bool operator == (uint_t bitmask) {
            return m_bitmask == bitmask;
        }

        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator & (TBitmask bitmask) {
            return Bitmask<TBits>(m_bitmask & bitmask.m_bitmask);
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline void operator &= (TBitmask bitmask) {
            m_bitmask &= bitmask.m_bitmask;
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline Bitmask<TBits> operator | (TBitmask bitmask) {
            return Bitmask<TBits>(m_bitmask | bitmask.m_bitmask);
        }
        
        template<typename TBitmask, typename std::enable_if<std::is_same<typename TBitmask::uint_t, uint_t>::value, std::nullptr_t>::type = nullptr>
        inline void operator |= (TBitmask bitmask) {
            m_bitmask |= bitmask.m_bitmask;
        }
        
        template<typename TBitmask>
        inline bool operator == ( TBitmask bitmask) {
            return m_bitmask == bitmask.m_bitmask;
        }

        template<typename TBitmask>
        inline bool operator != ( TBitmask bitmask) {
            return m_bitmask != bitmask.m_bitmask;
        }
        
        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint8_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint8_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit)) & 0x80;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint16_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint8_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit)) & 0x8000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint32_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint8_t nbit) const {
            assert(nbit < TBits);
            return (m_bitmask << (nbit)) & 0x80000000;
        }

        template<typename T = uint_t, typename std::enable_if<std::is_same<T, std::uint64_t>::value, std::nullptr_t>::type = nullptr>
        inline bool operator [] (std::uint8_t nbit) const{
            assert(nbit < TBits);
            return (m_bitmask << (nbit)) & 0x800000000000;
        }

        inline void set_bit(std::uint8_t nbit) {
            m_bitmask |= 1UL << ((TBits - 1) - nbit); 
        }

        inline void unset_bit(std::uint8_t nbit) {
            m_bitmask &= ~(1UL << ((TBits - 1) - nbit));
        }

        Iterator begin() {
            return Iterator(*this, 0);
        }

        Iterator end() {
            return Iterator(*this, TBits);
        }

    private:
        uint_t m_bitmask;
    };
}
