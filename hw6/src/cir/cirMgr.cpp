/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <stdlib.h>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static string zeroInPin = "const0";

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{  
   fstream inputfile;
   string part;
   vector<int> state; // M I L O A
   vector<vector<int>> PoFanin, AIGFanin;
   inputfile.open(fileName);
   if(!inputfile) {
      cerr << "Can not open design \"" << fileName << "\"!!" << endl;
      return false;
   }
   if(!readHeader(inputfile, state)) return false;
   _totalList.resize(state[0]+state[3]+1);
   CirGate* flowinput = new CirPiGate(0, 0);
   _totalList[0] = flowinput;

   for(int i = 0; i < state[1]; i++){
      if(!readInput(inputfile, state[0])) return false;
   }
   for(int i = 0; i < state[3]; i++){
      if(!readOutput(inputfile, state[0], PoFanin)) return false;
   }
   for(int i = 0; i < state[4]; i++){
      if(!readAig(inputfile, state[0], AIGFanin)) return false;
   }
   setFanIO(PoFanin);
   setFanIO(AIGFanin);
   dfsTraversal(_poList);
   insertSort(_piList);
   insertSort(_poList);
   insertSort(_aigList);

   bool stopRun = false;
   while(!stopRun){
   // for(int i = 0;i<10;i++){
      if(!readSymbol(inputfile, stopRun)) return false;
   }

   // for(int i = 0; i < _totalList.size(); i++){
   //    if(_totalList[i] != 0){
   //       cout<<i<<":"<<_totalList[i]->getSymbols();
   //       string classType = typeid(*_totalList[i]).name();
   //       cout<<typeid(*_totalList[i]).name()<<endl;
   //       cout<<i<<" " <<"in:";
   //       for(int j = 0; j <_totalList[i]->getFanin().size();j++){
   //          cout<<" ";
   //          if(_totalList[i]->getFanin()[j].getInvPhase() == true) cout<<"!";
   //          cout<<_totalList[i]->getFanin()[j].getPin()->getGateID();
   //       }
   //       cout<<"out:";
   //       for(int j = 0; j <_totalList[i]->getFanout().size();j++){
   //          cout<<" ";
   //          if(_totalList[i]->getFanout()[j].getInvPhase() == true) cout<<"!";
   //          cout<<_totalList[i]->getFanout()[j].getPin()->getGateID();
   //       }
   //       cout<<endl;
   //    }
   // }
   return true;
}

bool
CirMgr::readHeader(fstream& file, vector<int>& state)
{
   string header;
   vector<string> sections;
   getline(file, header); cout<<header<<endl;
   if(!cutPiece(header, sections)) return false;

   int num;
   if(sections.size() < 6) return parseError(MISSING_DEF);
   if(sections[0] != "aag") return parseError(ILLEGAL_IDENTIFIER);
   for(int i = 1, s = sections.size(); i < s; i++){
      if(myStr2Int(sections[i], num)){
         if(num >= 0) state.push_back(num);
         else{ errInt = num; return parseError(ILLEGAL_NUM); }
      }
      else{ errMsg = sections[i]; return parseError(MISSING_NUM); }
   }
   int ILA = state[1] + state[2] + state[4];
   if(ILA > state[0]) return false;
   lineNo++;
   return true;
}

bool
CirMgr::readInput(fstream& file, int& M)
{
   int num;
   string I;
   vector<string> sections;
   getline(file, I);
   if(!cutPiece(I, sections)) return false;
   if(sections.size() != 1) {
      cerr << "have less or more than one element" << endl;
      return false;
   }
   if(myStr2Int(sections[0], num)){
      if(num < 0) { errInt = num; return parseError(NUM_TOO_SMALL); }
      if(num > M*2) { errInt = num; return parseError(NUM_TOO_BIG); }
   }else{ errMsg = sections[0]; return parseError(MISSING_NUM); }

   CirGate* PI = new CirPiGate(num, lineNo++);
   _piList.push_back(PI);
   _totalList[num/2] = PI;
   return true;
}

