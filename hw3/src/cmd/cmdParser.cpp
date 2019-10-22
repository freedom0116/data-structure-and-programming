/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************_readBufPtr = _readBufEnd;
               while(true){ // Ptr move to first
                  *_readBufPtr = '\0';
                  if(_readBufPtr == _readBuf){
                     cout << " ";
                     break;
                  }
                  cout << " " << '\b' << '\b';
                  _readBufPtr -= 1;
               }
               _readBufPtr = _readBufEnd = _readBuf;
               *_readBufPtr = 0;
               for(int i = 0; i < strs[0].size(); i++){ // print
                  cout << strs[0][i];
                  *_readBufPtr = strs[0][i];
                  _readBufPtr += 1;
                  _readBufEnd += 1;
               }
               cout << " ";
               *_readBufPtr = ' ';
               _readBufPtr += 1;
               _readBufEnd += 1;
               for(int i = 0, n = filenames[0].size(); i < n; i++){
                  *_readBufPtr = filenames[0][i];
                  cout << filenames[0][i];
                  _readBufPtr += 1;
                  _readBufEnd += 1;
               }
               cout << " ";
               *_readBufPtr = ' ';
               _readBufPtr += 1;
               _readBufEnd += 1;************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdio.h>
#include <sys/types.h>  // these use as show filename
#include <dirent.h>     //
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();
vector<int> _end, _cur;

void rePrint(char*& ptr, char*& end, vector<char> origin, int ptrpos){
   for(int i = 0, n = origin.size(); i < n; i++){
      *end = origin[i];
      cout << origin[i];
      ptr += 1;
      end += 1;
   }
   for(int i = 0, n = (origin.size() - ptrpos); i < n; i++){
      cout << '\b';
      ptr -= 1;
   }
}
//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
   _dofile = new ifstream(dof.c_str());
   _dofileStack.push(_dofile);
   if(!(*_dofile)){
      closeDofile();
      return false;
   }
   if(_dofileStack.size() >1024){
      closeDofile();
      return false;
   }
   return true;
/*   //find eof
   _dofile->seekg(0,ios::end);       // go to position to 0 before the end of _dofile
   _end.push_back(_dofile->tellg()); // set the end position
   _dofile->seekg(0,ios::beg);       // go back to opsition to 0 after the begin of _dofile
   _cur.push_back(_dofile->tellg());

   while(_dofile->tellg() != _end[_end.size()-1]){
      _cur[_end.size()-1] = _dofile->tellg();
      execOneCmd();
      _dofile = _dofileStack.top();
      _dofile->tellg() = _cur[_end.size()-1];
   }
   _cur.pop_back();
   _end.pop_back();
   _dofileStack.pop();*/
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
   assert(_dofile != 0);
   // TODO...
   delete _dofile;
   _dofile = 0;
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...
   CmdMap h = _cmdMap;
   for(CmdMap::iterator iter = h.begin();
   iter != h.end(); iter++){
      iter->second->help();
   }   
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   // TODO...
   string str = _history.back();
   string cmd;
   size_t n = myStrGetTok(str, cmd);
   if (n == string::npos){
      CmdExec* e = getCmd(str);
      if(e == 0){
         cerr << "Illegal command!! (" << cmd << ")" << endl;
         return e = 0;         
      }
      return e;
   }

   CmdExec* e = getCmd(cmd);
   if(e != 0){
      string opt = str.substr(n+1, str.length()-1);
      option = opt;
      return e;
   }else{
      cerr << "Illegal command!! (" << cmd << ")" << endl;
      return e = 0;
   }
   assert(str[0] != 0 && str[0] != ' ');
   return NULL;
}

// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the plexSiartial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this ise] Cursor to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    // e.g. in hw3/ref
//    cmd> help mydb-$orld
//    ==> DO NOT print the matched files
//    ==> If "tab" is pressed again, see 6.2
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    // e.g. in hw3/bin
//    cmd> help mydb $
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
void
CmdParser::listCmd(const string& str)
{
   // TODO...
   // not finish yet
   int option = 1;
   int cmdsize = 13;
   string cmd[13] = { "DBAPpend", "DBAve", "DBCount", "DBMAx", "DBMIn",
   "DBPrint", "DBRead", "DBSOrt", "DBSUm", "DOfile", "HELp", "HIStory", "Quit" };
   
   // 1
   for(int i = 0; i < str.size(); i++){
      if(str[i] != ' '){
         option = 0;
         break;
      }
   }
   // manage date
   vector<string> strs;
   string token;
   size_t n = myStrGetTok(str, token);   
   if (n == string::npos) strs.push_back(str);
   else{
      strs.push_back(token);
      string str2 = str.substr(n+1, str.length()-1);
      strs.push_back(str2);
   }
   // store data for reprint
   vector<char> origin;
   int ptrpos, cou = 0;
   while((_readBuf + cou) != _readBufEnd){ // store the date for re-print command
      origin.push_back(*(_readBuf + cou));
      cou++;
      if((_readBuf + cou) == _readBufPtr) {
         ptrpos = cou;
      }
   }
   // 2,3,4
   vector<int> match;
   vector<string> filenames;
   if(option == 0){
      for(int i = 0; i < cmdsize; i++){
         char c, f;
         for(int j = 0, n = cmd[i].size(); j < n; j++){
            isupper(strs[0][j])? c = tolower(strs[0][j]) : c = strs[0][j];
            isupper(cmd[i][j])? f = tolower(cmd[i][j]) : f = cmd[i][j];
            if(c != f) break;
            if(j == strs[0].size() - 1){
               if(strs[0].size() == cmd[i].size()) option = 5;
               match.push_back(i); // input matched command into output array
               break;
            }
         }
      }
      if(option == 5){
         if(_tabPressCount > 1){
            string filepath = "/home/freedom0116/Desktop/mytest"; // "."
            vector<string> files;
            DIR *dp; // 創立資料夾指標
            struct dirent *dirp;
            dp = opendir(filepath.c_str());
            while((dirp = readdir(dp)) != NULL){ // 如果dirent指標非空
               filenames.push_back(string(dirp->d_name)); // 將資料夾和檔案名放入vector
            }
            closedir(dp);
            if(*(_readBufPtr - 1) == ' ')
               option = 61;
            else{
               option = 62; 
            }
         }
      }
      else if(match.size() == 0) option = 4;
      else if(match.size() == 1 && option == 0){
         option = 3;
      }else{
         option = 2;
      }
   }
   switch(option){
      case 1:{
         cout << endl;
         for(int i = 0; i < cmdsize; i++){
            cout << setw(12) << left << cmd[i];
            if(i%5 == 4 && i != (n - 1)) cout << endl;
         }
         cout << endl; cout << endl;
         resetBufAndPrintPrompt();
         rePrint(_readBufPtr, _readBufEnd, origin, ptrpos);
         break;
      }
      case 2:{
         cout << endl;
         for(int i = 0, n = match.size(); i < n; i++){
            cout << setw(12) << left << cmd[match[i]];
            if(i%5 == 4 && i != n - 1) cout << endl;
         }
         cout << endl; cout << endl;
         resetBufAndPrintPrompt();
         rePrint(_readBufPtr, _readBufEnd, origin, ptrpos);
         break;
      }
      case 3:{
         for(int i = 0, n = cmd[match[0]].size() - strs[0].size(); i <= n; i++){
            int c = 0;
            while((_readBufEnd - c) != _readBufPtr){
               *(_readBufEnd - c) = *(_readBufEnd - c -1);
               c++;
               if((_readBufEnd - c) == _readBufPtr) 
                  *(_readBufEnd - c) = *(_readBufEnd - c -1);
            }
            if(i == n){
               *(_readBufEnd - c) = ' ';
            }
            else{
               *(_readBufEnd - c) = cmd[match[0]][strs[0].size() + i];
            }
            cout << *(_readBufEnd - c);
            _readBufEnd += 1;
            _readBufPtr += 1;
         }
         int c = 0;
         while((_readBufEnd - c) != _readBufPtr){
            cout << '\b';
            c++;
         }
        _tabPressCount = 0;
         break;
      }
      case 4:{
         mybeep();
         break;   
      }
      case 5:{
         CmdExec* e = getCmd(cmd[match[0]]);
         cout << endl;
         e->usage(cout);
         cout << endl;

         _readBufPtr = _readBufEnd = _readBuf;
        *_readBufPtr = 0;
         printPrompt();
         rePrint(_readBufPtr, _readBufEnd, origin, ptrpos);
         break;
      }
      case 61:{
         for(vector<string>::iterator i = filenames.begin(); i != filenames.end(); ){
            if(*i == "." | *i == "..")
               filenames.erase(i);
            else
               i++;
         }

         if(filenames.size() == 0) // no file in folder
            mybeep();
         else if(filenames.size() == 1){ // one file in folder
               _readBufPtr = _readBufEnd;
               while(true){ // Ptr move to first
                  *_readBufPtr = '\0';
                  if(_readBufPtr == _readBuf){
                     cout << " ";
                     break;
                  }
                  cout << " " << '\b' << '\b';
                  _readBufPtr -= 1;
               }
               _readBufPtr = _readBufEnd = _readBuf;
               *_readBufPtr = 0;
               for(int i = 0; i < strs[0].size(); i++){ // print
                  cout << strs[0][i];
                  *_readBufPtr = strs[0][i];
                  _readBufPtr += 1;
                  _readBufEnd += 1;
               }
               cout << " ";
               *_readBufPtr = ' ';
               _readBufPtr += 1;
               _readBufEnd += 1;
               for(int i = 0, n = filenames[0].size(); i < n; i++){
                  *_readBufPtr = filenames[0][i];
                  cout << filenames[0][i];
                  _readBufPtr += 1;
                  _readBufEnd += 1;
               }
               cout << " ";
               *_readBufPtr = ' ';
               _readBufPtr += 1;
               _readBufEnd += 1;
         }
         else{ // more than one file in folder
            int num = 0; // check same filename length
            for(int i = 0, n = filenames[0].size(); i < n; i++){
               for(int j = 1, m = filenames.size(); j < m; j++){
                  if(filenames[0][i] == filenames[j][i]) num = i;
                  else break;
               }
               if(num != i) break;
            }
            if(num == 0){ // don't have common prefix in folder
               cout << endl;
               filenames.push_back(".");
               filenames.push_back("..");
               for(int i = 0, n = filenames.size(); i < n; i++){
                  cout << setw(16) << left << filenames[i];
                  if(i%5 == 4) cout << endl;
               }
               cout << endl; cout << endl;
               
               _readBufPtr = _readBufEnd = _readBuf;
               *_readBufPtr = 0;
               printPrompt();
               rePrint(_readBufPtr, _readBufEnd, origin, ptrpos);
            }
            else{ // have common prefix
               for(int i = 0; i <= num; i++){
                  int c = -2;
                  while((_readBufEnd - c) != _readBufPtr){
                     c++;
                     *(_readBufEnd - c) = *(_readBufEnd - c -1);
                  }
                  if(i == n){
                     *(_readBufEnd - c) = '-';
                  }
                  else{
                     *(_readBufEnd - c) = filenames[0][i];
                  }
                  cout << *(_readBufEnd - c);
                  _readBufEnd += 1;
                  _readBufPtr += 1;
               }
               int c = 0;
               cout << _readBufPtr;
               while((_readBufEnd - c) != _readBufPtr){
                  cout << '\b';
                  c++;
               }
               mybeep();
            }
         }
         break;
      }
      case 62:{
         int num = 0; // check same filename length
         char c, f;
         vector<char> samefile;
         for(int i = 0; i < filenames.size(); i++){
            for(int j = 0; j < strs[1].size(); j++){
               isupper(strs[1][j])? c = tolower(strs[1][j]) : c = strs[1][j];
               isupper(cmd[i][j])? f = tolower(cmd[i][j]) : f = cmd[i][j];
               if(c != f) break;
               if(j == (strs[1].size() - 1)) samefile.push_back(i);
            }
         }
         if(samefile.size() == 0){
            mybeep();
         }else if(samefile.size() == 1){
              for(int i = 0, n = filenames[samefile[0]].size() - strs[1].size(); i <= n; i++){
               int c = 0;
               while((_readBufEnd - c) != _readBufPtr){
                  *(_readBufEnd - c) = *(_readBufEnd - c -1);
                  c++;
                  if((_readBufEnd - c) == _readBufPtr) 
                     *(_readBufEnd - c) = *(_readBufEnd - c -1);
               }
               if(i == n){
                  *(_readBufEnd - c) = ' ';
               }
               else{
                  *(_readBufEnd - c) = filenames[samefile[0]][strs[1].size() + i];
               }
               cout << *(_readBufEnd - c);
               _readBufEnd += 1;
               _readBufPtr += 1;
            }
            int c = 0;
            while((_readBufEnd - c) != _readBufPtr){
               cout << '\b';
               c++;
            }
         }else{
            for(vector<string>::iterator i = filenames.begin(); i != filenames.end(); ){
               if(*i == "." | *i == "..")
                  filenames.erase(i);
               else
                  i++;
            }
            for(int i = 0; i < filenames.size(); i++){
               for(int j = 0; j <= num; j++){
               }
            }
         }
         break;
      }
   }
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   // TODO...
   for(CmdMap::iterator iter = _cmdMap.begin();
   iter != _cmdMap.end(); iter++){
      string c = iter->first;
      for(int i = 0, n = cmd.length(); i < n; i++){
         if(isupper(cmd[i])) cmd[i] = tolower(cmd[i]);
         if(i < c.length()){
            c[i] = tolower(c[i]);
            if(cmd[i] != c[i]) break;
         }else{
            int j = i - c.length();
            if(cmd[i] != iter->second->getOptCmd()[j]) break;
         }
         if(i == n-1){
            if(cmd.size() < c.size()) return e;
            e = iter->second;
            return e;
         }
      }
   }
   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

