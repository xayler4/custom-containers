#pragma once

#include <cstdint>
#include <memory>
#include <assert.h>

namespace ccnt {
    template<typename TValue>
    struct Node {
    public:
        template<typename... TArgs>
        Node(std::nullptr_t dummy,  Node<TValue>* previous,  Node<TValue>* next, TArgs&&... args) : value(std::forward<TArgs>(args)...), previous(previous), next(next) {
            if (this->previous != nullptr) {
                this->previous->next = this;
            }
            if (this->next != nullptr) {
                this->next->previous = this;
            }
        }

        Node(Node<TValue>* previous,  Node<TValue>* next, TValue&& value) : value(std::forward<TValue>(value)), previous(previous), next(next) {
            if (this->previous != nullptr) {
                this->previous->next = this;
            }
            if (this->next != nullptr) {
                this->next->previous = this;
            }
        }

        Node(Node<TValue>* previous,  Node<TValue>* next, const TValue& value) : value(std::move(value)), previous(previous), next(next) {
            if (this->previous != nullptr) {
                this->previous->next = this;
            }
            if (this->next != nullptr) {
                this->next->previous = this;
            }
        }

        ~Node() {
            if (this->previous != nullptr) {
                this->previous->next = next;
            }
            if (this->next != nullptr) {
                this->next->previous = previous;
            }
        }

        inline operator TValue&() { return value; }

        Node(const Node<TValue>&) = delete;
        Node operator = (const Node<TValue>&) = delete;

    public: 
        TValue value;
        Node<TValue>* previous;
        Node<TValue>* next;

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

