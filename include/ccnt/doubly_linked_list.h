#pragma once

#include "algorithm.h"
#include <cstdint>
#include <memory>
#include <assert.h>

namespace ccnt {
    template<typename TValue>
    class Node;
    template<typename TValue, typename TAllocator = std::allocator<Node<TValue>>>
    class DoublyLinkedList;

    template<typename TValue>
    class Node {
    public:
        template<typename... TArgs>
        Node(void* owner_list, Node<TValue>* previous,  Node<TValue>* next, TArgs&&... args) : m_owner_list(owner_list), value(std::forward<TArgs>(args)...), previous(previous), next(next) {
            if (this->previous != nullptr) {
                this->previous->next = this;
            }
            if (this->next != nullptr) {
                this->next->previous = this;
            }
        }

        Node(void* owner_list, Node<TValue>* previous,  Node<TValue>* next, const TValue& value) : m_owner_list(owner_list), value(std::move(value)), previous(previous), next(next) {
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

        void swap(Node& node) {
            assert(node.m_owner_list == m_owner_list);
            auto* owner_list = static_cast<DoublyLinkedList<TValue>*>(m_owner_list);

            if (&node == owner_list->get_head()) {
                owner_list->m_head = this;
            }
            else if (this == owner_list->get_head()) {
                owner_list->m_head = &node;
            }

            if (&node == owner_list->get_tail()) {
                owner_list->m_tail = this;
            }
            else if (this == owner_list->get_tail()) {
                owner_list->m_tail = &node;
            }

            if (&node == next) {
                if (previous != nullptr) {
                    previous->next = &node;
                }
                node.previous = previous;
                previous = &node;
                next = node.next;
                if (node.next != nullptr) {
                    node.next->previous = this;
                }
                node.next = this;
                return;
            }
            else if (&node == previous) {
                if (node.previous != nullptr) {
                    node.previous->next = this;
                }
                previous = node.previous;
                node.previous = this;
                node.next = next;
                if (next != nullptr) {
                    next->previous = &node;
                }
                next = &node;
                return;
            }

            Node* node_previous = node.previous; 
            Node* node_next = node.next; 

            if (node_previous != nullptr) {
                node_previous->next = this;
            }
            if (node_next != nullptr) {
                node_next->previous = this;
            }

            if (previous != nullptr) {
                previous->next = &node;
            }
            if (next != nullptr) {
                next->previous = &node;
            }

            node.previous = previous;
            node.next = next;

            previous = node_previous;
            next = node_next;
        }

        inline operator TValue&() { 
            return value; 
        }

        inline operator const TValue&() const { 
            return value; 
        }

        Node(const Node<TValue>&) = delete;
        Node operator = (const Node<TValue>&) = delete;

    public: 
        TValue value;
        Node<TValue>* previous;
        Node<TValue>* next;

    private:
        void* m_owner_list;
    };

    template<typename TValue, typename TAllocator>
    class DoublyLinkedList {
    public:
        class Iterator {
        public:
            using Type = ListIterator;
            using ValueType = Node<TValue>;
            using Pointer   = Node<TValue>*;
            using Reference = Node<TValue>&;

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

        protected:
            Pointer m_node;
        };

        class ReverseIterator {
        public:
            using Type = ListIterator;
            using ValueType = Node<TValue>;
            using Pointer   = Node<TValue>*;
            using Reference = Node<TValue>&;

        public:
            ReverseIterator(Pointer node) : m_node(node) {
            };

            ~ReverseIterator() = default;

            Reference operator * () {
                return *m_node;
            }

            Pointer operator -> () {
                return m_node; 
            }

            void operator ++ () {
                m_node = m_node->previous;
            }

            bool operator == (const ReverseIterator& it) {
                return m_node == it.m_node;
            }

            bool operator != (const ReverseIterator& it) {
                return m_node != it.m_node;
            }

        protected:
            Pointer m_node;
        };


        class ConstIterator {
        public:
            using Type = ListIterator;
            using ValueType = const Node<TValue>;
            using Pointer   = const Node<TValue>*;
            using Reference = const Node<TValue>&;

        public:
            ConstIterator(Node<TValue>* node) : m_node(node) {
            };

            ~ConstIterator() = default;

            Reference operator * () {
                return *m_node;
            }

            Pointer operator -> () {
                return m_node; 
            }

            void operator ++ () {
                m_node = m_node->next;
            }

            bool operator == (const ConstIterator& it) {
                return m_node == it.m_node;
            }

            bool operator != (const ConstIterator& it) {
                return m_node != it.m_node;
            }

        protected:
            Pointer m_node;
        };

        class ConstReverseIterator : public ConstIterator {
        public:
            using Type = ListIterator;
            using ValueType = const Node<TValue>;
            using Pointer   = const Node<TValue>*;
            using Reference = const Node<TValue>&;

        public:
            ConstReverseIterator(Node<TValue>* node) : ConstIterator(node) {
            };

            ~ConstReverseIterator() = default;

            void operator ++ () {
                ConstIterator::m_node = ConstIterator::m_node->previous;
            }
        };

