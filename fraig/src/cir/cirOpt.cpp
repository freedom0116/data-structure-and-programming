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

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
// First, remove aig that not in the dfs
// Second, remove undef or aig both inputs are const0
void
CirMgr::sweep()
{
  bool indfs, allZerofanin;
  for(int i = 0, s = _totalList.size(); i < s; i++){
    if(typeid(*_totalList[i]) == typeid(UnDef)){
      _totalList[i]->adjustFanoutState(Const0);
      delete _totalList[i];
      _totalList[i] = NULL;
      cout << "Sweeping: UNDEF(" << i << ") removed..." << endl;
    }
    else if(typeid(*_totalList[i]) == typeid(AndGate)){
      indfs = false;
      for(int j = 0, dfsSize = _dfsList.size(); j < dfsSize; j++){
        if(_totalList[i] == _dfsList[j]) indfs = true;
      }
      if(!indfs){
        _totalList[i]->adjustFanoutState(Const0);
        _totalList[i]->adjustFaninState(Const0);
        cout << "Sweeping: AIG(" << i << ") removed..." << endl;
      }
    }
  }
  GateList::iterator it = _aigList.begin();
  while(it != _aigList.end()){
    allZerofanin = true;
    vector<Pin> fanin = (*it)->getFanin();
    for(int i = 0, s = fanin.size(); i < s; i++){
      if(fanin[0].getPin() != Const0) allZerofanin = false;
    }
    if(allZerofanin){
      _totalList[(*it)->getGateID()] = NULL;
      _aigList.erase(it);
    }
    else { it++; }
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
