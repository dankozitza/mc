//
// Q.hpp
//
// Linked queue implementation using templates.
//

#ifndef _Q
#define _Q

#include <cassert>
#include <cstdlib>
#include <ostream>

template<class T>
class Q;

template<class T>
ostream& operator<<(ostream& out, Q<T>& q);

template<class T>
class Q {
   private:
      struct Node {
         T item;
         Node* next;
      };
      Node*  first;
      Node*  last;
      size_t q_size;

      Node* get_node(const T& item, Node* next);

   public:
      Q();
      Q(const Q& src);
      void operator=(const Q& src);
      void init(const Q& src);
      ~Q();
      void clear();
      void enq(const T& item);
      T deq();
      size_t size();

      friend ostream& operator<<<T>(ostream& out, Q<T>& q);
};

//
// The template implementation is here so that this code will be generated and
// compiled with the client code. Also there will be no Q.cpp file to be
// mistakenly compiled and linked.
//

template<class T>
Q<T>::Q() {
   first  = NULL;
   last   = NULL;
   q_size = 0;
}

template<class T>
Q<T>::Q(const Q& src) {
   init(src);
}

template<class T>
void Q<T>::operator=(const Q& src) {
   if (this == &src)
      return;
   clear();
   init(src);
}

template<class T>
void Q<T>::init(const Q& src) {
   Node *p;
   q_size = 0;

   if (src.first == NULL) {
      first = NULL;
      last  = NULL;
   }
   else {
      first = get_node(src.first->item, NULL);
      q_size++;
      last = first;
      p = src.first->next;
      while (p != NULL) {
         last->next = get_node(p->item, NULL);
         last = last->next;
         q_size++;
         p = p->next;
      }
   }
}

template<class T>
Q<T>::~Q() {
   clear();
}

template<class T>
void Q<T>::clear() {
   Node* tmp;
   while (first != NULL) {
      tmp = first;
      first = first->next;
      delete tmp;
   }
   last = NULL;
   q_size = 0;
}

template<class T>
void Q<T>::enq(const T& item) {
   if (last == NULL) {
      first = get_node(item, NULL);
      last = first;
   }
   else {
      last->next = get_node(item, NULL);
      last = last->next;
   }
   q_size++;
}

template<class T>
T Q<T>::deq() {
   assert(first != NULL);

   T tmp_item = first->item;
   Node* tmp_node = first;

   first = first->next;
   if (first == NULL)
      last = NULL;

   delete tmp_node;

   q_size--;
   return tmp_item;
}

template<class T>
size_t Q<T>::size() {
   return q_size;
}

template<class T>
typename Q<T>::Node* Q<T>::get_node(const T& item, Node* next) {
   Node* p = new Node;
   p->item = item;
   p->next = next;
   return p;
}

template<class T>
ostream& operator<<(ostream& out, Q<T>& q) {
   typename Q<T>::Node* tmp = q.first;
   if (tmp != NULL) {
      while (tmp->next != NULL) {
         cout << tmp->item << " ";
         tmp = tmp->next;
      }
      cout << tmp->item;
   }
   return out;
}

#endif
