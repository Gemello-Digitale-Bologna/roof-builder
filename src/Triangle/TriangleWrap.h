#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <list>
#include "../Geometry.h"
extern "C" {
	#include "triangle.h"
}

typedef triangulateio TriangleIO;

class TriangleWrapper {
public:
	TriangleWrapper();
	~TriangleWrapper();
	void Initialize();

	void UploadPoints(const std::vector<Point2>& constrainedPoints, const std::vector<Point2>& other = {}, const std::vector<std::pair<int, int>>& constrainedIndexes = {});
	std::vector<Triangle> Triangulate();
	std::vector<Triangle> TriangulateConstrained();

	std::vector<std::vector<int>> GetIndices() { return m_Indices; };
private:
	void deallocateStructs();

private:
	TriangleIO m_In;
	TriangleIO m_Out;
	std::vector<Triangle> m_Triangles;
	std::vector<std::vector<int>> m_Indices;
};


class UtilsTriangle {
private:
	static int FindNextVert(std::list<std::pair<int, int>>& v, int num) {
		for (auto it = v.begin(); it != v.end(); /* no increment here */) {
			if (it->first == num) {
				int temp = it->second;
				v.erase(it);
				return temp;
			}
			else if (it->second == num) {
				int temp = it->first;
				it = v.erase(it);
				return temp;
			}
			else {
				++it; // only increment if we didn't erase
			}
		}

		return -1;
	}

public:
	static std::vector<std::pair<int, int>> Polygonize(std::list<std::pair<int, int>>& list) {
		std::vector<std::pair<int, int>> res;
		int init = list.front().first;
		int temp = init;
		while (!list.empty()) { // no increment here
			int sec = FindNextVert(list, temp);
			res.push_back({ temp, sec });
			temp = sec;
			if (sec == init) { // there are some dirty edges
				return res;
			}
		}

		return res;
	}
};
