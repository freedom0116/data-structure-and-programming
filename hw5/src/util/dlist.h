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
      iterator(DListNode<T>* n= 0): _node(n) {}
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
         DListNode<T>* t = _node;
         _node = _node->_next;
         return t;
      }
      iterator& operator -- () {
         _node = _node->_prev;
         return *this;
      }
      iterator operator -- (int) {
         DListNode<T>* t = _node;
         _node = _node->_prev;
         return t;
      }

      iterator& operator = (const iterator& i) {
         if(this != &i) this = &i;
         return *this;
      }

      bool operator != (const iterator& i) const {
         if(this != &i) return true;
         return false;
      }
      bool operator == (const iterator& i) const {
         if(this == &i) return true;
         return false;
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return _head; }
   iterator end() const { return _head->_prev; }
   bool empty() const { 
      if(_head->_prev == _head->_next) return true;
      return false; 
   }
   size_t size() const {  
      size_t count = 0;
      for(DList<T>::iterator n = begin(); n != end(); ++n){
         count++;
      }
      return count; 
   }

   void push_back(const T& x) {
      DListNode<T>* dummy = _head->_prev;
      DListNode<T>* n = new DListNode<T>(x, dummy->_prev, dummy);
      dummy->_prev = n;
      dummy->_prev->_next = n;
   }
   void pop_front() {
      if(!empty()){
         DListNode<T>* d = _head;
         _head->_next = _head->_prev;
         _head->_prev = _head->_next;
         _head = _head->_next;
         delete d;
         d = NULL;
      }
   }
   void pop_back() {
      if(!empty()){
         DListNode<T>* d = _head->_prev->_prev;
         d->_prev = d->_next;
         d->_next = d->_prev;
         delete d;
         d = NULL;
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      for(DList<T>::iterator n = begin(); n != end(); ++n){
         if(n == pos) return true;
      }
      return false; 
   }
   bool erase(const T& x) { 
      for(DList<T>::iterator n = begin(); n != end(); ++n){
         if(n._node->_data == x) return true;
      }
      return false; 
   }

   iterator find(const T& x) { 
      for(DList<T>::iterator n = begin(); n != end(); ++n){
         if(n._node->_data == x) return n;
      }
      return end();
   }

   void clear() {
      for(size_t i = 0, s = size(); i < s; i++){
         pop_front();
      }
   }  // delete all nodes except for the dummy node

   void sort() const { }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
