/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

#define Const0 _totalList[0]

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
enum SimFileError{
  ILLEAGE_LENGTH,
  ILLEAGE_PATTERN,
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static size_t simLength = 0;
static size_t errLength = 0;
static string errLine;
static char errWord;

static void
simError(SimFileError err)
{
  switch(err){
    case ILLEAGE_LENGTH:
      cerr << "Error: Pattern(" << errLine << ") length(" << errLength 
           << ") does not match the number of inputs(" << simLength << ") in a circuit!!" << endl;
      break;
    case ILLEAGE_PATTERN:
      cerr << "Error: Pattern(" << errLine << ") contains a non-0/1 character('" 
           << errWord << "')." << endl;
      break;    
  }
}

// Generate random simulation number
static size_t genPatt()
{
  return ((size_t)rnGen(INT_MAX) << 32) | (size_t)rnGen(INT_MAX);
}

// Set Pi/AIG/PO simulation for random simulation
static void setRanSim(GateList& list)
{
  for(int i = 0, s = list.size(); i < s; i++){
    if(typeid(*list[i]) == typeid(CirPiGate))
      list[i]->setPatt(genPatt());
    else if(typeid(*list[i]) == typeid(CirPoGate))
      list[i]->setPatt();
    else if(typeid(*list[i]) == typeid(AndGate))
      list[i]->setPatt();
  }  
}

// Set AIG/Po simulation for reading simulation file
static void setInputSim(GateList& list)
{
  for(int i = 0, s = list.size(); i < s; i++){
    if(typeid(*list[i]) == typeid(CirPoGate))
      list[i]->setPatt();
    else if(typeid(*list[i]) == typeid(AndGate))
      list[i]->setPatt();
  }  
}

// If there is a new simulation works, and we need what to check
// whether elements in same FECGroup will be separated.
static bool grpClassify(const SimKey k, HashMap<SimKey, CirGate*>& fecGrps, HashMap<SimKey, CirGate*>& newGrps)
{
  bool change = false;
  vector<CirGate*> keyGrp;
  fecGrps.getKeyNodes(k, keyGrp);
  // check if change
  for(int i = 1, s = keyGrp.size(); i < s; i++){
    if(keyGrp[i-1]->getPatt() != keyGrp[i]->getPatt())
      if(keyGrp[i-1]->getPatt() != ~keyGrp[i]->getPatt())
        change = true;
  }
  // new FECGrp generate
  HashMap<SimKey, CirGate*> Grp(getHashSize(keyGrp.size()));
  for(int i = 0, s = keyGrp.size(); i < s; i++){
    if(Grp.query(SimKey(~keyGrp[i]->getPatt()), keyGrp[i]))
      Grp.insert(SimKey(~keyGrp[i]->getPatt()), keyGrp[i]);
    else Grp.insert(SimKey(keyGrp[i]->getPatt()), keyGrp[i]); }
  HashMap<SimKey, CirGate*>::iterator it = Grp.begin();
  for( ; it != Grp.end(); ++it){
    if(Grp.query((*it).first, (*it).second))
      newGrps.insert((*it).first, (*it).second);
  }
  return change;
}

// Check if this key allready store in list
static bool DataInVec(CirGate* gate, vector<GateList>& list)
{
  for(int i = 0, s = list.size(); i < s; i++){
    if(gate->getPatt() == list[i].front()->getPatt()){
      list[i].push_back(gate);
      return true;
    }else if(~gate->getPatt() == list[i].front()->getPatt()){
      list[i].push_back(gate);
      return true;
    }
  }
  return false;
}

// Sort elements in same FECGroup
static void sortGrp(GateList& list)
{
  CirGate* temp;
  for(int i = 0, s = list.size(); i < s; i++){
    for(int j = 0; j < i; j++){
      if(list[i]->getGateID() < list[j]->getGateID()){
        temp = list[j];
        list[j] = list[i];
        list[i] = temp;
      }
    }
  }
}

static void passHash(const HashMap<SimKey, CirGate*>& passHash, HashMap<SimKey, CirGate*>& getHash)
{
  HashMap<SimKey, CirGate*>::iterator it = passHash.begin(); 
  for( ; it != passHash.end(); ++it){
    getHash.insert((*it).first, (*it).second);
  }
}


// static void printHashMap(const HashMap<SimKey, CirGate*>& hash, bool showPatt = false)
// {
//   HashMap<SimKey, CirGate*>::iterator it = hash.begin();
//   int k = -1;
//   cout<<"---------------------";
//   for( ; it != hash.end(); ++it){
//     if((*it).first()%hash.numBuckets() != k){
//       cout << endl;
//       k = (*it).first()%hash.numBuckets();
//       cout << "[" << (*it).first()%hash.numBuckets() << "] ";
//     }
//     cout<<(*it).second->getGateID();
//     if(showPatt) cout<<"("<<(*it).first()<<")";
//     cout << " ";
//   }cout<<endl;
// }

// Separate input simulation with space and change line,
// then store in patterns
static void cutSim(string& input, vector<string>& patterns)
{
  size_t begin = 0;
  bool prevSpace = true;
  patterns.clear();
  for(size_t i = 0, s = input.length(); i < s; i++){
    if(input[i] == ' '){
      if(!prevSpace){
        patterns.push_back(input.substr(begin, i - begin));
        prevSpace = true;
      }
    }else{
      if(prevSpace){ begin = i; prevSpace = false; }
      if(i == s-1) patterns.push_back(input.substr(begin, i - begin + 1));
    }
  }
}

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  bool stopSim = false, FECchange = false;
  size_t simNum = 0, keepNum = 0;
  HashMap<SimKey, CirGate*> newGrps;
  if(_FECList.size() == 0){
    _fecGrps.init(getHashSize(_dfsList.size()));
    _fecGrps.insert(SimKey(0), Const0);
    initFECHash();
    simNum++;
  }
  newGrps.init(_fecGrps.numBuckets());
  while(!stopSim){
    // cout<<"keppNum: "<<keepNum<<endl;
    setRanSim(_dfsList);
    HashMap<SimKey, CirGate*>::iterator it = _fecGrps.begin();
    for( ; it != _fecGrps.end(); ++it){
      if(_fecGrps.query((*it).first, (*it).second))
        if(!newGrps.query(SimKey((*it).second->getPatt()), (*it).second))
          if(!newGrps.query(SimKey(~(*it).second->getPatt()), (*it).second))
            if(grpClassify((*it).first, _fecGrps, newGrps)) FECchange = true;
    }
    _fecGrps.clear();
    passHash(newGrps, _fecGrps);
    newGrps.clear();
    simNum++;
    (FECchange)? keepNum = 0 : keepNum++;
    (keepNum == 4)? stopSim = true : FECchange = false;
  }
  cout << (simNum*64) << " patterns simulated." << endl;
  Hash2Vec();
  sortFEC();
  setFECsData();
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  // record input
  size_t simNum = 0, simPos = 0;
  string simInput;
  vector<string> patterns;
  vector<vector<int>> CirPiSim(_piList.size());
  simLength = _piList.size();
  while(getline(patternFile, simInput)){
    cutSim(simInput, patterns);
    for(size_t i = 0, s = patterns.size(); i < s; i++){
      if(patterns[i].size() != simLength){
        errLine = patterns[i]; errLength = patterns[i].size();
        return simError(ILLEAGE_LENGTH); }
      for(size_t j = 0, patts = patterns[i].length(); j < patts; j++){
        if(patterns[i][j] == '0') CirPiSim[j].push_back(0);
        else if(patterns[i][j] == '1') CirPiSim[j].push_back(1);
        else{
          errLine = patterns[i]; errWord = patterns[i][j];
          return simError(ILLEAGE_PATTERN); }
      }
    } simNum += patterns.size();
  }
  // simulate input
  size_t pattNum = 0;
  HashMap<SimKey, CirGate*> newGrps;
  while(simPos < CirPiSim.front().size()){
    if(CirPiSim.front().size()-simPos > 64){
      for(size_t i = 0, s = simLength; i < s; i++){
        for(size_t j = 0; j < 64; j++){
          pattNum += CirPiSim[i][simPos+j] << j; 
        } 
        _piList[i]->setPatt(pattNum); pattNum = 0; 
      } 
      simPos += 64;
    }else{
      for(size_t i = 0; i < simLength; i++){
        for(size_t j = 0, s = CirPiSim.front().size()-simPos; j < s; j++){
          pattNum += CirPiSim[i][simPos+j] << j;
        } 
        _piList[i]->setPatt(pattNum); pattNum = 0; 
      }
      simPos += (CirPiSim.front().size()-simPos);
    }
    if(_fecGrps.empty()) {
      _fecGrps.init(getHashSize(_dfsList.size()));
      _fecGrps.insert(SimKey(0), Const0);
      initInputFEC();
    }
    else{
      setInputSim(_dfsList);
      HashMap<SimKey, CirGate*>::iterator it = _fecGrps.begin();
      for( ; it != _fecGrps.end(); ++it){
        if(_fecGrps.query((*it).first, (*it).second))
          if(!newGrps.query(SimKey((*it).second->getPatt()), (*it).second))
            if(!newGrps.query(SimKey(~(*it).second->getPatt()), (*it).second))
              grpClassify((*it).first, _fecGrps, newGrps);
      }
      _fecGrps.clear();
      passHash(newGrps, _fecGrps);
      newGrps.clear();
    }
    Hash2Vec();
    sortFEC();
    setFECsData();
  }
  cout << simNum << " patterns simulated." << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

// This function use when calling random simulation 
// with empty _fecGrps.
void
CirMgr::initFECHash()
{
  for(int i = 0, s = _dfsList.size(); i < s; i++){
    if(typeid(*_dfsList[i]) == typeid(CirPiGate))
      _dfsList[i]->setPatt(genPatt());
    else if(typeid(*_dfsList[i]) == typeid(CirPoGate))
      _dfsList[i]->setPatt();
    else if(typeid(*_dfsList[i]) == typeid(AndGate)){
      _dfsList[i]->setPatt();
      if(_fecGrps.query(SimKey(~_dfsList[i]->getPatt()), _dfsList[i]))
        _fecGrps.insert(SimKey(~_dfsList[i]->getPatt()), _dfsList[i]);
      else _fecGrps.insert(SimKey(_dfsList[i]->getPatt()), _dfsList[i]); }
  }
}

// This function use when input simulate file
// with empty _fecGrps.
void
CirMgr::initInputFEC()
{
  for(int i = 0, s = _dfsList.size(); i < s; i++){
    if(typeid(*_dfsList[i]) == typeid(CirPoGate))
      _dfsList[i]->setPatt();
    if(typeid(*_dfsList[i]) == typeid(AndGate)){
      _dfsList[i]->setPatt();
      if(_fecGrps.query(SimKey(~_dfsList[i]->getPatt()), _dfsList[i]))
        _fecGrps.insert(SimKey(~_dfsList[i]->getPatt()), _dfsList[i]);
      else _fecGrps.insert(SimKey(_dfsList[i]->getPatt()), _dfsList[i]); }
  }
}

// This fuction convert data in _fecGrps to _FECList
void 
CirMgr::Hash2Vec()
{
  _FECList.clear();
  HashMap<SimKey, CirGate*>::iterator it = _fecGrps.begin();
  for( ; it != _fecGrps.end(); ++it){
    if(!DataInVec((*it).second, _FECList)){
      GateList Grp;
      Grp.push_back((*it).second);
      _FECList.push_back(Grp);
    }
  }
  vector<GateList>::iterator it2 = _FECList.begin();
  for( ;it2 != _FECList.end(); ){
    if((*it2).size() == 1) _FECList.erase(it2);
    else it2++;
  }
}

// Sort _FECList from small to big number
void
CirMgr::sortFEC()
{
  GateList tempL;
  for(int i = 0, s = _FECList.size(); i < s; i++){
    sortGrp(_FECList[i]); }
  for(int i = 0, s = _FECList.size(); i < s; i++){
    for(int j = 0; j < i; j++){
      if(_FECList[i].front()->getGateID() < _FECList[j].front()->getGateID()){
        tempL = _FECList[i];
        _FECList[j] = _FECList[j];
        _FECList[i] = tempL;
      }
    }
  }
}

// This function input FECsData to CirGate::_FECs
void 
CirMgr::setFECsData()
{
  for(int i = 0, s = _FECList.size(); i < s; i++){
    for(int j = 0, gs = _FECList[i].size(); j < gs; j++){
      _FECList[i][j]->resetFECs();
      for(int n = 0; n < gs; n++){
        if(n != j) _FECList[i][j]->setFECs( _FECList[i][n]); }
    }
  }
}