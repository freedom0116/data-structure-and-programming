/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data.front(); }
   void insert(const Data& d) {
      int t = size(), p;
      _data.push_back(d);
      while(t > 0){
         p = (t-1) / 2;
         if(d > _data[p]) break;
         _data[t] = _data[p];
         t = p;
      }
      _data[t] = d;
   }
   void delMin() {
      int i = 0, c = 1;
      while(c < size()){
         if(_data[i] == _data[c+1]) { i = c + 1; }
         else if(_data[i] == _data[c]) { i = c; }
         else break;
         c = i*2 + 1;
      }
      delData(i);
   }
   void delData(size_t i) { 
      int t = i, p, c = t*2 + 1;
      Data tempData = _data.back();
      _data.pop_back();
      while(t > 0){
         p = (t-1) / 2;
         if(tempData > _data[p]) break;
         _data[t] = _data[p];
         t = p;
      }
      while(c < size()){
         if(c+1 < size())
            if(_data[c] > _data[c+1]) 
               c++; // find smaller child
         if(tempData < _data[c]) break;
         _data[t] = _data[c];
         t = c;
         c = t*2 + 1;
      }
      _data[t] = tempData;
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
