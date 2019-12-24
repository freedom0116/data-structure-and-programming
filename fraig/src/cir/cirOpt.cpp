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
void
CirMgr::sweep()
{
   GateList::iterator it = _aigList.begin();
   bool indfs;
   while(it != _aigList.end()){
      if(typeid(*it) == typeid(UnDef)){
         (*it)->adjustFanout(_totalList[0]);
         _totalList[(*it)->getGateID()] = NULL;
         _aigList.erase(it);
      }
      else{
         indfs = false;
         for(int j = 0, dfsSize = _dfsList.size(); j < dfsSize; j++){
            if(*it == _dfsList[j]) indfs = true;
         }
         if(!indfs){
            (*it)->adjustFanout(_totalList[0]);
            _totalList[(*it)->getGateID()] = NULL;
            _aigList.erase(it);
         }
      }
      it++;
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
