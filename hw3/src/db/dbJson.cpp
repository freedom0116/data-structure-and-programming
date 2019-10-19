/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string a, tempkey;
   int num = 0;
   char sub1 = '\"', sub2 = ':', sub3 = ',';
   while(is >> a){
      if(a != "{" && a != "}"){
         if(j.deletesub(a, sub1, a.length()) == true) tempkey = a;
         else if(j.deletesub(a, sub2, a.length()) == true) num++;
         else if(num == 1){
            j.deletesub(a, sub3, a.length());
            int intvalue = stoi(a);
            DBJsonElem Jtemp(tempkey, intvalue);
            j._obj.push_back(Jtemp);
            num--;
         }
      }
   }
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   cout << "{" << endl;
   for(int i = 0, n = j._obj.size(); i < n; i++){
      cout << "  " << j._obj[i];
      if(i < n - 1) cout << ","<< endl;
      else cout << endl;
   }
   cout << "}" << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   for(int i = 0, n = _obj.size(); i < n; i++) _obj.pop_back();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   for(int i = 0, n = _obj.size(); i < n; i++){
      if(_obj[i].key() == elm.key())
         return false;
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   float avg = NAN;
   if(_obj.empty())
      return avg;
   else{
      int total = 0;
      for(int i = 0, n = _obj.size(); i < n; i++){
         total += _obj[i].value();
      }
      avg = total/_obj.size();
      return avg;
   }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if(_obj.empty()){
      idx = _obj.size();
      return maxN;
   }else{
      for(size_t i = 0, n = _obj.size(); i < n; i++){
         if(_obj[i].value() > maxN){
            maxN = _obj[i].value();
            idx = i;
         }
      }
      return maxN;
   }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if(_obj.empty()){
      idx = _obj.size();
      return minN;
   }else{
      for(size_t i = 0, n = _obj.size(); i < n; i++){
         if(_obj[i].value() < minN){
            minN = _obj[i].value();
            idx = i;
         }
      }
      return minN;
   }
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   if(_obj.size() == 0) return s;
   else{
      for(int i = 0, n = _obj.size(); i < n; i++){
         s += _obj[i].value();
      }
      return s;
   }
}

bool
DBJson::deletesub(string& word, char& sub, int size)
{
   bool ans = false;
   for(int i = 0; i < size; i++){
      if(word[i] == sub){
         word.erase(i,1);
         ans = true;
      }
   }
   return ans;
}

