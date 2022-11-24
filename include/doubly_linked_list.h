#pragma once

#include <cstdint>
#include <memory>

namespace ccnt {
    template<typename TValue>
    class Node {
    public:
        template<typename... TArgs>
        Node(std::nullptr_t dummy,  Node<TValue>* previous,  Node<TValue>* next, TArgs&&... args) : m_value(TValue(std::forward<TArgs>(args)...)), m_previous(previous), m_next(next) {
            if (m_previous != nullptr) {
                m_previous->m_next = this;
            }
            if (m_next != nullptr) {
                m_next->m_previous = this;
            }
        }

        Node(Node<TValue>* previous,  Node<TValue>* next, TValue&& value) : m_value(std::forward<TValue>(value)), m_previous(previous), m_next(next) {
            if (m_previous != nullptr) {
                m_previous->m_next = this;
            }
            if (m_next != nullptr) {
                m_next->m_previous = this;
            }
        }

        Node(Node<TValue>* previous,  Node<TValue>* next, TValue& value) : m_value(std::move(value)), m_previous(previous), m_next(next) {
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

        inline operator TValue&() { return m_value; }
        inline Node<TValue>* get_next() const { return m_next; }
        inline Node<TValue>* get_previous() const { return m_previous; }

        Node(const Node<TValue>&) = delete;
        Node operator = (const Node<TValue>&) = delete;

    private: 
        TValue m_value;
        Node<TValue>* m_previous;
        Node<TValue>* m_next;
    };


    template<typename TValue, typename TAllocator = std::allocator<Node<TValue>>>
    class DoublyLinkedList {
    public:
        class Iterator {
        public:
            using ValueType = ::ccnt::Node<TValue>;
            using Pointer   = ::ccnt::Node<TValue>*;
            using Reference = ::ccnt::Node<TValue>&;

        public:
            Iterator(Pointer node) : m_node(node) {
            };

            ~Iterator() = default;

            Reference operator * () {
                return *m_node;
            }

            Pointer operator -> () {
                return m_node; 
            }

            Reference operator ++ () {
                m_node = m_node->get_next();
                return *m_node;
            }

            bool operator == (const Iterator& it) {
                return m_node == it.m_node;
            }

            bool operator != (const Iterator& it) {
                return m_node != it.m_node;
            }

        private:
            Pointer m_node;
        };


    public:
        DoublyLinkedList() : m_head(nullptr), m_tail(nullptr), m_length(0) {
            
        }

        ~DoublyLinkedList() {
            
        }

        template<typename... TArgs>
        Node<TValue>& emplace_after(Node<TValue>& previous, TArgs&&... args) {
            Node<TValue>* ptr = &previous;
            if (ptr == m_tail) {
                return emplace_at_tail(std::forward<TArgs>(args)...);
            }
            Node<TValue>* new_node = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(new_node, nullptr, previous, nullptr, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_after(Node<TValue>& previous, TValue&& value) {
            Node<TValue>* ptr = &previous;
            if (ptr == m_tail) {
                return insert_at_tail(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(new_node, previous, nullptr, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_after(Node<TValue>& previous, TValue& value) {
            Node<TValue>* ptr = &previous;
            if (ptr == m_tail) {
                return insert_at_tail(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(new_node, previous, nullptr, value);
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_before(Node<TValue>& next, TArgs&&... args) {
            Node<TValue>* ptr = &next;
            if (ptr == m_head) {
                return emplace_at_head(std::forward<TArgs>(args)...);
            }

            Node<TValue>* new_node = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(new_node, nullptr, nullptr, next, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_before(Node<TValue>& next, TValue&& value) {
            Node<TValue>* ptr = &next;
            if (ptr == m_head) {
                return insert_at_head(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(new_node, nullptr, next, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_before(Node<TValue>& next, TValue& value) {
            Node<TValue>* ptr = &next;
            if (ptr == m_head) {
                return insert_at_head(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(new_node, nullptr, next, value);
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_at_tail(TArgs&&... args) {
            Node<TValue>* tail = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(tail, nullptr, nullptr, m_tail, std::forward<TArgs>(args)...);
            m_tail = tail;
            m_length++;
            return *m_tail;
        }

        Node<TValue>& insert_at_tail(TValue&& value) {
            Node<TValue>* tail = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(tail, nullptr, m_tail, std::forward<TValue>(value));
            m_tail = tail;
            m_length++;
            return *m_tail;
        }

        Node<TValue>& insert_at_tail(TValue& value) {
            Node<TValue>* tail = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(tail, nullptr, m_tail, value);
            m_tail = tail;
            m_length++;
            return *m_tail;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_at_head(TArgs&&... args) {
            Node<TValue>* head = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(head, nullptr, m_head, std::forward<TArgs>(args)...);
            m_head = head;
            m_length++;
            return *m_head;
        }

        Node<TValue>& insert_at_head(TValue&& value) {
            Node<TValue>* head = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(head, nullptr, m_head, std::forward<TValue>(value));
            m_head = head;
            m_length++;
            return *m_head;
        }

        Node<TValue>& insert_at_head(TValue& value) {
            Node<TValue>* head = m_allocator.allocate(sizeof(Node<TValue>));
			std::construct_at(head, nullptr, m_head, value);
            m_head = head;
            m_length++;
            return *m_head;
        }

        Node<TValue>* erase(Node<TValue>& node) {
            Node<TValue>* ptr = &node;
            if (ptr == m_tail) {
                return pop_tail();
            }
            if (ptr == m_head) {
                return pop_head();
            }
            Node<TValue>* next = ptr->get_next();
            delete ptr;
            m_length--;
            return next;
        }

        Node<TValue>* pop_tail() {
            Node<TValue>* new_tail = m_tail.get_previous();
            delete m_tail;
            m_tail = new_tail;
            m_length--;
            return m_tail;
        }

        Node<TValue>* pop_head() {
            Node<TValue>* new_head = m_tail.get_next();
            delete m_head;
            m_head = new_head;
            m_length--;
            return m_head;
        }

        Iterator begin() {
            return Iterator(m_head);
        }

        Iterator end() {
            return Iterator(nullptr);
        }

        inline const Node<TValue>* get_head() const { 
            return m_head; 
        }
        inline const Node<TValue>* get_tail() const {
            return m_tail; 
        }
        inline std::uint32_t get_length() const {
            return m_length;
        }

    private:
        Node<TValue>* m_head;
        Node<TValue>* m_tail;
        std::uint32_t m_length;
        TAllocator m_allocator;
    };
}
