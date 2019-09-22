/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include <algorithm>
#include <sstream>

using namespace std;

// Implement member functions of class Row and Table here
string 
JsonElem::getKey(){
   string k = _key;
   return k;
}
int 
JsonElem::getValue(){
   int v = _value;
   return v;
}
bool
Json::read(const string& jsonFile)
{
   fstream inputfile;
   string a;
   vector<string> content;
   inputfile.open(jsonFile);
   while(inputfile >> a){
      if(a != "{" && a != "}"){
         content.push_back(a);
      }
   }
   string tempkey;
   int num = 0;
   char sub1 = '\"', sub2 = ':', sub3 = ',';
   int contentsize = content.size();
   for(int i = 0; i < contentsize; i++){
      if(deletesub(content[i], sub1, content[i].length()) == true){
         tempkey = content[i];
      }else if(deletesub(content[i], sub2, content[i].length()) == true){
         num++;
      }else if(num == 1){
         deletesub(content[i], sub3, content[i].length());
         int intvalue = stoi(content[i]);
         JsonElem Jtemp(tempkey, intvalue);
         _obj.push_back(Jtemp);
         num--;
      }
   }
   return true; // TODO
}
bool
Json::deletesub(string& word, char& sub, int size)
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
void
Json::print()
{
   cout << "{" << endl;
   for(int i = 0; i < _obj.size(); i++){
      int objsize = _obj.size();
      cout << "  " << _obj[i];
      if(i < objsize - 1){
         cout << ","<< endl;
      }else{
         cout << endl;
      }
   }
   cout << "}" << endl;
}
void 
Json::sum()
{
   int total = 0;
   for(int i = 0; i < _obj.size(); i++){
      total += _obj[i].getValue();
   }
   cout << "The summation of the values is:" << total << "." <<endl;
}
void 
Json::avg()
{
   float total = 0;
   for(int i = 0; i < _obj.size(); i++){
      total += _obj[i].getValue();
   }
   float average = total/_obj.size();
   cout << "The average of the values is:";
   cout << fixed << setprecision(1) << average << "." <<endl;
}
void 
Json::max()
{
   int maxone = 0, maxvalue = _obj[0].getValue();
   for(int i = 0; i < _obj.size(); i++){
      if(_obj[i].getValue() > maxvalue){
         maxone = i;
      }
   }
   cout << "The maximum element is: {" << _obj[maxone] << "}." <<endl;
}
void 
Json::min()
{
   int minone = 0, minvalue = _obj[0].getValue();
   for(int i = 0; i < _obj.size(); i++){
      if(_obj[i].getValue() < minvalue){
         minone = i;
      }
   }
   cout << "The maximum element is: {" << _obj[minone] << "}." <<endl;
}
void 
Json::add(string& key, string& value)
{
   istringstream ss(value);
   int intvalue;
   ss >> intvalue;
   JsonElem newelem(key, intvalue);
   _obj.push_back(newelem);
}
ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

