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
         iterator t(this->_node);
         this->_node = this->_node->_next;
         return t;
      }
      iterator& operator -- () {
         _node = _node->_prev;
         return *this;
      }
      iterator operator -- (int) {
         iterator t(this->_node);
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
      iterator n(_head);
      return n;
   }
   iterator end() const {
      iterator n(_head->_prev);
      return n;
   }
   bool empty() const { 
      if(_head->_prev == _head && _head->_next == _head) return true;
      return false;
   }
   size_t size() const {
      size_t count = 0;
      if(empty()) return count;
      DList<T>::iterator n = begin();
      for(; n != end(); ++n){
         count++;
      }
      return count;
   }

   void push_back(const T& x) {
      _isSorted = false;
      if(empty()){
         DListNode<T>* n = new DListNode<T>(x, _head->_prev, _head->_next);
         _head->_prev = n;
         _head->_next = n;
         _head = n;
      }else{
         DListNode<T>* dummy = _head->_prev;
         DListNode<T>* n = new DListNode<T>(x, dummy->_prev, dummy);
         dummy->_prev->_next = n;
         dummy->_prev = n;
      }
   }
   void pop_front() {
      if(!empty()){
         DListNode<T>* dummy = _head->_prev;
         DListNode<T>* d = _head;
         if(dummy->_prev == _head){
            _head = dummy;
            dummy->_prev = dummy;
            dummy->_next = dummy;
            delete d;
            d = NULL;
         }
         else{
            dummy->_next = dummy->_next->_next;
            dummy->_next->_prev = dummy;
            _head = dummy->_next;
            delete d;
            d = NULL;
         }
      }
   }
   void pop_back() {
      if(!empty()){
         DListNode<T>* dummy = _head->_prev;
         DListNode<T>* d = dummy->_prev;
         if(d == _head){
            _head = dummy;
            dummy->_prev = dummy;
            dummy->_next = dummy;
            delete d;
            d = NULL;
         }
         else{
            dummy->_prev = dummy->_prev->_prev;
            dummy->_prev->_next = dummy;
            delete d;
            d = NULL;
         }
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(empty()) return false;
      DList<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(n == pos) {
            if(n._node == _head){
               DListNode<T>* d = _head;
               _head->_prev->_next = _head->_next;
               _head->_next->_prev = _head->_prev;
               _head = _head->_next;
               delete d;
               d = NULL;
            }else{
               n._node->_prev->_next = n._node->_next;
               n._node->_next->_prev = n._node->_prev;
               delete n._node;
            }
            return true;
         }
      }
      return false; 
   }
   bool erase(const T& x) { 
      if(empty()) return false;
      DList<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(*n == x) {
            if(n._node == _head){
               DListNode<T>* d = _head;
               _head->_prev->_next = _head->_next;
               _head->_next->_prev = _head->_prev;
               _head = _head->_next;
               delete d;
               d = NULL;
            }else{   n._node->_prev->_next = n._node->_next;
               n._node->_next->_prev = n._node->_prev;
               delete n._node;
               }
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
         pop_back();
      }
   }  // delete all nodes except for the dummy node

   //Insertion Sort O(n^2)
   void sort() const {
      if(_isSorted == true) return;
      DList<T>::iterator check, li = begin();
      _isSorted = true;

      for(; li != end(); ++li){
         for(check = begin(); check != li; ++check){
            if(*li < *check){
               T temp = *check;
               *check = *li;
               *li = temp;
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
