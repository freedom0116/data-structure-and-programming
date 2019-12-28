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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class Pin;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class Pin
{
public:
  Pin(CirGate* ID = 0, bool phase = false): _pin(ID), _invPhase(phase) {}
  ~Pin() {}
  CirGate* getPin() const { return _pin; }
  bool getInvPhase() const { return _invPhase; }
  void setPin(CirGate* n){ _pin = n; }
  void setPhase(bool p){ _invPhase = p; }

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

  // Operator
  bool operator == (CirGate*& gate) const { return _gateID == gate->getGateID(); }
  bool operator != (CirGate*& gate) const { return _gateID != gate->getGateID(); }

  //  For depth-first search
  static void setGlobalRef() { _globalRef++; }
  void setToGlobalRef() const { _ref = _globalRef; }
  bool isGlobalRef() const { return (_ref == _globalRef); }
  void dfsTraversal(GateList&);

  // Basic access methods
  virtual string getTypeStr() const = 0;
  virtual bool isAig() const { return false; }
  int getGateID() const { return _gateID; }
  void setGateID(int id) { _gateID = id; }
  unsigned getLineNo() const { return _lineNO; }
  bool getInvPhase() const { return _invPhase; }
  const string& getSymbols() const { return _symbols; }
  void setSymbols(string s) { _symbols = s; }

  // Fanin/out access methods
  vector<Pin*> getFanin() const { return _faninList; }
  vector<Pin*> getFanout() const { return _fanoutList; }
  void setFanin(CirGate* ID, bool phase = false)
    { Pin* inpin = new Pin(ID, phase); _faninList.push_back(inpin); }
  void setFanout(CirGate* ID)
    { Pin* outpin = new Pin(ID, false); _fanoutList.push_back(outpin); }
  void removeFanout(int it){ _fanoutList.erase(_fanoutList.begin()+it); }
  void removeFanin(int it){ _faninList.erase(_faninList.begin()+it); }

  // Printing functions
  virtual void printGate() const = 0;
  virtual void printPin() const = 0;
  void reportGate() const;
  void reportFanin(int level) const;
  void reportFanout(int level) const;
  void faninTraversal(const int& level, int dist) const;
  void fanoutTraversal(const int& level, int dist) const;

  // For sweep / optimization
  void sweep();

  // For optimization
  // void checkFanin();
  void faninOne(CirGate* one, CirGate* replace);
  void faninZero(CirGate* zero, CirGate* eliminate);
  void sameFanin(CirGate* inGate);
  void inverseFanin(CirGate* zero, CirGate* inGate);
  

private:
protected:
  int _gateID;
  int _lineNO;
  bool _invPhase;
  string _symbols;

  vector<Pin*> _faninList;
  vector<Pin*> _fanoutList;
  static size_t _globalRef;
  mutable size_t _ref;
};

class CirPiGate: public CirGate
{
public:
  CirPiGate(int ID = 0, int NO = 0): CirGate(ID, NO) {}
  string getTypeStr() const{ return "PI"; }
  void printGate() const;
  void printPin() const;
};

class CirPoGate: public CirGate
{
public:
  CirPoGate(int ID = 0, int NO = 0): CirGate(ID, NO) {}
  string getTypeStr() const{ return "PO"; }
  void printGate() const;
  void printPin() const;
};

class AndGate: public CirGate
{
public:
  AndGate(int ID = 0, int NO = 0): CirGate(ID, NO) {}
  string getTypeStr() const{ return "AIG"; }
  bool isAig() const { return true; }
  void printGate() const;
  void printPin() const;
};

class UnDef: public CirGate
{
public:
  UnDef(int ID = 0): CirGate(ID, 0) {}
  string getTypeStr() const{ return "UNDEF"; }
  void printGate() const;
  void printPin() const;
};

class Const0: public CirGate
{
public:
  Const0(): CirGate(0, 0) {}
  string getTypeStr() const{ return "CONST0"; }
  void printGate() const;
  void printPin() const;
};

#endif // CIR_GATE_H
