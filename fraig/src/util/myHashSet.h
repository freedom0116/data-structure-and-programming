/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* s = 0, size_t num = 0, size_t p = 0, size_t node = 0)
               : _slot(s), _bucketSize(num), _pos(p), _nodeNum(node) {}
      iterator(const iterator& i): _slot(i._slot), _nodeNum(i._nodeNum) {}
      ~iterator() {}

      const Data& operator * () const { return (*(_slot+_pos))[_nodeNum]; }
      iterator& operator ++ () {
         if(_nodeNum+1 < (_slot+_pos)->size()) { _nodeNum++; }
         else{ 
            do{
               if(++_pos == _bucketSize) break;
            }while((_slot+_pos)->size() == 0);
            _nodeNum = 0;
         }
         return (*this); 
      }
      bool operator == (const iterator& i) const { 
         if(_pos == i._pos && _nodeNum == i._nodeNum) return true;
         return false;
      }
      bool operator != (const iterator& i) const { 
         if(_pos != i._pos || _nodeNum != i._nodeNum) return true;
         return false;
      }
   private:
      vector<Data>* _slot;
      size_t _bucketSize, _pos, _nodeNum;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      if(empty()) return end();
      int pos = 0;
      while((_buckets+pos  )->size() == 0){ pos++; }
      return iterator(_buckets, numBuckets(), pos); 
   }
   // Pass the end
   iterator end() const {
      if(empty()) return iterator(_buckets);
      return iterator(_buckets, numBuckets(), numBuckets());
   }
   // return true if no valid data
   bool empty() const { return size() == 0; }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for(size_t i = 0; i < _numBuckets; i++){
         s += (_buckets+i)->size();
      }
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      int num = bucketNum(d);
      vector<Data> slot = *(_buckets+num);
      for(int i = 0, s = slot.size(); i < s; i++){
         if(d == slot[i]) return true;
      }
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      int num = bucketNum(d);
      vector<Data> slot = *(_buckets+num);
      for(int i = 0, s = slot.size(); i < s; i++){
         if(d == slot[i]){
            d = slot[i];
            return true;
         }
      }
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      int num = bucketNum(d);
      for(int i = 0, s = (_buckets+num)->size(); i < s; i++){
         if(d == (*(_buckets+num))[i]){
            (*(_buckets+num))[i] = d;
            return true;
         }
      }
      (_buckets+num)->push_back(d);
      return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      if(check(d)) return false;
      int num = bucketNum(d);
      (_buckets+num)->push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      int num = bucketNum(d);
      vector<Data> slot = *(_buckets+num);
      for(int i = 0, s = slot.size(); i < s; i++){
         if(d == slot[i]){
            slot.erase(slot.begin()+i);
            return true;
         }
      }
      return false; 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
