/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef INFILL_H
#define INFILL_H
#include "utils/logoutput.h"
#include "utils/gettime.h"

void generateConcentricInfill(Polygons outline, Polygons& result, int offsets[], int offsetsSize)
{
    int step = 0;
    while(1)
    {
        for(unsigned int polygonNr=0; polygonNr<outline.size(); polygonNr++)
            result.push_back(outline[polygonNr]);
        ClipperLib::OffsetPolygons(outline, outline, -offsets[step], ClipperLib::jtSquare, 2, false);
        if (outline.size() < 1)
            break;
        step = (step + 1) % offsetsSize;
    }
}

int compare_int64_t(const void* a, const void* b)
{
    int64_t n = (*(int64_t*)a) - (*(int64_t*)b);
    if (n < 0) return -1;
    if (n > 0) return 1;
    return 0;
}

void generateLineInfill(Polygons outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation)
{
log("original outline.size %u \n",outline.size());

    ClipperLib::OffsetPolygons(outline, outline, extrusionWidth * infillOverlap / 100, ClipperLib::jtSquare, 2, false);
    PointMatrix matrix(rotation);
    
log("offset outline.size %u \n",outline.size());


    matrix.apply(outline);
    
    AABB boundary(outline);
    
    int lineCount = (boundary.max.X - boundary.min.X + (lineSpacing - 1)) / lineSpacing;
    //vector<int64_t> cutList[lineCount];
    //vector< int64_t> *cutList = new vector<int64_t>[lineCount];
std::vector< std::vector<int64_t> > cutList (lineCount);
//cutList.reserve(lineCount);
   //vector< vector<int64_t> > cutList ;
    //vector<int64_t> cutList[lineCount];
    //vector<int64_t> cutList[lineCount];
//std::vector<int64_t> cutList(lineCount);
    //vector<int64_t> *cutList = new vector<int64_t>[lineCount];
	//int64_t *cutList = new int64_t[lineCount];
//cutList.reserve(lineCount);
    //TODO: fix this correctly
    log("outline.size %u, lineCount %u \n",outline.size(), lineCount);
    for(unsigned int polyNr=0; polyNr < outline.size(); polyNr++)
    {
	double t = getTime();
	//log("    Handling poly %u out of %u \n", polyNr+1, outline.size());
        Point p1 = outline[polyNr][outline[polyNr].size()-1];
        for(unsigned int i=0; i < outline[polyNr].size(); i++)
        {
		//log("     Handling poly outline %u out of %u \n", i+1,outline[polyNr].size());
            Point p0 = outline[polyNr][i];
		//log("     DONE Handling\n");
            int idx0 = (p0.X - boundary.min.X) / lineSpacing;
            int idx1 = (p1.X - boundary.min.X) / lineSpacing;
            int64_t xMin = p0.X, xMax = p1.X;
            if (p0.X > p1.X) { xMin = p1.X; xMax = p0.X; }
            if (idx0 > idx1) { int tmp = idx0; idx0 = idx1; idx1 = tmp; }
            for(int idx = idx0; idx<=idx1; idx++)
            {
                int x = (idx * lineSpacing) + boundary.min.X + lineSpacing / 2;
                if (x < xMin) continue;
                if (x >= xMax) continue;
                int y = p0.Y + (p1.Y - p0.Y) * (x - p0.X) / (p1.X - p0.X);
                cutList[idx].push_back(y);
            }
            p1 = p0;
        }
        log("Poly loop done in %5.3fs\n", timeElapsed(t));
    }
    log("    Done with pre treating polys\n");
    int idx = 0;
    int64_t x = (boundary.min.X + lineSpacing / 2);
    int64_t max = boundary.max.X;
int64_t totalNb = boundary.max.X/lineSpacing;

    //log("Total %u \n", totalNb);
    log("x %d \n", x);
    log("boundary minX %d, maxX %d\n",boundary.min.X, max);
    log("LineSpacing %u \n", lineSpacing);
    log("Condition is true? %u \n", (x<boundary.max.X));
    log("Condition is true? %u \n", (1>3));

    for(int64_t x = (boundary.min.X + lineSpacing / 2); x < boundary.max.X; x += lineSpacing)
    {
	log("Blablabla\n");
	break;
	//log("     QSorting boundary %u out of %u \n", x,boundary.max.X);
        //qsort(cutList[idx].data(), cutList[idx].size(), sizeof(int64_t), compare_int64_t);
	//log("     QSorting boundary done \n");
	//log("     Index : %u out of %u , Cutlist size %u \n", idx, totalNb, cutList[idx].size());
	
        /*
        for(unsigned int i = 0; i + 1 < cutList[idx].size(); i+=2)
        {
            //if (cutList[idx][i+1] - cutList[idx][i] < extrusionWidth / 2) continue;
	    //log("     Generating result  %u out of %u \n", i+1,cutList[idx].size());
            ClipperLib::Polygon p;
            p.push_back(matrix.unapply(Point(x, cutList[idx][i])));
            p.push_back(matrix.unapply(Point(x, cutList[idx][i+1])));
            result.push_back(p);
        }*/
        idx += 1;
        //log("     index  %u done\n", idx);
    }
    log("    Done with infill\n");
}

#endif//INFILL_H
