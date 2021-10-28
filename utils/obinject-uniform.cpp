#include <iostream>
#include <cstdlib>
#include <queue>

#include "br/edu/unifei/tree/File.h"
#include "br/edu/unifei/tree/MTree.h"
#include "br/edu/unifei/tree/Punctual.h"
#include "br/edu/unifei/tree/KFarthest.h"
#include "br/edu/unifei/tree/Point.h"

using namespace br::edu::unifei::tree;

using namespace std;

Int main(Int argc, char **argv) {

    ofstream ofs("../data/geonet.txt");
    char strLat[50];
    char strLon[50];
    float lat, lon;

	fstream fileStream;
	fileStream.open("../data/geo.mtree");
	// if exists mtree
	if (fileStream.fail()) {
		//creating mtree
		File *fileNew = new File("../data/geonet.mtree", 4096);
		MTree<Point> *mtreeNew = new MTree<Point>(fileNew);
		Punctual<Point> *punct = new Punctual<Point>(mtreeNew);
		for (Int i = 0; i < 1000; i++) {
			for (Int j = 0; j < 1000; j++) {
				Point *pIns = new Point();
				//			p->setLatitude(((float)(rand()%31415))/10000);
				//			p->setLongitude(((float)(rand()%15707))/10000);
				pIns->setLatitude((float) i);
				pIns->setLongitude((float) j);
				mtreeNew->add(pIns);
				Uuid *uuidFind = punct->execute(pIns);
				if (uuidFind == NULL) {
					cout << "FAIL IN INSERTION: lat=" << pIns->getLatitude()
							<< " Long=" << pIns->getLongitude() << endl;
				}
				//clean home
				delete uuidFind;
				delete pIns;
			}
		}
		//clean home
		delete fileNew;
		delete mtreeNew;
	} else {
		//clean home
		fileStream.close();
	}

	File *fileOpen = new File("../data/geo.mtree");
	MTree<Point> *mtreeOpen = new MTree<Point>(fileOpen);
	Point *pQry = new Point();
	for (Int i = 0; i < 100; i++) {
		for (Int j = 0; j < 100; j++) {
			pQry->setLatitude((float) i);
			pQry->setLongitude((float) j);
			KFarthest<Point> *knn = new KFarthest<Point>(mtreeOpen);

			KFarthest<Point>::Collection *res = knn->execute(pQry, 6);
			if (res->empty()) {
				cout << "nenhum resultado" << endl;
			} else {
				cout << "verification: "
						<< knn->getPerformance()->getTotalVerifications()
						<< endl;
				while (!res->empty()) {
					Point *obj = res->top();
//					cout << "lat=" << obj->getLatitude() << " Long="
//							<< obj->getLongitude() << endl;
					res->pop();
					delete obj;
				}
			}
			delete res;
			delete knn;
		}
	}
	//clean home
	delete fileOpen;
	delete mtreeOpen;
	delete pQry;

	return 0;
}
