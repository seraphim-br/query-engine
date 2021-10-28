//---------------------------------------------------------------------------

#pragma hdrstop

#include <ctime>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

using namespace std;

//---------------------------------------------------------------------------

#pragma argsused
int main(int argc, char* argv[])
{
	if(argc != 3){
		cout << "cleanCity sintaxe : cleanCity [SourceFileName] [TargetFileName]" << endl;
		return 0;
	}//end if

   //input file
   ifstream in(argv[1]);
   if(!in.is_open()){
		cout << "File not found" << endl;
      return 0;
   }
   in.seekg(0);

   //output file
   ofstream out(argv[2]);
   char *buf = new char[1024];
   char strLat[50];
   char strLon[50];
   double lat, lon;
   char * tab, * nextTab;
   long long k = 0;
   long long i = 0;
   while (in.getline(buf, 1023)){
      if(strlen(buf)>0){

         tab = buf;
         for (int j=0;j<3;j++){
            tab = strchr(tab, '\t');
            if (tab!=NULL)
                tab++;
            else
                break;
         }//endfor

         if(tab!=NULL){
            nextTab = strchr(tab, '\t');
            if((nextTab!=NULL)&&((nextTab-tab)<50)) {
               strncpy(strLat, tab, (unsigned)(nextTab-tab));
               strLat[nextTab-tab+1]='\0';
               lat = atof(strLat);
               tab = nextTab+1;

               if(tab!=NULL){
                  nextTab = strchr(tab, '\t');
                  if((nextTab!=NULL)&&((nextTab-tab)<50)) {
                     strncpy(strLon, tab, (unsigned)(nextTab-tab));
                     strLat[nextTab-tab+1]='\0';
                     lon = atof(strLon);

                     //write
                     if((lat!=0.0)&&(lon!=0.0)){
                        out << i << '\t' << lat << '\t' << lon << endl;
                        //increment
                        i++;
                     }//endif
                  }//endif
               }//endif
            }//endif
         }//endif
      }//endif
      //increment
      k++;
   }//end while

   in.close();
   out.close();

	cout << "Number of Cities exists: " << k << endl;
	cout << "Number of Cities copied: " << i << endl;

   return 0;
}
//---------------------------------------------------------------------------

