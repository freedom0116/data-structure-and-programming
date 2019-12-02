/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;
class Pin;
class GateState;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class Pin
{
public:
  Pin(CirGate* ID, bool phase = false): _pin(ID), _invPhase(phase) {}
  ~Pin() {}
  CirGate* getPin() const { return _pin; }
  bool getInvPhase() const { return _invPhase; }

private:
  CirGate* _pin;
  bool _invPhase;
};

class CirGate
{
public:
  CirGate(int num = 0, int NO = 0): _gateID(num/2), _lineNO(NO)
    { (num%2 == 0)? _invPhase = false :  _invPhase = true; _ref = 0; }
  virtual ~CirGate() {}

  static void setGlobalRef() { _globalRef++; }
  void setToGlobalRef() { _ref = _globalRef; }
  bool isGlobalRef() { return (_ref == _globalRef); }
  void dfsTraversal(vector<CirGate*>&);

  // Basic access methods
  string getTypeStr() const { return ""; }
  int getGateID() const { return _gateID; }
  unsigned getLineNo() const { return _lineNO; }
  bool getInvPhase() const { return _invPhase; }
  string getSymbols() const { return _symbols; }
  void setSymbols(string s) { _symbols = s; }

  vector<Pin> getFanin() const { return _faninList; }
  vector<Pin> getFanout() const{ return _fanoutList; }
  void setFanin(CirGate* ID, bool phase = false) { Pin inpin(ID, phase); _faninList.push_back(inpin); }
  void setFanout(CirGate* ID, bool phase = false) { Pin outpin(ID, phase); _fanoutList.push_back(outpin); }

  // Printing functions
  virtual void printGate() const = 0;
  void reportGate() const;
  void reportFanin(int level) const;
  void reportFanout(int level) const;

private:
protected:
  int _gateID;
  int _lineNO;
  bool _invPhase;
  string _symbols;

  vector<Pin> _faninList;
  vector<Pin> _fanoutList;
  static size_t _globalRef;
  size_t _ref;
};

class CirPiGate: public CirGate
{
public:
  CirPiGate(int ID = 0, int NO = 0): CirGate(ID, NO) {}
  void printGate() const { cout<<"h"<<endl; }
};

class CirPoGate: public CirGate
{
public:
  CirPoGate(int ID = 0, int NO = 0): CirGate(ID, NO) {}
  void printGate() const { cout<<"h"<<endl; }
};

class AndGate: public CirGate
{
public:
  AndGate(int ID = 0, int NO = 0): CirGate(ID, NO) {}
  void printGate() const { cout<<"h"<<endl; }
};

class UnDef: public CirGate
{
public:
  UnDef(int ID = 0): CirGate(ID, -1) {}
  void printGate() const { cout<<"h"<<endl; }
};


#endif // CIR_GATE_H