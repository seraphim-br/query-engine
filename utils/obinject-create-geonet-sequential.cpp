#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <queue>

#include <br/edu/unifei/tree/PointRTree.h>
#include <br/edu/unifei/tree/File.h>
#include <br/edu/unifei/tree/MTree.h>
#include <br/edu/unifei/tree/RTree.h>
#include <br/edu/unifei/tree/Sequential.h>
#include <br/edu/unifei/tree/Point.h>

using namespace br::edu::unifei::tree;

using namespace std;

Int main(Int argc, char **argv) {

	cout << "Ponto 1";
	ifstream ifs("../data/geonet.txt");
	char strLat[50];
	char strLon[50];
	char *buf = new char[1024];
	char *tab, *nextTab;
	int i = 0;

	cout << "Ponto 2";

	fstream fileStream;
	fileStream.open("../data/geonet.txt");
	cout << "Ponto 3";
	if (!fileStream.fail()) {
		//creating sequential
		cout << "Come�a aqui";

		File *fileSeqNew = new File("../data/geonet.sequential", 8192);
		Sequential<Point> *seqNew = new Sequential<Point>(fileSeqNew);

		while (ifs.getline(buf, 1023)) {
			if (strlen(buf) > 0) {
				tab = buf;

				//lat
				tab = strchr(tab, '\t');
				nextTab = strchr(tab, '\t');
				strncpy(strLat, tab, (unsigned)(nextTab-tab));
				strLat[nextTab-tab+1]='\0';
				float lat = atof(tab);
				//lon
				tab = strchr(tab, '\t');
				nextTab = strchr(tab, '\t');
				strncpy(strLon, tab, (unsigned)(nextTab-tab));
				strLon[nextTab-tab+1]='\0';
				float lon = atof(tab);

				//point
				Point *pIns = new Point();
				pIns->setLatitude(lat);
				pIns->setLongitude(lon);
				seqNew->add(pIns);

				i++;
				if (i%100 == 0){
					cout << ".";
				}

				//clean home
				delete pIns;
			}
		}
		//clean home
		delete fileSeqNew;
		delete seqNew;
	} else {
		//clean home
		fileStream.close();
	}

	return 0;
}
