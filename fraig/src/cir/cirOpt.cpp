/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

#define Const0 _totalList[0]

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
enum CirOptType{
  FANIN_ONE = 1,
  FANIN_ZERO,
  SAME_FANIN,
  INVERSE_FANIN
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  bool indfs;
  for(int i = 0, s = _totalList.size(); i < s; i++){
    if(typeid(*_totalList[i]) == typeid(UnDef)){
      indfs = false;
      for(int j = 0, dfsSize = _dfsList.size(); j < dfsSize; j++){
        if(_totalList[i] == _dfsList[j]) indfs = true;
      }
      if(!indfs){
        _totalList[i]->sweep();
        delete _totalList[i];
        _totalList[i] = NULL;
        cout << "Sweeping: UNDEF(" << i << ") removed..." << endl;
      }
    }else if(typeid(*_totalList[i]) == typeid(AndGate)){
      indfs = false;
      for(int j = 0, dfsSize = _dfsList.size(); j < dfsSize; j++){
        if(_totalList[i] == _dfsList[j]) indfs = true;
      }
      if(!indfs){
        _totalList[i]->sweep();
        _totalList[i]->setGateID(-1);
        _totalList[i] = NULL;
        cout << "Sweeping: AIG(" << i << ") removed..." << endl;
      }
    }
  }
  updateAIG();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  bool isGateChange = true;
  if(_isSimulated) 
    cout << "Error: circuit has been simulated!! Do '\"'CIRFraig'\"' first!!" << endl;
  else{
    vector<Pin> fanin;
    for(int i = 0, s = _totalList.size(); i < s; i++){
      if(typeid(*_totalList[i]) == typeid(UnDef)){
        OptExecute(FANIN_ZERO, _totalList[i]);
      }
      else if(typeid(*_totalList[i]) == typeid(AndGate)){
        checkFanin(_totalList[i]);
      }
    }
    updateAIG();
    while(isGateChange){
      isGateChange = false;
      for(int i = 0, s = _aigList.size(); i < s; i++){
        if(checkFanin(_aigList[i])) isGateChange = true;
      }
      updateAIG();
    }
    _dfsList.clear();
    dfsTraversal(_poList);
  }
}
/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
bool
CirMgr::checkFanin(CirGate* g)
{
  CirGate* pin1 = g->getFanin()[0]->getPin();
  CirGate* pin2 = g->getFanin()[1]->getPin();
  if(pin1->getGateID() == 0){
    if(g->getFanin()[0]->getInvPhase()) return OptExecute(FANIN_ONE, g ,pin2);
    else return OptExecute(FANIN_ZERO, g ,pin2);
  }else if(pin2->getGateID() == 0){
    if(g->getFanin()[1]->getInvPhase()) return OptExecute(FANIN_ONE, g ,pin1);
    else return OptExecute(FANIN_ZERO, g ,pin1);
  }else if(pin1 == pin2){
    if(g->getFanin()[0]->getInvPhase() == g->getFanin()[1]->getInvPhase()) 
      return OptExecute(SAME_FANIN, g, pin1);
    else return OptExecute(INVERSE_FANIN, g, pin1);
  }
  return false;
}

bool
CirMgr::OptExecute(int type, CirGate* gate, CirGate* nonZero)
{
  switch(type){
    case FANIN_ONE:
      gate->faninOne(Const0, nonZero);
      break;
    case FANIN_ZERO:
      gate->faninZero(Const0, nonZero);
      break;
    case SAME_FANIN:
      gate->sameFanin(nonZero);
      break;
    case INVERSE_FANIN:
      gate->inverseFanin(Const0, nonZero);
      break;
  }
  if(typeid(*gate) == typeid(UnDef)){
    delete _totalList[gate->getGateID()];
    _totalList[gate->getGateID()] = NULL;        
  }else if(typeid(*gate) == typeid(AndGate)){
    _totalList[gate->getGateID()]->setGateID(-1);
    _totalList[gate->getGateID()] = NULL;
  }
  return true;
}

void 
CirMgr::updateAIG()
{
  GateList::iterator it = _aigList.begin();
  while(it != _aigList.end()){
    if((*it)->getGateID() == -1){
      delete (*it);
      _aigList.erase(it);
    }else{ it++; }
  }
}

void 
CirMgr::updateDfs()
{
  GateList::iterator it = _dfsList.begin();
  while(it != _dfsList.end()){
    if((*it)->getGateID() == -1){
      (*it) = NULL;
      _dfsList.erase(it);
    }else{ it++; }
  }
}
