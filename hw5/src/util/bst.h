/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* t = 0, BSTreeNode<T>* r = 0,
   BSTreeNode<T>* l = 0): _data(d), _top(t), _right(r), _left(l) {}

   T _data;
   BSTreeNode<T>* _top;
   BSTreeNode<T>* _right;
   BSTreeNode<T>* _left;
};

template <class N, class S>
class List
{
public:
   List() {}
   ~List() {}

   void push_back(N n, S s){
      _nodelist.push_back(n);
      _statelist.push_back(s);
   }
   void pop_back(){
      _nodelist.pop_back();
      _statelist.pop_back();      
   }

   int size(){ return _nodelist.size(); }

   bool empty(){
      if(this->size() == 0) return true;
      else return false;
   }

   N get_node(int n = 0){ return _nodelist[n]; }
   S get_state(int n = 0){ return _statelist[n]; }

private:
   vector<N> _nodelist; 
   vector<S> _statelist; // only 2 state: left, top
};

template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(): _root(0), _size(0) {
      _BSend = new BSTreeNode<T>(T());
   }
   ~BSTree() {clear(); delete _BSend; }

   class iterator
   { 
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* n = 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {}

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         if(list.empty()){
            if(_node->_top == _node){
               _node = _node->_right;
            } // _root
            else{
               if(_node->_right == 0){
                  _node = _node->_top;
               }else{
                  list.push_back(_node->_top);
                  _node = localmin(_node->_right);
               }
            }
         }else{
            _node = list[list.size() - 1]; // return this one
            list.pop_back();
         }
         
         if(_node->_top != _node){
            BSTreeNode<T>* t = _node->_top;
            if(t->_right != _node) list.push_back(_node->_top);
         }
         if(_node->_right != 0){
            BSTreeNode<T>* i;
            i = localmin(_node->_right);
            list.push_back(i);
         }
         return *this;
      } // move to next
      iterator operator ++ (int) {
         iterator t(this->_node), n(this->_node);
         ++n;
         this->_node = n._node;
         return t;
      }
      iterator& operator -- () {
         while(_node->_top != 0){
            _node = _node->_top;
         }
         while(_node->_right != 0){
            _node = _node->_right;
         }
         _node = _node->_top;
         return *this;
      } // move to end (rightmost)
      iterator operator -- (int) {
         iterator t(this->_node), n(this->_node);
         --n;
         this->_node = n._node;
         return t;
      }

      iterator operator = (const iterator& i) {
         if(_node != (i._node)) _node = i._node;
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
      BSTreeNode<T>* _node; // store init node
      vector<BSTreeNode<T>*> list; // state: node come from      

      BSTreeNode<T>* localmin(BSTreeNode<T>* n){
         while(n->_left != 0)
            n = n->_left;
         return n;
      }
   };

   iterator begin() const {
      if(empty()) return end();
      BSTreeNode<T>* h = head();
      iterator n(h);
      return n;
   }
   iterator end() const {
      iterator e(_BSend);
      return e;
   }
   bool empty() const {
      if(_root == 0) return true;
      return false;
   }
   size_t size() const { return _size; }

   void insert(const T& x) {
      BSTreeNode<T>* newnode = new BSTreeNode<T>(x);
      if(empty()){
         _root = newnode;
         newnode->_top = _root;
      }else{
         int direction;
         BSTreeNode<T>* i = _root;
         while(true){
            direction = compare(i->_data, x);
            if(direction == -1){
               if(i->_left == 0) {
                  i->_left = newnode;
                  newnode->_top = i;
                  break;
               }else i = i->_left;
            }
            else{
               if(i->_right == 0 || i->_right == _BSend){
                  i->_right = newnode;
                  newnode->_top = i;
                  break;
               }else i = i->_right;
            }
         }
      }
      setend();
      _size += 1;
   }
   void pop_front() {
      if(empty()) return;
      if(size() == 1){
         delete _root;
         _root = NULL;
      }else{
         BSTreeNode<T>* d = head();
         BSTreeNode<T>* head = d;
         if(d == _root){
            _root = _root->_right;
            _root->_top = _root;
         }else{
            if(head->_right == 0){
               head = head->_top;
               head->_left = 0;
            }else{
               head = head->_right;
               d->_top->_left = head;
               head->_top = d->_top;
            }
         }
         delete d;
      }
      setend();
      _size -= 1;
   }
   void pop_back() {
      if(empty()) return;
      if(size() == 1){
         delete _root;
         _root = NULL;         
      }else{
         BSTreeNode<T>* d = _BSend->_top;
         BSTreeNode<T>* tail = d;
         if(tail == _root) {
            _root = _root->_left;
            _root->_top = _root;
         }else{
            if(tail->_left == 0){
               tail = tail->_top;
               tail->_right = 0;
            }else{
               tail = tail->_left;
               d->_top->_right = tail;
               tail->_top = d->_top;
            }
         }
         delete d;  
      }
      setend();
      _size -= 1;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(pos._node == 0) return false;
      _size -= 1;
      if(size() == 1){
         delete _root;
         _root = NULL;
      }else
         repair(pos._node);
      setend();
      return true;
   }
   bool erase(const T& x) {
      iterator n;
      n = find(x);
      if(n._node == 0) return false;
      _size -= 1;
      if(size() == 1){
         delete _root;
         _root = NULL;
      }else
         repair(n._node);
      setend();
      return true;
   }

   iterator find(const T& x) {
      if(empty()) return iterator();
      BSTree<T>::iterator i = begin();
      for(; i != end(); ++i){
         if(*i == x) return i;
      }
      return iterator();
      // int direction;
      // BSTreeNode<T>* i = _root;
      // while(true){
      //    direction = compare(i->_data, x);
      //    if(direction == -1){
      //       if(i->_left != 0) i = i->_left;
      //       else return iterator(); 
      //    }
      //    else if(direction == 0){
      //       iterator n(i);
      //       i = NULL;
      //       return n;
      //    } // find
      //    else if(direction == 1){
      //       if(i->_right != 0) i = i->_right;
      //       else return iterator();
      //    }
      //    else{
      //       cerr << "Error: function compare() isn't correct" << endl;
      //       throw exception();
      //    }
      // }
   }

   void clear() {
      if(!empty()){
         BSTree<T>::iterator i = begin();
         for(; i != end(); i++){
            delete i._node;
         }
      }
   } // remember to do _head = NULL

   void print() const {
      // BSTree<T>::iterator i = begin();
      // int idx = 0;
      // if(!empty()){
      //    for(; i != end(); i++){
      //       cout << "[" << setw(3) << right << idx << "] = "
      //       << setw(3) << right << *i << "   ";
      //       if(idx % 4 == 3) cout << endl;
      //    }
      //    if(idx % 4 == 3) cout << endl;
      // }
   }

   void sort() const {} // useless

