/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>
#include <math.h>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) { _isSorted = false;}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n = 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () {
         _node += 1;
         return (*this);
      }
      iterator operator ++ (int) { 
         iterator t = *this;
         _node += 1;
         return t;
      }
      iterator& operator -- () {
         _node -= 1;
         return (*this);
      }
      iterator operator -- (int) {
         iterator t = *this;
         _node -= 1;
         return t;
      }

      iterator operator + (int i) const {         
         iterator t = _node + i;
         return t; 
      }
      iterator& operator += (int i) { 
         if(_node != (_node + i)) _node += 1;
         return *this; 
      }

      iterator& operator = (const iterator& i) {
         if(_node != i._node) _node = i._node;
         return (*this);
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
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const {
      iterator n(_data);
      return n;
   }
   iterator end() const {
      iterator n(_data + size());
      return n;
   }
   bool empty() const {
      if(size() == 0) return true;
      return false; 
   }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return *(_data + i); }
   const T& operator [] (size_t i) const { return *(_data + i); }

   void push_back(const T& x) {
      size_t newsize = (size() + 1), count = 0;
      if(newsize > _capacity){
         while(_capacity != 0){
            _capacity = _capacity / 2;
            count++;
         }
         _capacity = pow(2, count);
         // Create new array
         T* newArray = new T[_capacity];
         for(int i = 0, s = size(); i < s; i++){
            newArray[i] = *(_data + i);
         }
         newArray[size()] = x;
         if(!empty()){
            T* temp = _data;
            _data = newArray;
            delete[] temp;
            temp = NULL;
         }else 
            _data = newArray;
      }else{
         *(_data + size()) = x;
      }
      _size = newsize;
      _isSorted = false;
   }
   void pop_front() {
      if(!empty()){
         *(_data) = *(_data + size() - 1);
         _size -= 1;
      }
   }
   void pop_back() { if(!empty()) _size -= 1; }

   bool erase(iterator pos) {
      Array<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(n == pos){
            *n = *(_data + size() - 1); 
            _size -= 1;
            return true;
         }
      }
      return false;
   }
   bool erase(const T& x) {
      Array<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(*n == x){
            *n = *(_data + size() - 1); 
            _size -= 1;
            return true;
         }
      }
      return false;
   }

   iterator find(const T& x) {
      Array<T>::iterator n = begin();
      for(; n != end(); ++n){
         if(*n == x) return n;
      }
      return end();
   }

   void clear() { _size = 0; }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
