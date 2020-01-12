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
  updateList(_aigList);
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  int id;
  for(int i = 0, s = _dfsList.size(); i < s; i++){
    id = _dfsList[i]->getGateID();
    if(typeid(*_dfsList[i]) == typeid(UnDef)){
      OptExecute(FANIN_ZERO, _dfsList[i]);
      delete _totalList[id];
      _totalList[id] = NULL;
    }else if(typeid(*_dfsList[i]) == typeid(AndGate)){
      if(checkFanin(_dfsList[i])){
        _totalList[id]->setGateID(-1);
        _totalList[id] = NULL;
      }
    }
  }
  updateList(_aigList);
  _dfsList.clear();
  dfsTraversal(_poList);
}
/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
bool
CirMgr::checkFanin(CirGate* gate)
{
  CirGate* pin1 = gate->getFanin()[0]->getPin();
  CirGate* pin2 = gate->getFanin()[1]->getPin();
  if(pin1 == pin2){
    if(gate->getFanin()[0]->getInvPhase() == gate->getFanin()[1]->getInvPhase()) 
      return OptExecute(SAME_FANIN, gate, pin1);
    else return OptExecute(INVERSE_FANIN, gate, pin1);
  }else if(pin1->getGateID() == 0){
    if(gate->getFanin()[0]->getInvPhase()) return OptExecute(FANIN_ONE, gate, pin2);
    else return OptExecute(FANIN_ZERO, gate, pin2);
  }else if(pin2->getGateID() == 0){
    if(gate->getFanin()[1]->getInvPhase()) return OptExecute(FANIN_ONE, gate, pin1);
    else return OptExecute(FANIN_ZERO, gate, pin1);
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
  return true;
}

void 
CirMgr::updateList(GateList& list)
{
  GateList::iterator it = list.begin();
  while(it != list.end()){
    if((*it)->getGateID() == -1){
      delete (*it);
      list.erase(it);
    }else{ it++; }
  }
}

// bool
// CirMgr::optNotDfs(CirGate* gate)
// {
//   if(typeid(*gate) == typeid(UnDef))
//     OptExecute(FANIN_ZERO, gate);
//   else if(typeid(*gate) == typeid(AndGate)){
//     if(!checkFanin(gate)) return false;
//   }else return false;
//   for(int i = 0, s = gate->getFanout().size(); i < s; i++){
//     optNotDfs(gate->getFanout()[i]->getPin());
//   }
//   delete _totalList[gate->getGateID()];
//   _totalList[gate->getGateID()] = 0;
//   return true;
// }

