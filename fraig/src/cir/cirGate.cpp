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
static size_t mergePhase;
static size_t replaceID;

static void printDivider()
{
   for(int i = 0; i < 80; i++)
      cout << "=";
   cout << endl;
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

static bool samePhase(CirGate* merge, CirGate* replace)
{
   if(merge->getFanin()[0]->getPin()->getGateID() == merge->getFanin()[1]->getPin()->getGateID())
      return false;
   if(merge->getFanin()[0]->getPin()->getGateID() == replace->getFanin()[0]->getPin()->getGateID())
      return (merge->getFanin()[0]->getPin()->getInvPhase() != replace->getFanin()[0]->getPin()->getInvPhase());
   if(merge->getFanin()[0]->getPin()->getGateID() == replace->getFanin()[1]->getPin()->getGateID())
      return (merge->getFanin()[0]->getPin()->getInvPhase() != replace->getFanin()[1]->getPin()->getInvPhase());
   return true;
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

string 
CirGate::getIdData() const
{
   string output;
   output = "= " + getTypeStr() + "(" + to_string(_gateID) + ")";
   if(_symbols != "")
      output = output + "\"" + _symbols + "\"";
   output = output + ", line " + to_string(_lineNO);
   return output;
}

string
CirGate::getFECsData() const
{
   string output;
   output = "= FECs:";
   for(size_t i = 0, s = _FECs.size(); i < s; i++){
      output = output + " ";
      if( _pattern != _FECs[i]->getPatt()) output = output + "!";
      output = output + to_string(_FECs[i]->getGateID());
   }
   return output;
}

string
CirGate::getSimData() const
{
   size_t patt = _pattern, supply;
   string output;
   vector<int> value;
   output = "= Value: ";
   while(patt != 0){
      value.push_back(patt%2);
      patt = patt >> 1;
   }
   supply = 64 - value.size();
   for(int i = 0, s =supply; i < s; i++){
      output = output + "0";
      if(i%8 == 7) output = output + "_";
   }
   for(int i = 0, s = value.size(); i < s; i++){
      if(i != 0 && (i+supply)%8 == 0) output = output + "_";
      if(value[value.size()-1-i] == 0) output = output + "0";
      else output = output + "1";
   }
   return output;
}

void CirPiGate::printGate() const
{
   printDivider();
   cout << getIdData() << endl;
   cout << getFECsData() << endl;
   cout << getSimData() << endl;
   printDivider();
}
void CirPoGate::printGate() const
{
   string data = getIdData();
   printDivider();
   cout << getIdData() << endl;
   cout << getFECsData() << endl;
   cout << getSimData() << endl;
   printDivider();
}
void AndGate::printGate() const
{
   string data = getIdData();
   printDivider();
   cout << getIdData() << endl;
   cout << getFECsData() << endl;
   cout << getSimData() << endl;
   printDivider();
}
void UnDef::printGate() const
{
   string data = getIdData();
   printDivider();
   cout << getIdData() << endl;
   cout << getFECsData() << endl;
   cout << getSimData() << endl;
   printDivider();
}
void Const0::printGate() const
{
   string data = getIdData();
   printDivider();
   cout << getIdData() << endl;
   cout << getFECsData() << endl;
   cout << getSimData() << endl;
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
   }
}

void 
CirGate::faninOne(CirGate* one, CirGate* replace)
{
   int it;
   bool phase, inP;
   CirGate* fanout;
   isFanin(this, replace, it);
   inP = _faninList[it]->getInvPhase();
   if(isFanout(one, this, it)) one->removeFanout(it);
   if(isFanout(replace, this, it)) replace->removeFanout(it);
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      fanout = _fanoutList[i]->getPin();
      if(isFanin(fanout, this, it)){
         phase = (inP != fanout->getFanin()[it]->getInvPhase());
         fanout->getFanin()[it]->setPin(replace);
         fanout->getFanin()[it]->setPhase(phase);
         replace->setFanout(fanout);
      }
   }
   cout << "Simplifying: "<< replace->getGateID() << " merging " 
   << (phase? "!" : "") << _gateID << "..." << endl;
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
         zero->setFanout(fanout);
      }      
   }
   cout << "Simplifying: 0 merging " << _gateID << "..." << endl;
}

void 
CirGate::sameFanin(CirGate* inGate)
{
   int it;
   bool phase, inP;
   CirGate* fanout;
   isFanin(this, inGate, it);
   inP = _faninList[it]->getInvPhase();
   for(int i = 0; i < 2; i++)
      if(isFanout(inGate, this, it)) inGate->removeFanout(it);
   for(int i = 0, s = _fanoutList.size(); i < s; i++){
      fanout = _fanoutList[i]->getPin();
      if(isFanin(fanout, this, it)){
         phase = (inP != fanout->getFanin()[it]->getInvPhase());
         fanout->getFanin()[it]->setPin(inGate);
         fanout->getFanin()[it]->setPhase(phase);
         inGate->setFanout(fanout);         
      }      
   }
   cout << "Simplifying: "<< inGate->getGateID() << " merging " 
   << (phase? "!" : "") << _gateID << "..." << endl;
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
         zero->setFanout(fanout);
      }
   }
   cout << "Simplifying: 0 merging " << _gateID << "..." << endl;
}

void
CirGate::merge(CirGate* replace)
{
   int it;
   bool sameP;
   CirGate *fanin, *fanout;
   sameP = samePhase(this, replace);
   for(int i = 0; i < 2; i++){
      fanin = replace->getFanin()[i]->getPin();
      if(isFanout(fanin, replace, it)) fanin->removeFanout(it);
   }
   for(int i = 0, s = replace->getFanout().size(); i < s; i++){
      fanout = replace->getFanout()[i]->getPin();
      if(isFanin(fanout, replace, it)){
         mergePhase = (sameP != fanout->getFanin()[it]->getInvPhase());
         replaceID = replace->getGateID();
         fanout->getFanin()[it]->setPin(this);
         fanout->getFanin()[it]->setPhase(mergePhase);
         this->setFanout(fanout);
      }
   }
}

void 
CirGate::printMerge(int type)
{
   if(type == 0) cout << "Strashing: ";
   else if(type == 1) cout << "Fraig: ";
   cout << this->getGateID() << " merging " 
   << (mergePhase? "!" : "") << replaceID << "..." << endl;
}

void
CirPoGate::setPatt()
{
   if(_faninList[0]->getInvPhase())
      _pattern = ~_faninList[0]->getPin()->getPatt();
   else _pattern = _faninList[0]->getPin()->getPatt();   
}

void 
AndGate::setPatt()
{
   if(_faninList[0]->getInvPhase())
      _pattern = ~_faninList[0]->getPin()->getPatt();
   else _pattern = _faninList[0]->getPin()->getPatt();
   if(_faninList[1]->getInvPhase())
      _pattern = _pattern & ~_faninList[1]->getPin()->getPatt();
   else _pattern = _pattern & _faninList[1]->getPin()->getPatt();
}