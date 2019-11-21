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
   ~BSTreeNode<T>() {}

   T _data;
   BSTreeNode<T>* _top;
   BSTreeNode<T>* _right;
   BSTreeNode<T>* _left;
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
               }else {
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
         iterator t(this->_node);
         if(list.empty()){
            if(_node->_top == _node){
               _node = _node->_right;
            } // _root
            else{
               if(_node->_right == 0){
                  _node = _node->_top;
               }else {
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
         }if(list.empty()){
            if(_node->_top == _node){
               _node = _node->_right;
            } // _root
            else{
               if(_node->_right == 0){
                  _node = _node->_top;
               }else {
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
         return t;
      }
      iterator& operator -- () {
         if(list.empty()){
            if(_node->_top == _node){
               if(_node->_left != 0) _node = _node->_left;
            } // _root
            else{
               if(_node->_left == 0){
                  _node = _node->_top;
               }else {
                  list.push_back(_node->_top);
                  _node = localmax(_node->_left);
               }
            }
         }else{
            _node = list[list.size() - 1]; // return this one
            list.pop_back();
         }

         if(_node->_top != _node){
            BSTreeNode<T>* t = _node->_top;
            if(t->_left != _node) list.push_back(_node->_top);
         }
         if(_node->_left != 0){
            BSTreeNode<T>* i;
            i = localmax(_node->_left);
            list.push_back(i);
         }
         return *this;
      } // move to end (rightmost)
      iterator operator -- (int) {
         iterator t(this->_node);
         if(list.empty()){
            if(_node->_top == _node){
               if(_node->_left != 0) _node = _node->_left;
            } // _root
            else{
               if(_node->_left == 0){
                  _node = _node->_top;
               }else {
                  list.push_back(_node->_top);
                  _node = localmax(_node->_left);
               }
            }
         }else{
            _node = list[list.size() - 1]; // return this one
            list.pop_back();
         }

         if(_node->_top != _node){
            BSTreeNode<T>* t = _node->_top;
            if(t->_left != _node) list.push_back(_node->_top);
         }
         if(_node->_left != 0){
            BSTreeNode<T>* i;
            i = localmax(_node->_left);
            list.push_back(i);
         }
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
         if(n->_left == 0) return n;
         else return localmin(n->_left);
      }
      BSTreeNode<T>* localmax(BSTreeNode<T>* n){
         if(n->_right == 0) return n;
         else return localmax(n->_right);
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
      if(size() == 0) return true;
      return false;
   }
   size_t size() const { return _size; }

   void insert(const T& x) {
      BSTreeNode<T>* newnode = new BSTreeNode<T>(x);
      if(empty()){
         _root = newnode;
         newnode->_top = _root;
      }else{
         BSTreeNode<T>* i = _root;
         BSTreeNode<T>* parent;
         while(i != 0 && i != _BSend){
            parent = i;
            if(newnode->_data < i->_data) i = i->_left;
            else i = i->_right;
         }
         newnode->_top = parent;
         if(newnode->_data < parent->_data) parent->_left = newnode;
         else parent->_right = newnode;
      }
      setend();
      _size += 1;
   }
   void pop_front() {
      if(empty()) return;
      if(size() == 1){
         delete _root;
      }else{
         BSTreeNode<T>* d = head();
         repair(d);
         delete d;
      }
      _size -= 1;
      setend();
   }
   void pop_back() {
      if(empty()) return;
      if(size() == 1){
         delete _root;
      }else{
         BSTreeNode<T>* d = _BSend->_top;
         repair(d);
         delete d;
      }
      _size -= 1;
      setend();
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      iterator d = find(*pos);
      if(d._node == 0) return false;
      if(size() == 1){
         delete _root;
      }else{
         repair(d._node);
         delete d._node;
      }
      _size -= 1;
      setend();
      return true;
   }
   bool erase(const T& x) {
      iterator d = find(x);
      d = find(x);
      if(d._node == 0) return false;
      if(size() == 1){
         delete _root;      
      }else{
         repair(d._node);
         delete d._node;
      }
      _size -= 1;
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
   }

   void clear() {
      if(!empty()){
         BSTree<T>::iterator i = begin();
         for(; i != end(); ++i){
            erase(i);
         }
         _size = 0;
         setend();
      }
   } // remember to do _head = NULL

   void print() const {
      BSTreeNode<T>* n = head();
      cout<<"head:"<<n->_data<<endl;
      cout<<"root:"<<_root->_data<<endl;
      cout<<"tail:"<<_BSend->_top->_data<<endl;
      cout<<"size:"<<size()<<endl;
   }

   void sort() const {} // useless

private:
   BSTreeNode<T>* _root;
   BSTreeNode<T>* _BSend;
   size_t _size;

   BSTreeNode<T>* head() const {
      BSTreeNode<T>* h = _root;
      while(h->_left != 0){
         h = h->_left;
      }
      return h;
   }

   bool repairRoot(BSTreeNode<T>* n){
      if(n != _root) return false;
      // cout<<"root"<<endl;

      int children = 0;
      if(n->_left != 0) children += 1;
      if(n->_right != 0 && n->_right != _BSend) children += 2;
      // cout<<"ch:"<<children<<endl;

      if(children == 1){
         _root = _root->_left;
         _root->_top = _root;
      }
      else if(children == 2){
         _root = _root->_right;
         _root->_top = _root;
      }
      else if(children == 3){
         BSTreeNode<T>* i = successor(n);
         if(n->_right == i){
            // cout<<"2"<<endl;
            _root = i;
            _root->_top = _root;
            _root->_left = n->_left;
            _root->_left->_top = _root;
         }else{
            // cout<<"3"<<endl;
            repair(i);
            _root = i;
            _root->_top = _root;
            _root->_left = n->_left;
            if(_root->_left != 0) _root->_left->_top = _root;
            _root->_right = n->_right;
            if(_root->_right != 0 && _root->_right != _BSend) _root->_right->_top = _root;
         }
      }
      return true;
   }
   void repair(BSTreeNode<T>* n){
      if(!repairRoot(n)){
         int children = 0;
         if(n->_left != 0) children += 1;
         if(n->_right != 0 && n->_right != _BSend) children += 2;
         // cout<<"ch:"<<children<<endl;

         if(children == 0) crossLink(n->_top, n, 0);
         else if(children == 1) crossLink(n->_top, n, n->_left);
         else if(children == 2) crossLink(n->_top, n, n->_right);
         else{
            BSTreeNode<T>* i = successor(n);

            if(n->_right == i){
               // cout<<"="<<endl;
               crossLink(n->_top, n, i);
               i->_left = n->_left;
               if(i->_left != 0) i->_left->_top = i;
            }else{
               // cout<<"!="<<endl;
               repair(i);
               crossLink(n->_top, n, i);
               i->_left = n->_left;
               if(i->_left != 0) i->_left->_top = i;
               i->_right = n->_right;
               if(i->_right != 0 && i->_right != _BSend) i->_right->_top = i;
            }
         }
      }
   }
   BSTreeNode<T>* successor(BSTreeNode<T>* n){ return findmin(n->_right); }
   BSTreeNode<T>* findmin(BSTreeNode<T>* n){
      if(n->_left == 0) return n;
      else return findmin(n->_left);
   }
   void crossLink(BSTreeNode<T>* top, BSTreeNode<T>* check, BSTreeNode<T>* child){
      if(child != 0){
         if(top->_left == check) top->_left = child;
         else if(top->_right == check) top->_right = child;
         child->_top = top;
      }else{
         if(top->_left == check) top->_left = 0;
         else if(top->_right == check) top->_right = 0;
      }
   }

   void setend(){
      if(empty()) _BSend->_top = NULL;
      BSTreeNode<T>* i = _root;
      while(i->_right != 0 && i->_right != _BSend){
         i = i->_right;
      }
      _BSend->_top = i;
      i->_right = _BSend;
   }
};

#endif // BST_H
