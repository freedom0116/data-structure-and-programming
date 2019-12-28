/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr(): _isSimulated(false) {}
   ~CirMgr() {} 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if(gid < _totalList.size() && _totalList[gid] != 0)
      return _totalList[gid];
      return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
   ofstream *_simLog;
   GateList _piList;
   GateList _poList;
   GateList _aigList;
   GateList _totalList;
   GateList _dfsList;
   vector<string> _commentList;

   bool _isSimulated;
   
   // funtions use in readCircuit
   bool readHeader(fstream&, vector<int>&);
   bool readInput(fstream&, int&);
   bool readOutput(fstream&, int&, vector<vector<int>>&);
   bool readAig(fstream&, int&, vector<vector<int>>&);
   bool readSymbol(fstream&, bool&);
   bool readComment(fstream&);
   bool cutPiece(string&, vector<string>&, int, bool&);
   void setFanIO(vector<vector<int>>&);

   // Depth-first search 
   void dfsTraversal(const GateList&);

   // For optimization
   bool checkFanin(CirGate*);
   bool OptExecute(int, CirGate*, CirGate* nonZero = 0);
   void updateAIG();
   void updateDfs();
};

#endif // CIR_MGR_H
