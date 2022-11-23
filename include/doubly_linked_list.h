#pragma once

#include <cstdint>

namespace ccnt {
    template<typename TValue>
    class DoublyLinkedList {
    public:
        class Node {
        public:
            template<typename... TArgs>
            Node(std::nullptr_t dummy,  Node* previous,  Node* next, TArgs&&... args) : m_value(TValue(std::forward<TArgs>(args)...)), m_previous(previous), m_next(next) {
                if (m_previous != nullptr) {
                    m_previous->m_next = this;
                }
                if (m_next != nullptr) {
                    m_next->m_previous = this;
                }
            }

            Node(Node* previous,  Node* next, TValue&& value) : m_value(std::forward<TValue>(value)), m_previous(previous), m_next(next) {
                if (m_previous != nullptr) {
                    m_previous->m_next = this;
                }
                if (m_next != nullptr) {
                    m_next->m_previous = this;
                }
            }

            Node(Node* previous,  Node* next, TValue& value) : m_value(value), m_previous(previous), m_next(next) {
                if (m_previous != nullptr) {
                    m_previous->m_next = this;
                }
                if (m_next != nullptr) {
                    m_next->m_previous = this;
                }
            }

            ~Node() {
                if (m_previous != nullptr) {
                    m_previous->m_next = m_next;
                }
                if (m_next != nullptr) {
                    m_next->m_previous = m_previous;
                }
            }

            inline Node* get_next() const { return m_next; }
            inline Node* get_previous() const { return m_previous; }

            Node(const Node&) = delete;
            Node operator = (const Node&) = delete;

        private: 
            TValue m_value;
            Node* m_previous;
            Node* m_next;
        };

    public:
        DoublyLinkedList() : m_tail(nullptr), m_head(nullptr), m_length(0) {
            
        }

        ~DoublyLinkedList() {
            
        }

        template<typename... TArgs>
        Node& emplace_after(Node& previous, TArgs&&... args) {
            Node* ptr = &previous;
            if (ptr == m_head) {
                return emplace_at_head(std::forward<TArgs>(args)...);
            }
            Node* new_node = new Node(nullptr, previous, nullptr, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node& insert_after(Node& previous, TValue&& value) {
            Node* ptr = &previous;
            if (ptr == m_head) {
                return insert_at_head(value);
            }
            Node* new_node = new Node(previous, nullptr, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node& insert_after(Node& previous, TValue& value) {
            Node* ptr = &previous;
            if (ptr == m_head) {
                return insert_at_head(value);
            }
            Node* new_node = new Node(previous, nullptr, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node& emplace_before(Node& next, TArgs&&... args) {
            Node* ptr = &next;
            if (ptr == m_tail) {
                return emplace_at_tail(std::forward<TArgs>(args)...);
            }

            Node* new_node = new Node(nullptr, nullptr, next, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node& insert_before(Node& next, TValue&& value) {
            Node* ptr = &next;
            if (ptr == m_tail) {
                return insert_at_tail(value);
            }
            Node* new_node = new Node(nullptr, next, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node& insert_before(Node& next, TValue& value) {
            Node* ptr = &next;
            if (ptr == m_tail) {
                return insert_at_tail(value);
            }
            Node* new_node = new Node(nullptr, next, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node& emplace_at_head(TArgs&&... args) {
            Node* new_node = new Node(nullptr, m_head, nullptr, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node& insert_at_head(TValue&& value) {
            Node* new_node = new Node(m_head, nullptr, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node& insert_at_head(TValue& value) {
            Node* new_node = new Node(m_head, nullptr, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node& emplace_at_tail(TArgs&&... args) {
            Node* new_node = new Node(nullptr, m_tail, nullptr, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node& insert_at_tail(TValue&& value) {
            Node* new_node = new Node(m_tail, nullptr, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node& insert_at_tail(TValue& value) {
            Node* new_node = new Node(m_tail, nullptr, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        void erase(Node& node) {
            Node* ptr = &node;
            if (ptr == m_head) {
                return pop_head();
            }
            if (ptr == m_tail) {
                return pop_tail();
            }
            delete ptr;
            m_length--;
        }

        void pop_head() {
            Node* new_head = m_head.get_previous();
            delete m_head;
            m_head = new_head;
            m_length--;
        }

        void pop_tail() {
            Node* new_tail = m_head.get_next();
            delete m_tail;
            m_tail = new_tail;
            m_length--;
        }

        inline const Node* get_tail() const { 
            return m_tail; 
        }
        inline const Node* get_head() const {
            return m_head; 
        }

    private:
        Node* m_tail;
        Node* m_head;
        std::uint32_t m_length;
    };
}