bool
CirMgr::readOutput(fstream& file, int& M, vector<vector<int>>& fanin)
{
   int num, ID;
   string I;
   vector<string> sections;
   vector<int> out;
   CirGate* PO;
   getline(file, I);
   if(!cutPiece(I, sections)) return false;
   if(sections.size() != 1) {
      cerr << "have less or more than one element" << endl;
      return false;
   }
   if(myStr2Int(sections[0], num)){
      if(num < 0) { errInt = num; return parseError(NUM_TOO_SMALL); }
      if(num > M*2) { errInt = num; return parseError(NUM_TOO_BIG); }
   }else{ errMsg = sections[0]; return parseError(MISSING_NUM); }

   if(_poList.empty()){
      ID = (M + 1) * 2;
      PO = new CirPoGate(ID, lineNo++);
   }else{
      ID = (_poList[_poList.size() - 1]->getGateID() + 1) * 2;
      PO = new CirPoGate(ID, lineNo++);
   }
   _poList.push_back(PO);
   _totalList[ID/2] = PO;

   out.push_back(ID/2); out.push_back(num);
   fanin.push_back(out);
   return true;
}

bool
CirMgr::readAig(fstream& file, int& M, vector<vector<int>>& fanin)
{
   int num;
   string I;
   vector<string> sections;
   vector<int> gate;
   getline(file, I);
   if(!cutPiece(I, sections)) return false;
   if(sections.size() != 3) {
      cerr << "have less or more than one element" << endl;
      return false;
   }
   if(myStr2Int(sections[0], num)){
      if(num < 0) { errInt = num; return parseError(NUM_TOO_SMALL); }
      if(num > M*2) { errInt = num; return parseError(NUM_TOO_BIG); }
   }else{ errMsg = sections[0]; return parseError(MISSING_NUM); }
   CirGate* AIG = new AndGate(num, lineNo++);
   _aigList.push_back(AIG);
   _totalList[num/2] = AIG;
   gate.push_back(num/2);

   for(int i = 1; i < 3; i++){
      if(myStr2Int(sections[i], num)){
         if(num < 0) { errInt = num; return parseError(NUM_TOO_SMALL); }
         if(num > M*2) { errInt = num; return parseError(NUM_TOO_BIG); }
      }else{ errMsg = sections[i]; return parseError(MISSING_NUM); }
      gate.push_back(num);
   }
   fanin.push_back(gate);
   return true;
}

bool
CirMgr::readSymbol(fstream& file, bool& stopRun)
{
   string I;
   vector<string> sections;
   getline(file, I);
   if(I == "") { stopRun = true; return true; }
   if(!cutPiece(I, sections)) return false;
   if(sections.size() == 1){
      if(sections[0] == "c") { stopRun = true; return true; }
      return false;
   }
   else if(sections.size() != 2) {
      cerr << "have less or more than one element" << endl;
      return false;
   }
   if(sections[0].size() != 2){
      cerr << "wrong symbols" << endl;
      return false;
   }

   char setIO = sections[0][0];
   char* id = &sections[0][1];
   int pinID = atoi(id);
   if(setIO == 'i'){
      if(_piList[pinID] == 0) {
         cerr << "number wrong" << endl; return false;
      }
      _piList[pinID]->setSymbols(sections[1]);
   }
   else if(setIO == 'o'){
      if(_poList[pinID] == 0) {
         cerr << "number wrong" << endl; return false;
      }
      _poList[pinID]->setSymbols(sections[1]);
   }
   else{
      cerr << "wrong pin select" << endl;
      return false;
   }
   return true;   
}

bool
CirMgr::cutPiece(string& pharse, vector<string>& words)
{
   int begin = 0, end = 0;
   bool prevSpace = false;
   for(int i = 0, s = pharse.length(); i <= s; i++){
      if(pharse[0] == ' ') return parseError(EXTRA_SPACE);
      if(prevSpace == true){
         if(pharse[i] == ' ') return parseError(EXTRA_SPACE);
         else{ prevSpace = false; begin = i; }
      }else{
         end = i;
         if(pharse[i] == ' '){
            words.push_back(pharse.substr(begin, end - begin));
            prevSpace = true;
         }
         if(end == s){ words.push_back(pharse.substr(begin, end - begin + 1)); }
      }
   }
   return true;
}

void
CirMgr::setFanIO(vector<vector<int>>& fanin)
{
   int ID, inID;
   bool phase;
   for(int i = 0, s = fanin.size(); i < s; i++){
      ID = fanin[i][0];
      for(int j = 1, insize = fanin[0].size(); j < insize; j++){
         inID = fanin[i][j] / 2;
         (fanin[i][j]%2 == 1)? phase = true : phase = false;
         if(_totalList[inID] == 0){
            CirGate* Undef = new UnDef(inID);
            _totalList[inID] = Undef;
         }
         _totalList[ID]->setFanin(_totalList[inID], phase); // set fanin
         _totalList[inID]->setFanout(_totalList[ID]); //set faninout
      }
   }
}

