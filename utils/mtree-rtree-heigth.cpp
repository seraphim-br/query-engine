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
	fstream fileStream;
	fileStream.open("../data/geonet.mtree");
	// if exists mtree
	if (!fileStream.fail()) {
		//creating mtree
		File *fileMtree = new File("../data/geonet.mtree.4k");
		MTree<Point> *mtree = new MTree<Point>(fileMtree);

		File *fileRtree = new File("../data/geonet.rtree.4k");
		RTree<PointRTree> *rtree = new RTree<PointRTree>(fileRtree);

		cout << "[MTREE Heigth]:" << mtree->height() << endl;
		cout << "[RTREE Heigth]:" << rtree->height() << endl;
		//clean home
		delete fileMtree;
		delete fileRtree;
		delete mtree;
		delete rtree;
	} else {
		//clean home
		fileStream.close();
	}

	return 0;
}
