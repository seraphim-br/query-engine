#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <queue>

#include <br/edu/unifei/tree/PointRTree.h>
#include <br/edu/unifei/tree/File.h>
#include <br/edu/unifei/tree/MTree.h>
#include <br/edu/unifei/tree/RTree.h>
#include <br/edu/unifei/tree/Punctual.h>
#include <br/edu/unifei/tree/KFarthest.h>
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
	// if exists mtree
	cout << "Ponto 3";
	if (!fileStream.fail()) {
		//creating mtree
//		File *fileMTreeNew = new File("../data/geonet.mtree", 8192);
//		MTree<Point> *mtreeNew = new MTree<Point>(fileMTreeNew);
//		Punctual<Point> *punct = new Punctual<Point>(mtreeNew);
		cout << "Começa aqui";

		File *fileRTreeNew = new File("../data/geonet.rtree", 8192);
		RTree<PointRTree> *rtreeNew = new RTree<PointRTree>(fileRTreeNew);

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
//				Point *pIns = new Point();
//				pIns->setLatitude(lat);
//				pIns->setLongitude(lon);
//				mtreeNew->add(pIns);

//				Uuid *uuidFind = punct->execute(pIns);
//				if (uuidFind == NULL) {
//					cout << "FAIL IN INSERTION: lat=" << pIns->getLatitude()
//							<< " Long=" << pIns->getLongitude() << endl;
//				}

				//pointTTree
				PointRTree *pRtree = new PointRTree();
				pRtree->setOrigin(0, lat);
				pRtree->setOrigin(1, lon);
				rtreeNew->add(pRtree);
				i++;
				if (i%100 == 0){
					cout << ".";
				}

				//clean home
				delete pRtree;
//				delete uuidFind;
//				delete pIns;
			}
		}
		//clean home
//		delete fileMTreeNew;
//		delete mtreeNew;
		delete fileRTreeNew;
		delete rtreeNew;
	} else {
		//clean home
		fileStream.close();
	}

	return 0;
}