    public:
        DoublyLinkedList() : m_length(0) {
            m_head = m_allocator.allocate(1);
            m_tail = m_head;
            m_head->previous = nullptr;
            m_head->next = nullptr;
        }

        ~DoublyLinkedList() {
            for (Node<TValue>* node = m_head; node != nullptr; node = node->next) {
                std::destroy_at(node);
                m_allocator.deallocate(node, 1);
            }
        }

        template<typename... TArgs>
        Node<TValue>& emplace_after(Node<TValue>& previous, TArgs&&... args) {
            if (&previous == m_tail) {
                return emplace_at_tail(std::forward<TArgs>(args)...);
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, this, &previous, previous.next, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_after(Node<TValue>& previous, const TValue& value) {
            if (&previous == m_tail) {
                return insert_at_tail(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, this, &previous, previous.next, std::move(value));
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_before(Node<TValue>& next, TArgs&&... args) {
            if (&next == m_head) {
                return emplace_at_head(std::forward<TArgs>(args)...);
            }

            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, this, next.previous, &next, std::forward<TArgs>(args)...);
            m_length++;
            return *new_node;
        }

        Node<TValue>& insert_before(Node<TValue>& next, const TValue& value) {
            if (&next == m_head) {
                return insert_at_head(value);
            }
            Node<TValue>* new_node = m_allocator.allocate(1);
			std::construct_at(new_node, this, next.previous, &next, std::move(value));
            m_length++;
            return *new_node;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_at_tail(TArgs&&... args) {
            if (m_length == 0) {
                std::construct_at(m_tail, this, nullptr, nullptr, std::forward<TArgs>(args)...);
                m_length++;
                return *m_tail;
            }

            Node<TValue>* tail = m_allocator.allocate(1);
            std::construct_at(tail, this, m_tail, nullptr, std::forward<TArgs>(args)...);
            m_tail = tail;
            m_length++;

            return *m_tail;
        }

        Node<TValue>& insert_at_tail(const TValue& value) {
            if (m_length == 0) {
                std::construct_at(m_tail, this, nullptr, nullptr, value);
                m_length++;
                return *m_tail;
            }

            Node<TValue>* tail = m_allocator.allocate(1);
            std::construct_at(tail, this, m_tail, nullptr, std::move(value));
            m_tail = tail;
            m_length++;

            return *m_tail;
        }

        template<typename... TArgs>
        Node<TValue>& emplace_at_head(TArgs&&... args) {
            if (m_length == 0) {
                std::construct_at(m_head, this, nullptr, nullptr, std::forward<TArgs>(args)...);
                m_length++;
                return *m_head;
            }

            Node<TValue>* head = m_allocator.allocate(1);
            std::construct_at(head, this, nullptr, m_head, std::forward<TArgs>(args)...);
            m_head = head;
            m_length++;

            return *m_head;
        }

        Node<TValue>& insert_at_head(const TValue& value) {
            if (m_length == 0) {
                std::construct_at(m_head, this, nullptr, nullptr, value);
                m_length++;
                return *m_head;
            }

            Node<TValue>* head = m_allocator.allocate(1);
            std::construct_at(head, this, nullptr, m_head, value);
            m_head = head;

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
            if (m_length == 1) {
                std::destroy_at(m_tail);
                m_length = 0;
                return m_tail;
            }

            Node<TValue>* new_tail = m_tail->previous;
            std::destroy_at(m_tail);
			m_allocator.deallocate(m_tail, 1);

            m_tail = new_tail;
            if (m_length == 2) {
                m_head = m_tail;
            }
            m_length--;

            return m_tail;
        }

        Node<TValue>* pop_head() {
            assert(m_length);

            if (m_length == 1) {
                std::destroy_at(m_head);
                m_length = 0;
                return m_head;
            }

            Node<TValue>* new_head = m_head->next;
            std::destroy_at(m_head);
			m_allocator.deallocate(m_head, 1);

            m_head = new_head;
            if (m_length == 2) {
                m_tail = m_head;
            }
            m_length--;

            return m_head;
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

        Iterator begin() {
            return Iterator(m_head);
        }

        Iterator end() {
            return Iterator(nullptr);
        }

        ConstIterator begin() const {
            return ConstIterator(m_head);
        }

        ConstIterator end() const {
            return ConstIterator(nullptr);
        }

        ReverseIterator rbegin() {
            return ReverseIterator(m_tail);
        }

        ReverseIterator rend() {
            return ReverseIterator(nullptr);
        }

        ConstReverseIterator crbegin() {
            return ConstReverseIterator(m_tail);
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(nullptr);
        }

        DoublyLinkedList(const DoublyLinkedList<TValue, TAllocator>&) = delete;
        DoublyLinkedList& operator= (const DoublyLinkedList<TValue, TAllocator>&) = default;

    private:
        Node<TValue>* m_head;
        Node<TValue>* m_tail;
        std::uint32_t m_length;
        TAllocator m_allocator;
    
        friend class Node<TValue>;
    };
}