private:
   BSTreeNode<T>* _root;
   BSTreeNode<T>* _BSend;
   size_t _size;

   BSTreeNode<T>* head() const {
      if(empty()) return NULL;
      BSTreeNode<T>* h = _root;
      while(h->_left != 0){
         h = h->_left;
      }
      return h;
   }   

   int compare(T ref,T n){
      if(ref > n) return -1; // go left
      else if(ref == n) return 0; // same
      return 1; // go right
   }

   BSTreeNode<T>* findmin(BSTreeNode<T>* n){
      while(n->_left != 0){
         n = n->_left;
      }
      return n;
   }
   void repair(BSTreeNode<T>* n){
      BSTreeNode<T>* alter;
      BSTreeNode<T>* mintop;
      alter = n->_right;
      alter = findmin(alter);

      mintop = alter->_top;
      mintop->_left = 0;

      if(n == _root){
         _root = alter;
         _root->_top = _root;
      }else
         alter->_top = n->_top;
      alter->_left = n->_left;
      alter->_right = n->_right;
      delete n;
   }

   void setend(){
      if(empty()){
         _BSend->_top = NULL;
      }
      BSTreeNode<T>* i = _root;
      while(i->_right != 0){
         i = i->_right;
      }
      if(i != _BSend){
         _BSend->_top = i;
         i->_right = _BSend;
      }
   }
};

#endif // BST_H