void
CirMgr::dfsTraversal(const vector<CirGate*>& sinkList)
{
   CirGate::setGlobalRef();
   for(int i = 0, s = sinkList.size(); i < s; i++){
      sinkList[i]->dfsTraversal(_dfsList);
   }
}

void 
CirMgr::insertSort(vector<CirGate*>& list)
{
   CirGate* temp;
   vector<CirGate*>::iterator iter = list.begin(), comp;
   for(; iter != list.end(); iter++){
      for(comp = list.begin(); comp != list.end(); comp++){
         if((*iter)->getGateID() < (*comp)->getGateID()){
            temp = *comp;
            *comp = *iter;
            *iter = temp;
         }
      }
   }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  " << left << setw(7) << "PI" << right << setw(7) << _piList.size() <<endl;
   cout << "  " << left << setw(7) << "PO" << right << setw(7) << _poList.size() <<endl;
   cout << "  " << left << setw(7) << "AIG" << right << setw(7) << _aigList.size() <<endl;
   cout << "------------------" << endl;
   cout << "  " << left << setw(7) << "Total" << right << setw(7) 
         << _piList.size() + _poList.size() + _aigList.size() <<endl;
}

void
CirMgr::printNetlist() const
{
   vector<Pin> fanin;
   cout << endl;
   for(int i = 0, s = _dfsList.size(); i < s; i++){
      cout << "[" << i << "] ";
      if(_dfsList[i]->getGateID() == 0) cout << "const0" << endl;
      else if(typeid(*_dfsList[i]) == typeid(CirPiGate)){
         cout << left << setw(3) << "PI" << " " << _dfsList[i]->getGateID();
         if(_dfsList[i]->getSymbols() != ""){
            cout << " (" << _dfsList[i]->getSymbols() << ")";
         } cout << endl;
      }
      else if(typeid(*_dfsList[i]) == typeid(UnDef)){
         cout << "UNDEF" << " " << _dfsList[i]->getGateID() << endl;
      }
      else{
         if(typeid(*_dfsList[i]) == typeid(CirPoGate))
            cout << left << setw(3) << "PO" << " " << _dfsList[i]->getGateID();
         else if(typeid(*_dfsList[i]) == typeid(AndGate))
            cout << left << setw(3) << "AIG" << " " << _dfsList[i]->getGateID();
         fanin = _dfsList[i]->getFanin();
         for(int j = 0, s = fanin.size(); j < s; j++){
            cout << " ";
            if(typeid(*(fanin[j].getPin())) == typeid(UnDef)) cout << "*";
            if(fanin[j].getInvPhase()) cout << "!";
            cout << fanin[j].getPin()->getGateID();
         } 
         if(_dfsList[i]->getSymbols() != ""){
            cout << " (" << _dfsList[i]->getSymbols() << ")";
         } cout << endl;
      }
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i = 0, s = _piList.size(); i < s; i++){
      cout << " " << _piList[i]->getGateID();
   }cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i = 0, s = _poList.size(); i < s; i++){
      cout << " " << _poList[i]->getGateID();
   }cout << endl;
}

void
CirMgr::printFloatGates() const
{  ;
   vector<Pin> fanin;
   vector<int> floatInGate, NoOutGate;
   for(int i = 0, s = _aigList.size(); i < s; i++){
      fanin = _aigList[i]->getFanin();
      for(int j = 0, inSize = fanin.size(); j < inSize; j++){
         if(typeid(*(fanin[j].getPin())) == typeid(UnDef)){
            floatInGate.push_back(i);
            break;
         }
      }
      if(_aigList[i]->getFanout().size() == 0)
         NoOutGate.push_back(i);
   }
   if(floatInGate.size() > 0){
      cout << "Gates with floating fanin(s):";
      for(int i = 0, s = floatInGate.size(); i < s; i++){
         cout << " " << floatInGate[i];
      } cout << endl;
   }
   if(NoOutGate.size() > 0){
      cout << "Gates defined but not used :";
      for(int i = 0, s = NoOutGate.size(); i < s; i++){
         cout << " " << NoOutGate[i];
      } cout << endl;
   }   
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
