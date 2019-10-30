/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   if(!option.size()) return CmdExec::errorOption(CMD_OPT_MISSING, option);
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   int numObj;
   if(!myStr2Int(options[0], numObj)) 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   if(numObj <= 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   // if numObj is int > 0
   if(options.size() == 1){
         mtest.newObjs(numObj);
   }else if(options.size() == 3){
      if(myStrNCmp("-Array", options[1], 1) != 0) 
         return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
      int arraySize;
      if(!myStr2Int(options[2], arraySize))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      if(arraySize <= 0) 
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      try{
         mtest.newArrs(numObj, arraySize);
      }
      catch(bad_alloc){ // have problem
         return CMD_EXEC_ERROR;
      }
   }else 
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[options.size() - 1]);
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   if(!option.size()) 
      return CmdExec::errorOption(CMD_OPT_MISSING, option);
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   int opt;
   if(myStrNCmp("-Index", options[0], 1) == 0) opt = 0;
   else if(myStrNCmp("-Random", options[0], 1) == 0) opt = 1;
   else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   
   int numObj;
   if(options.size() >= 2){
      if(!myStr2Int(options[1], numObj)) 
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      if(numObj < 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         // if numObj is int > 0
   }else return CmdExec::errorOption(CMD_OPT_MISSING, "");

   if(options.size() == 2){ // objList
      if(opt == 0){
         if(numObj >= mtest.getObjListSize()){
            cerr << "Size of object list ("<< mtest.getObjListSize() 
            <<") is <= "<< numObj << "!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         }
         mtest.deleteObj(numObj);
      }else{
         if(numObj == 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         for(int i = 0; i < numObj; i++){
            mtest.deleteObj(rnGen(mtest.getObjListSize()));
         }
      }
   }else if(options.size() == 3){ // arrList
      if(myStrNCmp("-Array", options[2], 1) != 0)
         return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);
      if(opt == 0){
         if(numObj >= mtest.getArrListSize()){
            cerr << "Size of array list ("<< mtest.getArrListSize() 
            <<") is <= "<< numObj << "!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         }
         mtest.deleteArr(numObj);
      }else{
         if(numObj == 0) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         for(int i = 0; i < numObj; i++){
            mtest.deleteArr(rnGen(mtest.getArrListSize()));
         }
      }
      
   }else return CmdExec::errorOption(CMD_OPT_EXTRA, options[options.size() - 1]);
   
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


