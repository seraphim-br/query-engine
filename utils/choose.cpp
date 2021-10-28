//---------------------------------------------------------------------------

#pragma hdrstop

#include <ctime>
#include <iostream>
#include <fstream>
#include <cstdlib>

//---------------------------------------------------------------------------

using namespace std;

#pragma argsused
int main(int argc, char* argv[])
{
	if(argc != 4){
		cout << "ChooseCity sintaxe : ChooseCity [Percent] [SourceFileName] [TargetFileName]" << endl;
		return 0;
	}//end if

   double perc = atof(argv[1]);
   if(perc>1){
   	cout << "Percent must be less than 1.0" << endl;
      return 0;
   }//end if

   //input file
   ifstream arq(argv[2]);
   if(!arq.is_open()){
		cout << "File not found" << endl;
      return 0;
   }
   arq.seekg(0);

   //output file
   ofstream arq1(argv[3]);
   char buf [40960];
   int i = 0;
   int k = 0;
   int num = 0;
   while (!arq.eof()){
      arq.getline(buf, 40960);
      num = rand();
      if((int)(num % 1000000) < (int)(perc*1000000)){
         //write R1
         arq1 << buf << endl;
         i++;
		}//end if
      //increment
      k++;
   }//end while
   arq.close();
   arq1.close();

	cout << "Number of Cities exists: " << k << endl;
	cout << "Number of Cities copied: " << i << endl;

   return 0;
}
//---------------------------------------------------------------------------

