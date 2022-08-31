#include <br/edu/unifei/tree/File.h>
#include <br/edu/unifei/tree/Point.h>
#include <br/edu/unifei/tree/PointRTree.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <list>

#include "br/edu/unifei/tree/MTree.h"
#include "br/edu/unifei/tree/RTree.h"
#include "br/edu/unifei/tree/Punctual.h"
#include "br/edu/unifei/tree/KFarthestRTree.h"
#include "br/edu/unifei/tree/KFarthest.h"
#include "br/edu/unifei/tree/KFarthestNearest.h"

using namespace br::edu::unifei::tree;

using namespace std;

Int main(Int argc, char **argv) {

	cout << "COMECA AQUI" << endl;

	ifstream ifs("../data/kfarthest.txt");
	char strLat[50];
	char strLon[50];
	char *buf = new char[1024];
	char *tab, *nextTab;

	fstream fileStream;
	fileStream.open("../data/geonet.mtree.4k");
	// if exists mtree
	if (!fileStream.fail()) {
		//creating mtree
		File *fileMtree = new File("../data/geonet.mtree.4k");
		MTree<Point> *mtree = new MTree<Point>(fileMtree);

		File *fileRtree = new File("../data/geonet.rtree.4k");
		RTree<PointRTree> *rtree = new RTree<PointRTree>(fileRtree);

		KFarthestRTree<PointRTree> *kfarRTree = new KFarthestRTree<PointRTree>(rtree);
		KFarthestNearest<Point> *kfarNear = new KFarthestNearest<Point>(mtree);
		list<Point*> notFoundList;
		int i = 0;
		cout << "WHILE COMECA AQUI" << endl;
		while (ifs.getline(buf, 1023)) {
			cout << i << " -- ";
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
				Point *pQry = new Point();
				pQry->setLatitude(lat);
				pQry->setLongitude(lon);
				PointRTree *pQryRTree = new PointRTree();
				pQryRTree->setOrigin(0, lat);
				pQryRTree->setOrigin(1, lon);

//				cout << " point lat=" << pIns->getLatitude() << " Long=" << pIns->getLongitude() << endl;
				KFarthestRTree<PointRTree>::Collection *res1 = kfarRTree->execute(pQryRTree, 1);
				KFarthestNearest<Point>::Collection *res2 = kfarNear->execute(pQry, 1);

				//checking resulted equality
				if(res1->size()==res2->size()){
					while (!res1->empty()) {
						PointRTree *obj1 = res1->top();
						Point *obj2 = NULL;
						res1->pop();
						while (!res2->empty()) {
							Point *obj2 = res2->top();
							res2->pop();
							if((obj1->getOrigin(0) == obj2->getLatitude()) &&
									(obj1->getOrigin(1) == obj2->getLongitude())){
								break;
							} else {
								notFoundList.push_back(obj2);
							}
						}
						while (!notFoundList.empty()) {
							Point *obj3 = notFoundList.front();
							notFoundList.pop_front();
							res2->push(obj3);
						}
						if (res1->size()!=res2->size()){
							cout << "Listas diferentes";
							while (!res1->empty()) {
								PointRTree *obj1 = res1->top();
								res1->pop();
								delete obj1;
							}
							while (!res2->empty()) {
								Point *obj2 = res2->top();
								res2->pop();
								delete obj2;
							}
							break;
						} else {
							delete obj1;
							delete obj2;
						}
					}
				}else{
					cout << "Diferente tamanho da lista resultado";
				}
				//clean home
				delete res1;
				delete res2;
				delete pQry;
			}
			cout << endl;
			i++;
		}
		cout << "[kfarRTree] disk:" << kfarRTree->getPerformance()->measuredDiskAccess() << endl;
		cout << "[kfarRTree] distance:" << kfarRTree->getPerformance()->measuredVerifications() << endl;
		cout << "[kfarRTree] time:" << kfarRTree->getPerformance()->measuredTime() << endl;
		cout << "[kfar near] disk:" << kfarNear->getPerformance()->measuredDiskAccess() << endl;
		cout << "[kfar near] distance:" << kfarNear->getPerformance()->measuredVerifications() << endl;
		cout << "[kfar near] time:" << kfarNear->getPerformance()->measuredTime() << endl;
		//clean home
		delete kfarRTree;
		delete fileMtree;
		delete mtree;
	} else {
		//clean home
		fileStream.close();
	}

	return 0;
}
