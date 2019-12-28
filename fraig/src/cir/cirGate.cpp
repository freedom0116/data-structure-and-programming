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

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static void printDivider()
{
   for(int i = 0; i < 50; i++)
      cout << "=";
   cout << endl;
}

static string getIdDate(const CirGate* pin, string gateName)
{
   string output;
   output = "= " + gateName + "(" + to_string(pin->getGateID()) + ")";
   if(pin->getSymbols() != "")
      output = output + "\"" + pin->getSymbols() + "\"";
   output = output + ", line " + to_string(pin->getLineNo());
   return output;
}
static  bool findMyInv(int ID, CirGate* prev)
{
   vector<Pin*> fanin = prev->getFanin();
   for(int i = 0, s = fanin.size(); i < s; i++){
      if(fanin[i]->getPin()->getGateID() == ID) 
         return fanin[i]->getInvPhase();
   }
   return false;
}

static void printSpace(int n)
{  
   for(int i = 0; i < n; i++)
      cout << "  ";
}

static bool isFanout(CirGate* gate, CirGate* Tout, int& it)
{
   for(int i = 0, s = gate->getFanout().size(); i < s; i++){
      if(Tout == gate->getFanout()[i]->getPin()){
         it = i; return true;
      }
   }
   return false;
}

static bool isFanin(CirGate* gate, CirGate* Tin, int& it)
{
   for(int i = 0, s = gate->getFanin().size(); i < s; i++){
      if(Tin == gate->getFanin()[i]->getPin()){
         it = i; return true;
      }
   }
   return false;
}

static string ifExclamation(bool b)
{
   if(b) return "!";
   return "";
}

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::dfsTraversal(GateList& dfsList)
{  
   CirGate* next; 
   for(int i = 0, s = _faninList.size(); i < s; i++){
      next = _faninList[i]->getPin();
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
               next = _faninList[i]->getPin();
               printSpace(level - dist);
               if(_faninList[i]->getInvPhase() == true) cout << "!";
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
               prev = _fanoutList[i]->getPin();
               printSpace(level - dist);
               if(findMyInv(this->getGateID(), prev) == true) cout << "!";
               prev->fanoutTraversal(level, dist);
            }
         }
      }else{ (this->getFanout().size() != 0)? cout << " (*)" << endl : cout << endl; }
   }else{ cout << endl; }
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

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
void
CirGate::sweep()
{
   int it;
   CirGate *fanout, *fanin;
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      fanout = _fanoutList[i]->getPin();
      if(isFanin(fanout, this, it)) fanout->removeFanin(it);
   }
   for(int i = 0, s = _faninList.size(); i < s; i++){
      fanin = _faninList[i]->getPin();
      if(isFanout(fanin, this, it)) fanin->removeFanout(it);
      
      // vector<Pin*>::iterator it = fanin->_fanoutList.begin();
      // for(; it != fanin->_fanoutList.end(); ){
      //    if((*it)->getPin() == this) fanin->removeFanout(it);
      //    else it++;
      // }
   }
}

void 
CirGate::faninOne(CirGate* one, CirGate* replace)
{
   int it;
   bool phase;
   CirGate* fanout;
   isFanin(this, replace, it);
   phase = _faninList[it]->getInvPhase();
   if(isFanout(one, this, it)) one->removeFanout(it);
   if(isFanout(replace, this, it)) replace->removeFanout(it);
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      fanout = _fanoutList[i]->getPin();
      if(isFanin(fanout, this, it)){
         fanout->getFanin()[it]->setPin(replace);
         fanout->getFanin()[it]->setPhase(phase);
         replace->setFanout(fanout);
      }
   }
   cout << "Simplifying: "<< replace->getGateID() << " merging " 
   << ifExclamation(phase) << _gateID << "..." << endl;
}

void 
CirGate::faninZero(CirGate* zero, CirGate* eliminate)
{  
   int it;
   CirGate* fanout;
   if(isFanout(zero, this, it)) zero->removeFanout(it);
   if(isFanout(eliminate, this, it)) eliminate->removeFanout(it);
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      fanout = _fanoutList[i]->getPin();
      if(isFanin(fanout, this, it)){
         fanout->getFanin()[it]->setPin(zero);
         fanout->getFanin()[it]->setPhase(false);
         zero->setFanout(fanout);
      }      
   }
}

void 
CirGate::sameFanin(CirGate* inGate)
{
   int it;
   bool phase;
   CirGate* fanout;
   isFanin(this, inGate, it);
   phase = _faninList[it]->getInvPhase();
   for(int i = 0; i < 2; i++)
      if(isFanout(inGate, this, it)) inGate->removeFanout(it);
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      fanout = _fanoutList[i]->getPin();
      if(isFanin(fanout, this, it)){
         fanout->getFanin()[it]->setPin(inGate);
         fanout->getFanin()[it]->setPhase(phase);
         inGate->setFanout(fanout);         
      }      
   }
   cout << "Simplifying: "<< inGate->getGateID() << " merging " 
   << ifExclamation(phase) << _gateID << "..." << endl;
}

void 
CirGate::inverseFanin(CirGate* zero, CirGate* inGate)
{
   int it;
   CirGate* fanout;
   for(int i = 0; i < 2; i++)
      if(isFanout(inGate, this, it)) inGate->removeFanout(it);
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      fanout = _fanoutList[i]->getPin();
      if(isFanin(fanout, this, it)){
         fanout->getFanin()[it]->setPin(zero);
         fanout->getFanin()[it]->setPhase(false);
         zero->setFanout(fanout); 
      }
   }
   

}