            void operator ++ () {
                m_node = m_node->next;
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
        DoublyLinkedList() : m_length(0) {
            m_head = m_allocator.allocate(1);
            m_tail = m_allocator.allocate(1);

            m_inactive_head = m_head;
            m_inactive_tail = m_tail;

            m_head->next = m_tail;
            m_head->previous = nullptr;
            m_tail->previous = m_head;
            m_tail->next = nullptr;
        }

        ~DoublyLinkedList() {
            for (Node<TValue>& node : *this){
                erase(node);
            }
        }

        template<typename... TArgs>
        Node<TValue>& emplace_after(Node<TValue>& previous, TArgs&&... args) {
            if (&previous == m_tail) {
                return emplace_at_tail(std::forward<TArgs>(args)...);
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, nullptr, &previous, previous.next, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_after(Node<TValue>& previous, TValue&& value) {
            if (&previous == m_tail) {
                return insert_at_tail(std::forward<TValue>(value));
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, &previous, previous.next, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_after(Node<TValue>& previous, const TValue& value) {
            if (&previous == m_tail) {
                return insert_at_tail(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, &previous, previous.next, value);
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_before(Node<TValue>& next, TArgs&&... args) {
            if (&next == m_head) {
                return emplace_at_head(std::forward<TArgs>(args)...);
            }

            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, nullptr, next.previous, &next, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_before(Node<TValue>& next, TValue&& value) {
            if (&next == m_head) {
                return insert_at_head(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, next.previous, &next, std::forward<TValue>(value));
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_before(Node<TValue>& next, const TValue& value) {
            if (&next == m_head) {
                return insert_at_head(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, next.previous, &next, value);
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_at_tail(TArgs&&... args) {
            if (m_tail == m_inactive_tail) {
                std::construct_at(&m_tail->value, std::forward<TArgs>(args)...);
                m_inactive_tail = nullptr;
            }
            else {
                Node<TValue>* tail = m_allocator.allocate(1);
                std::construct_at(tail, nullptr, m_tail, nullptr, std::forward<TArgs>(args)...);
                m_tail = tail;
            }
            m_length++;
            return *m_tail;
        }

        Node<TValue>& insert_at_tail(TValue&& value) {
            if (m_tail == m_inactive_tail) {
                std::construct_at(&m_tail->value, std::forward<TValue>(value));
                m_inactive_tail = nullptr;
            }
            else {
                Node<TValue>* tail = m_allocator.allocate(1);
                std::construct_at(tail, m_tail, nullptr, std::forward<TValue>(value));
                m_tail = tail;
            }
            m_length++;
            return *m_tail;
        }

        Node<TValue>& insert_at_tail(const TValue& value) {
            if (m_tail == m_inactive_tail) {
                m_tail->value = std::move(value);
                m_inactive_tail = nullptr;
            }
            else {
                Node<TValue>* tail = m_allocator.allocate(1);
                std::construct_at(tail, m_tail, nullptr, value);
                m_tail = tail;
            }
            m_length++;
            return *m_tail;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_at_head(TArgs&&... args) {
            if (m_head == m_inactive_head) {
                std::construct_at(&m_head->value, std::forward<TArgs>(args)...);
                m_inactive_head = nullptr;
            }
            else {
                Node<TValue>* head = m_allocator.allocate(1);
			    std::construct_at(head, nullptr, m_head, std::forward<TArgs>(args)...);
                m_head = head;
            }
            m_length++;
            return *m_head;
        }

        Node<TValue>& insert_at_head(TValue&& value) {
            if (m_head == m_inactive_head) {
                std::construct_at(&m_head->value, std::forward<TValue>(value));
                m_inactive_head = nullptr;
            }
            else {
                Node<TValue>* head = m_allocator.allocate(1);
			    std::construct_at(head, nullptr, m_head, std::forward<TValue>(value));
                m_head = head;
            }
            m_length++;
            return *m_head;
        }

        Node<TValue>& insert_at_head(const TValue& value) {
            if (m_head == m_inactive_head) {
                m_head->value = std::move(value);
                m_inactive_head = nullptr;
            }
            else {
                Node<TValue>* head = m_allocator.allocate(1);
			    std::construct_at(head, nullptr, m_head, value);
                m_head = head;
            }
            m_length++;
            return *m_head;
        }

        Node<TValue>* erase(Node<TValue>& node) {
            assert(m_length);
            if (&node == m_tail) {
                return pop_tail();
            }
            if (&node == m_head) {
                return pop_head();
            }
            Node<TValue>* next = node.next;
            std::destroy_at(&node);
			m_allocator.deallocate(&node, 1);
            m_length--;
            return next;
        }

        Node<TValue>* pop_tail() {
            assert(m_length);
            if (m_tail->previous == m_head) {
                if (m_tail != m_inactive_tail) {
                    std::destroy_at(&m_tail->value);
                    m_inactive_tail = m_tail;
                    m_length--;
                    return nullptr;
                }
                else {
                    return pop_head();
                }
            }
            Node<TValue>* new_tail = m_tail->previous;
            if (m_tail != m_inactive_tail) {
                std::destroy_at(m_tail);
            }
			m_allocator.deallocate(m_tail, 1);

            m_tail = new_tail;
            m_length--;
            return m_tail;
        }

        Node<TValue>* pop_head() {
            assert(m_length);
            if (m_head->next == m_tail) {
                if (m_head != m_inactive_head) {
                    std::destroy_at(&m_head->value);
                    m_inactive_head = m_head;
                    m_length--;
                    return nullptr;
                }
                else {
                    return pop_tail();
                }
            }
            Node<TValue>* new_head = m_head->next;
            if (m_head != m_inactive_head) {
                std::destroy_at(m_head);
            }
			m_allocator.deallocate(m_head, 1);

            m_head = new_head;
            m_length--;
            return m_head;
        }

        Iterator begin() {
            Node<TValue>* node = m_head;
            if (m_head == m_inactive_head) {
                node = node->next;
            }
            return Iterator(node);
        }

        Iterator end() {
            Node<TValue>* node = nullptr;
            if (m_tail == m_inactive_tail) {
                node = m_tail;
            }
            return Iterator(node);
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

        DoublyLinkedList(const DoublyLinkedList<TValue, TAllocator>&) = delete;

    private:
        Node<TValue>* m_head;
        Node<TValue>* m_tail;
        Node<TValue>* m_inactive_head;
        Node<TValue>* m_inactive_tail;
        std::uint32_t m_length;
        TAllocator m_allocator;
    };
}
