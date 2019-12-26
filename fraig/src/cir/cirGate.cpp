/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
size_t CirGate::_globalRef = 0;

void printDivider();
string getIdDate(const CirGate*, string);
bool findMyInv(int, CirGate*);
void printSpace(int);

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::dfsTraversal(GateList& dfsList)
{  
   CirGate* next; 
   for(int i = 0, s = _faninList.size(); i < s; i++){
      next = _faninList[i].getPin();
      if(!next->isGlobalRef()){
         next->setToGlobalRef();
         next->dfsTraversal(dfsList);
      }
   }
   if(typeid(*this) != typeid(UnDef)) dfsList.push_back(this);
}

void
CirGate::reportGate() const{ printGate(); }

void
CirGate::reportFanin(int level) const
{  
   assert (level >= 0);
   int dist = level;
   setGlobalRef();
   if(this->getInvPhase()) cout << "!";
   this->faninTraversal(level, dist);
}

void 
CirGate::faninTraversal(const int& level, int dist) const
{
   CirGate* next;
   this->printPin();
   dist--;
   if(dist >= 0){
      if(!this->isGlobalRef()){
         this->setToGlobalRef();
         cout << endl;
         if(this->getFanin().size() != 0){   
            for(int i = 0, s = _faninList.size(); i < s; i++){
               next = _faninList[i].getPin();
               printSpace(level - dist);
               if(_faninList[i].getInvPhase() == true) cout << "!";
               next->faninTraversal(level, dist);
            }
         }
      }else{ (this->getFanin().size() != 0)? cout << " (*)" << endl : cout << endl; }
   }else { cout << endl; }
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   int dist = level;
   setGlobalRef();
   if(this->getInvPhase()) cout << "!";
   this->fanoutTraversal(level, dist);
}

void 
CirGate::fanoutTraversal(const int& level, int dist) const
{
   CirGate* prev;
   this->printPin();
   dist--;
   if(dist >= 0){
      if(!this->isGlobalRef()){
         this->setToGlobalRef();
         cout << endl;
         if(this->getFanout().size() != 0){
            for(int i = 0, s = _fanoutList.size(); i < s; i++){
               prev = _fanoutList[i].getPin();
               printSpace(level - dist);
               if(findMyInv(this->getGateID(), prev) == true) cout << "!";
               prev->fanoutTraversal(level, dist);
            }
         }
      }else{ (this->getFanout().size() != 0)? cout << " (*)" << endl : cout << endl; }
   }else{ cout << endl; }
}

bool findMyInv(int ID, CirGate* prev)
{
   vector<Pin> fanin = prev->getFanin();
   for(int i = 0, s = fanin.size(); i < s; i++){
      if(fanin[i].getPin()->getGateID() == ID) 
         return fanin[i].getInvPhase();
   }
   return false;
}

void printSpace(int n)
{  
   for(int i = 0; i < n; i++)
      cout << "  ";
}

void CirPiGate::printPin() const{ cout << "PI " << this->getGateID(); }
void CirPoGate::printPin() const{ cout << "PO " << this->getGateID(); }
void AndGate::printPin() const{ cout << "AIG " << this->getGateID(); }
void UnDef::printPin() const{ cout << "UNDEF " << this->getGateID(); }
void Const0::printPin() const{ cout << "CONST " << this->getGateID(); }

void CirPiGate::printGate() const
{
   string data = getIdDate(this, "PI");
   printDivider();
   cout << left << setw(49) << data << "=" << endl;
   printDivider();
}
void CirPoGate::printGate() const
{
   string data = getIdDate(this, "PO");
   printDivider();
   cout << left << setw(49) << data << "=" << endl;
   printDivider();
}
void AndGate::printGate() const
{
   string data = getIdDate(this, "AIG");
   printDivider();
   cout << left << setw(49) << data << "=" << endl;
   printDivider();
}
void UnDef::printGate() const
{
   string data = getIdDate(this, "UNDEF");
   printDivider();
   cout << left << setw(49) << data << "=" << endl;
   printDivider();
}
void Const0::printGate() const
{
   string data = getIdDate(this, "const");
   printDivider();
   cout << left << setw(49) << data << "=" << endl;
   printDivider();
}

void printDivider()
{
   for(int i = 0; i < 50; i++)
      cout << "=";
   cout << endl;
}

string getIdDate(const CirGate* pin, string gateName)
{
   string output;
   output = "= " + gateName + "(" + to_string(pin->getGateID()) + ")";
   if(pin->getSymbols() != "")
      output = output + "\"" + pin->getSymbols() + "\"";
   output = output + ", line " + to_string(pin->getLineNo());
   return output;
}

void
CirGate::adjustFanoutState(CirGate* newPin)
{
   bool allZerofanin;
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      allZerofanin = true;
      CirGate* fanout = _fanoutList[i].getPin();
      vector<Pin>::iterator it = fanout->_faninList.begin();
      for(; it != fanout->_faninList.end(); it++){
         if((*it).getPin() == this){
            (*it).setPin(newPin);
            (*it).setPhase(false);
         }
         if((*it).getPin() != newPin) allZerofanin = false;
      }
      if(allZerofanin == true) fanout->adjustFanoutState(newPin);
   }
   newPin->setFanout(this, false);
}

void
CirGate::adjustFaninState(CirGate* newPin)
{
   for(int i = 0, s = _faninList.size(); i < s; i++){
      CirGate* fanin = _faninList[i].getPin();
      if(fanin != newPin){
         vector<Pin>::iterator it = fanin->_fanoutList.begin();
         for(; it != fanin->_fanoutList.end(); ){
            if((*it).getPin() == this) fanin->_fanoutList.erase(it);
            else it++;
         }
         _faninList[i].setPin(newPin);
         _faninList[i].setPhase(false);
      }
   }
}