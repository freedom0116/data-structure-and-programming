/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

#define Const0 _totalList[0]

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static vector<size_t> makeKey(CirGate* gate)
{
  vector<size_t> keyValue;
  for(int i = 0, s = gate->getFanin().size(); i < s; i++){
    keyValue.push_back(size_t(gate->getFanin()[i]->getPin()) + gate->getFanin()[i]->getInvPhase());
  }
  return keyValue;
}

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed

void
CirMgr::strash()
{
  int id;
  CirGate* mergeGate;
  size_t size = (_dfsList.size()+_totalList.size()) / 2;
  HashMap<HashKey, CirGate*> hashList(getHashSize(size));
  for(int i = 0, s = _dfsList.size(); i < s; i++){
    if(typeid(*_dfsList[i]) == typeid(AndGate)){
      if(hashList.query(HashKey(makeKey(_dfsList[i])), mergeGate, true)){
        mergeGate->merge(_dfsList[i]);
        mergeGate->printMerge(0);
        id = _dfsList[i]->getGateID();
        _totalList[id]->setGateID(-1);
        _totalList[id] = NULL;
      }else hashList.insert(HashKey(makeKey(_dfsList[i])), _dfsList[i]);
    }
  }
  updateList(_aigList);
  _dfsList.clear();
  dfsTraversal(_poList);
}

// Need to clear _FECList & _fecGrps
void
CirMgr::fraig()
{
  SatSolver solver;
  solver.initialize();
  genProofModel(solver);
  for(int i = 0, s = _FECList.size(); i < s; i++){
    compFraig(solver, _FECList[i]);
  }
  updateList(_aigList);
  _dfsList.clear();
  dfsTraversal(_poList);
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void 
CirMgr::genProofModel(SatSolver& solver)
{
  Var v;
  vector<Pin*> fanin;
  for(int i = 0, s = _dfsList.size(); i < s; i++){
    v = solver.newVar();
    _dfsList[i]->setVar(v);
  }
  for(int i = 0, s = _aigList.size(); i < s; i++){
    fanin = _aigList[i]->getFanin();
    solver.addAigCNF(_aigList[i]->getVar(), fanin[0]->getPin()->getVar(),
                  fanin[0]->getInvPhase(), fanin[1]->getPin()->getVar(), fanin[1]->getInvPhase());
  }
}

void 
CirMgr::compFraig(SatSolver& solver, GateList& list)
{
  Var newV;
  bool result;
  int id;
  GateList::iterator iter = list.begin();
  for( ; iter != list.end(); iter++){
    GateList::iterator iter2 = iter+1;
    for( ; iter2 != list.end(); iter2++){
      newV = solver.newVar();
      if((*iter)->getPatt() == (*iter2)->getPatt())
        solver.addXorCNF(newV, (*iter)->getVar(), true,(*iter2)->getVar(), true);
      else
        solver.addXorCNF(newV, (*iter)->getVar(), true,(*iter2)->getVar(), false);
      solver.assumeRelease();  // Clear assumptions
      solver.assumeProperty(newV, true);  // k = 1
      result = solver.assumpSolve();
      if(!result){
        (*iter)->merge((*iter2));
        (*iter)->printMerge(1);
        id = (*iter2)->getGateID();
        _totalList[id]->setGateID(-1);
        _totalList[id] = NULL;
      }
    }
  }
}

void 
CirMgr::reportResult(const SatSolver& solver, bool result)
{
  solver.printStats();
  cout << (result? "SAT" : "UNSAT") << endl;
  if (result) {
    for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
        cout << "Gate" << _dfsList[i]->getGateID() << ": "
              << solver.getValue(_dfsList[i]->getVar()) << endl;
  }
}