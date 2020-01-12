/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//

class HashKey
{
public:
   HashKey(vector<size_t> v){
      assert(v.size() == 2);
      if(v[0] < v[1]){ _in0 = v[0]; _in1 = v[1]; }
      else{_in0 = v[1]; _in1 = v[0]; }
   }

   size_t operator() () const { return (_in0 + _in1); }
   bool operator == (const HashKey& k) const {
      if(_in0 == k._in0 && _in1 == k._in1)
         return true;
      return false;
   }
private:
   size_t _in0, _in1;
};

class SimKey
{
public:
   SimKey(size_t p): _patt(p) {}

   size_t operator () () const { return _patt; }
   bool operator == (const SimKey& k) const { return (_patt == k._patt); }
private:
   size_t _patt;

};

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(vector<HashNode>* s = 0, size_t num = 0, size_t p = 0, size_t node = 0)
               : _slot(s), _bucketSize(num), _pos(p), _nodeNum(node) {}
      iterator(const iterator& i): _slot(i._slot), _nodeNum(i._nodeNum) {}
      ~iterator() {}

      HashNode& operator * () { return _slot[_pos][_nodeNum]; }
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
      vector<HashNode>* _slot;
      size_t _bucketSize, _pos, _nodeNum;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      if(empty()) return end();
      int pos = 0;
      while((_buckets+pos)->size() == 0){ pos++; }
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

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k, const HashData& d) const {
      for(int i = 0, s = _buckets[bucketNum(k)].size(); i < s; i++){
         if(k == _buckets[bucketNum(k)][i].first)
            if(d == _buckets[bucketNum(k)][i].second)
               return true;
      }
      return false;
   }

   // query if different HashData with same HashKey is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;   
   bool query(const HashKey& k, HashData& d, bool getd = false) const {
      for(int i = 0, s = _buckets[bucketNum(k)].size(); i < s; i++){
         if(k == _buckets[bucketNum(k)][i].first){
            if(d != _buckets[bucketNum(k)][i].second){
               if(getd) d = _buckets[bucketNum(k)][i].second;
               return true; }
         }
      }
      return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) { return false; }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
      if(check(k, d)) return false;
      _buckets[bucketNum(k)].push_back(HashNode(k, d));
      return true;
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) {
      for(int i = 0, s = _buckets[bucketNum(k)].size(); i < s; i++){
         if(_buckets[bucketNum(k)][i].first == k){
            _buckets[bucketNum(k)].erase(_buckets[bucketNum(k)].begin()+i);
            return true;
         }
      }
      return false;
   }

   void getKeyNodes(const HashKey& k, vector<HashData>& keyGrp){
      for(int i = 0, s = _buckets[bucketNum(k)].size(); i < s; i++){
         if(k == _buckets[bucketNum(k)][i].first){
            keyGrp.push_back(_buckets[bucketNum(k)][i].second);
         }
      }
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
