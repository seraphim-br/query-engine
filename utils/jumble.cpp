
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstdlib>

using namespace std;

typedef map<int, string, less<int> > map_type;

typedef map_type::value_type value_type;

typedef map_type::iterator ite_type;

#define BUFF_SIZE 2048

//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if(argc != 3){
		cout << "jumble sintaxe : jumble [SourceFileName] [TargetFileName]" << endl;
		return 0;
	}//end if

   //input file
   ifstream source(argv[1]);
   if(!source.is_open()){
		cout << "File not found" << endl;
      return 0;
   }
   source.seekg(0);
   //count
   int count=0;
   //output file
   ofstream target(argv[2]);
   //map
   map_type myMap;
   //iterator
   ite_type iteMap;
   //read
   char * srcLine = new char[BUFF_SIZE];
   //string
   string strLine;
   bool r;
   //add line
   while (!source.eof()) {
      //read
      source.getline(srcLine, BUFF_SIZE);
      //copy to string
      strLine = srcLine;
      //add tree
      myMap.insert(value_type(count, strLine));
      //increment
      count++;
   }// end while
   //close
   source.close();
   //while list no is empty
   while (myMap.size() > 0){
      //sort a number
      int num = rand() % count;
      //find num
      if(r){
         iteMap = myMap.lower_bound(num);
         r=false;
      }else{
         iteMap = myMap.upper_bound(num);
         r=true;
      }
      if (iteMap != myMap.end()){
         //write target file
         target << (*iteMap).second << endl;
         //erase
         myMap.erase(iteMap);
      }//end if
   }//end for
   //close
   target.close();

   cout << "Number of lines copied: " << count << endl;

   return 0;
}
//---------------------------------------------------------------------------
