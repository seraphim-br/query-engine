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
#include "br/edu/unifei/tree/Sequential.h"
#include "br/edu/unifei/tree/Punctual.h"
#include "br/edu/unifei/tree/KFarthestSequential.h"
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
	fileStream.open("../data/geonet.sequential.4k");
	// if exists mtree
	if (!fileStream.fail()) {

		File *fileSeq = new File("../data/geonet.sequential.4k");
		Sequential<Point> *sequential = new Sequential<Point>(fileSeq);

		KFarthestSequential<Point> *kfarSeq1 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq2 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq3 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq4 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq5 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq6 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq7 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq8 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq9 = new KFarthestSequential<Point>(sequential);
		KFarthestSequential<Point> *kfarSeq10 = new KFarthestSequential<Point>(sequential);
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

//				cout << " point lat=" << pIns->getLatitude() << " Long=" << pIns->getLongitude() << endl;
				KFarthestSequential<Point>::Collection *res1 = kfarSeq1->execute(pQry, 100);
				KFarthestSequential<Point>::Collection *res2 = kfarSeq2->execute(pQry, 200);
				KFarthestSequential<Point>::Collection *res3 = kfarSeq3->execute(pQry, 300);
				KFarthestSequential<Point>::Collection *res4 = kfarSeq4->execute(pQry, 400);
				KFarthestSequential<Point>::Collection *res5 = kfarSeq5->execute(pQry, 500);
				KFarthestSequential<Point>::Collection *res6 = kfarSeq6->execute(pQry, 600);
				KFarthestSequential<Point>::Collection *res7 = kfarSeq7->execute(pQry, 700);
				KFarthestSequential<Point>::Collection *res8 = kfarSeq8->execute(pQry, 800);
				KFarthestSequential<Point>::Collection *res9 = kfarSeq9->execute(pQry, 900);
				KFarthestSequential<Point>::Collection *res10 = kfarSeq10->execute(pQry, 1000);
				//clean home
				delete res1;
				delete res2;
				delete res3;
				delete res4;
				delete res5;
				delete res6;
				delete res7;
				delete res8;
				delete res9;
				delete res10;
				delete pQry;
			}
			cout << endl;
			i++;
		}
		cout << "[1] disk:" << kfarSeq1->getPerformance()->measuredDiskAccess() << endl;
		cout << "[1] distance:" << kfarSeq1->getPerformance()->measuredVerifications() << endl;
		cout << "[1] time:" << kfarSeq1->getPerformance()->measuredTime() << endl;

		cout << "[2] disk:" << kfarSeq2->getPerformance()->measuredDiskAccess() << endl;
		cout << "[2] distance:" << kfarSeq2->getPerformance()->measuredVerifications() << endl;
		cout << "[2] time:" << kfarSeq2->getPerformance()->measuredTime() << endl;

		cout << "[3] disk:" << kfarSeq3->getPerformance()->measuredDiskAccess() << endl;
		cout << "[3] distance:" << kfarSeq3->getPerformance()->measuredVerifications() << endl;
		cout << "[3] time:" << kfarSeq3->getPerformance()->measuredTime() << endl;

		cout << "[4] disk:" << kfarSeq4->getPerformance()->measuredDiskAccess() << endl;
		cout << "[4] distance:" << kfarSeq4->getPerformance()->measuredVerifications() << endl;
		cout << "[4] time:" << kfarSeq4->getPerformance()->measuredTime() << endl;

		cout << "[5] disk:" << kfarSeq5->getPerformance()->measuredDiskAccess() << endl;
		cout << "[5] distance:" << kfarSeq5->getPerformance()->measuredVerifications() << endl;
		cout << "[5] time:" << kfarSeq5->getPerformance()->measuredTime() << endl;

		cout << "[6] disk:" << kfarSeq6->getPerformance()->measuredDiskAccess() << endl;
		cout << "[6] distance:" << kfarSeq6->getPerformance()->measuredVerifications() << endl;
		cout << "[6] time:" << kfarSeq6->getPerformance()->measuredTime() << endl;

		cout << "[7] disk:" << kfarSeq7->getPerformance()->measuredDiskAccess() << endl;
		cout << "[7] distance:" << kfarSeq7->getPerformance()->measuredVerifications() << endl;
		cout << "[7] time:" << kfarSeq7->getPerformance()->measuredTime() << endl;

		cout << "[8] disk:" << kfarSeq8->getPerformance()->measuredDiskAccess() << endl;
		cout << "[8] distance:" << kfarSeq8->getPerformance()->measuredVerifications() << endl;
		cout << "[8] time:" << kfarSeq8->getPerformance()->measuredTime() << endl;

		cout << "[9] disk:" << kfarSeq9->getPerformance()->measuredDiskAccess() << endl;
		cout << "[9] distance:" << kfarSeq9->getPerformance()->measuredVerifications() << endl;
		cout << "[9] time:" << kfarSeq9->getPerformance()->measuredTime() << endl;

		cout << "[10] disk:" << kfarSeq10->getPerformance()->measuredDiskAccess() << endl;
		cout << "[10] distance:" << kfarSeq10->getPerformance()->measuredVerifications() << endl;
		cout << "[10] time:" << kfarSeq10->getPerformance()->measuredTime() << endl;
		//clean home
		delete kfarSeq1;
		delete kfarSeq2;
		delete kfarSeq3;
		delete kfarSeq4;
		delete kfarSeq5;
		delete kfarSeq6;
		delete kfarSeq7;
		delete kfarSeq8;
		delete kfarSeq9;
		delete kfarSeq10;
	} else {
		//clean home
		fileStream.close();
	}

	return 0;
}
