/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }
   // TODO read and execute commands
   // cout << "Enter command: ";

   while (true) {
      cout << "Enter command:";
      string nowcmd[3];
      //nowcmd = cin.getline(); cout << endl;
      cin >> nowcmd[0];
      if(nowcmd[0] == "PRINT"){
         json.print();
      }else if(nowcmd[0] == "SUM"){
         json.sum();
      }else if(nowcmd[0] == "AVG"){
         json.avg();
      }else if(nowcmd[0] == "MAX"){
         json.max();
      }else if(nowcmd[0] == "MIN"){
         json.min();
      }else if(nowcmd[0] == "ADD"){
         cin >> nowcmd[1] >> nowcmd[2];
         json.add(nowcmd[1], nowcmd[2]);
      }else if(nowcmd[0] == "EXIT"){
         break;
      }
   }
}
