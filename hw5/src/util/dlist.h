/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n = 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         _node = _node->_next;
         return *this;
      }
      iterator operator ++ (int) {
         iterator t = *this;
         this->_node = this->_node->_next;
         return t;
      }
      iterator& operator -- () {
         _node = _node->_prev;
         return *this;
      }
      iterator operator -- (int) {
         iterator t = *this;
         this->_node = this->_node->_prev;
         return t;
      }

      iterator& operator = (const iterator& i) {
         if(_node != i._node) _node = i._node;
         return *this;
      }

      bool operator != (const iterator& i) const {
         if(_node != i._node) return true;
         return false;
      }
      bool operator == (const iterator& i) const {
         if(_node == i._node) return true;
         return false;
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {
      iterator n(_head->_next);
      return n;
   }
   iterator end() const {
      iterator n(_head);
      return n;
   }
   bool empty() const { 
      if(_head->_prev == _head && _head->_next == _head) return true;
      return false;
   }
   size_t size() const {  
      size_t count = 0;
      DList<T>::iterator n = begin();
      for(; n != end(); ++n){
         count++;
      }
      return count;
   }

   void push_back(const T& x) {
      _isSorted = false;
      DListNode<T>* n = new DListNode<T>(x, _head->_prev, _head);
      _head->_prev->_next = n;
      _head->_prev = n;
   }
   void pop_front() {
      if(!empty()){
         DListNode<T>* d = _head->_next;
         _head->_next = _head->_next->_next;
         _head->_next->_prev = _head;
         delete d;
         d = NULL;
      }
   }
   void pop_back() {
      if(!empty()){
         DListNode<T>* d = _head->_prev;
         _head->_prev = _head->_prev->_prev;
         _head->_prev->_next = _head;
         delete d;
         d = NULL;
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      DList<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(n == pos) {
            n._node->_prev->_next = n._node->_next;
            n._node->_next->_prev = n._node->_prev;
            delete n._node;
            return true;
         }
      }
      return false; 
   }
   bool erase(const T& x) { 
      DList<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(*n == x) {
            n._node->_prev->_next = n._node->_next;
            n._node->_next->_prev = n._node->_prev;
            delete n._node;
            return true;
         }
      }
      return false; 
   }

   iterator find(const T& x) {
      DList<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(*n == x) return n;
      }
      return end();
   }

   void clear() {
      for(size_t i = 0, s = size(); i < s; i++){
         pop_front();
      }
   }  // delete all nodes except for the dummy node

   void sort() const {
      if(_isSorted == true) return;
      DList<T>::iterator n = begin(), check, li = begin();
      _isSorted = true;
      while(li != end()){
         n = li++;
         for(check = begin(); check != n; ++check){
            if(*n < *check){
               // origin n neighbor
               n._node->_prev->_next = n._node->_next;
               n._node->_next->_prev = n._node->_prev;
               // n new pos
               n._node->_next = check._node;
               n._node->_prev = check._node->_prev;
               // n new neighbor
               check._node->_prev->_next = n._node;
               check._node->_prev = n._node;
               break;
            }
         }
      }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
