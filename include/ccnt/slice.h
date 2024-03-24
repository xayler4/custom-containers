#pragma once

namespace ccnt {
    template<typename T, typename TIterator = T::Iterator, typename = std::nullptr_t>
    class Slice {
        Slice(TIterator begin, TIterator end) : m_begin(begin), m_end(end) {
        }

        TIterator begin() {
            return m_begin;
        }

        TIterator begin() const {
            return m_begin;
        }

        TIterator end() {
            return m_end;
        }

        TIterator end() const {
            return m_end;
        }

    private:
        TIterator m_begin;
        TIterator m_end;
    };

    template<typename T, typename TIterator>
    class Slice<T, TIterator, typename std::enable_if<std::is_same<RandomAccessIterator, typename TIterator::Type>::value, std::nullptr_t>::type> {
    public:
        Slice(TIterator begin, TIterator end) : m_begin(begin), m_end(end), m_count(abs(m_end - m_begin)) {
        }

        TIterator begin() {
            return m_begin;
        }

        TIterator begin() const {
            return m_begin;
        }

        TIterator end() {
            return m_end;
        }

        TIterator end() const {
            return m_end;
        }

        inline std::uint32_t get_count() {
            return m_count;
        }

        inline TIterator::Reference operator[](std::uint32_t index) {
            assert(index < get_count());
            return *(m_begin + index);
        }

        inline TIterator::Reference operator[](std::uint32_t index) const {
            assert(index < get_count());
            return *(m_begin + index);
        }

    private:
        TIterator m_begin;
        TIterator m_end;
        std::uint32_t m_count;
    };
}
