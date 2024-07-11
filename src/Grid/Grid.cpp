#include "Grid.h"

namespace nf = nanoflann;

void Grid::Init(const std::vector<Point3>& inPoints, float csize, float tol, float radius) {
    Clear();
    cellSize = csize;

    minX = std::min_element(inPoints.begin(), inPoints.end(), [](Point3 const a, Point3 const b) { return a.x < b.x; })->x - tol;
    maxX = std::max_element(inPoints.begin(), inPoints.end(), [](Point3 const a, Point3 const b) { return a.x < b.x; })->x + tol;
    minY = std::min_element(inPoints.begin(), inPoints.end(), [](Point3 const a, Point3 const b) { return a.y < b.y; })->y - tol;
    maxY = std::max_element(inPoints.begin(), inPoints.end(), [](Point3 const a, Point3 const b) { return a.y < b.y; })->y + tol;

    int num_cells_x = std::ceil((maxX - minX) / cellSize);
    int num_cells_y = std::ceil((maxY - minY) / cellSize);

    points.reserve(num_cells_x * num_cells_y);
    heightMat.resize(num_cells_x, std::vector<float>(num_cells_y, 0.0f));

    PointCloud cloud;
    for (const auto& point : inPoints) {
        cloud.pts.push_back({ point.x, point.y, point.z });
    }

    nf::KDTreeSingleIndexAdaptor<nf::L2_Simple_Adaptor<float, PointCloud>, PointCloud, 2> kdtree
    (2, cloud, nf::KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    kdtree.buildIndex();

    #pragma omp parallel for 
    for (int i = 0; i < num_cells_x; ++i) {
        for (int j = 0; j < num_cells_y; ++j) {
            float x = minX + i * cellSize;
            float y = minY + j * cellSize;
            float z = 0.0f;

            const size_t num_results = 1;
            size_t ret_index;
            float out_dist_sqr;
            nf::KNNResultSet<float> resultSet(num_results);
            resultSet.init(&ret_index, &out_dist_sqr);
            float query_pt[2] = { x, y };
            kdtree.findNeighbors(resultSet, &query_pt[0], nf::SearchParams(10));

            if (sqrt(out_dist_sqr) > radius) {
                z = 0.0f;
            }
            else {
                z = cloud.pts[ret_index].z;
            }

            heightMat[i][j] = z;
            if (z > 0) {
                #pragma omp critical
                {
                    points.emplace_back(x, y, z);
                }
            }
        }
    }

}

void Grid::Clear() {
    minX, maxX, minY, maxY, cellSize = 0;
    points.clear();
    heightMat.clear();
}

void Grid::FillHoles(uint16_t tol, int numIterations) {
    std::vector<std::pair<int, int>> kernel;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx != 0 || dy != 0) {
                kernel.push_back(std::make_pair(dx, dy));
            }
        }
    }

    int rows = heightMat.size();
    int cols = heightMat[0].size();

    std::vector<std::vector<float>> res = heightMat;

    for (int iteration = 0; iteration < numIterations; ++iteration) {
        #pragma omp parallel for
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (heightMat[i][j] == 0.0f) {
                    int count = 0;
                    float sum = 0.0;

                    for (auto& [dx, dy] : kernel) {
                        int ni = i + dx;
                        int nj = j + dy;

                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                            float neighbour = heightMat[ni][nj];
                            if (neighbour > 0.0f) {
                                ++count;
                                sum += neighbour;
                            }
                        }
                    }
                    if (count > tol) {
                        res[i][j] = sum / static_cast<float>(count);
                    }
                }
            }
        }
        heightMat = res;
    }
}

Point3 Grid::GetGridPointCoord(int gridX, int gridY) {
    // 0 - 0 in the opencv image is on top left not on bottom left (as in our matrix)
    float x = minX + cellSize * gridX;
    float y = maxY - cellSize * gridY;

    PointCloud cloud;
    for (const auto& point : points) {
        cloud.pts.push_back({ point.x, point.y, point.z });
    }

    nf::KDTreeSingleIndexAdaptor<nf::L2_Simple_Adaptor<float, PointCloud>, PointCloud, 2> kdtree
    (2, cloud, nf::KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    kdtree.buildIndex();

    const size_t num_results = 1;
    size_t ret_index;
    float out_dist_sqr;
    nf::KNNResultSet<float> resultSet(num_results);
    resultSet.init(&ret_index, &out_dist_sqr);
    float query_pt[2] = { x, y };
    kdtree.findNeighbors(resultSet, &query_pt[0], nf::SearchParams(10));

    return cloud.pts[ret_index];
}

std::vector<std::vector<float>> Grid::GetBooleanRoof() {
    std::vector<std::vector<float>> res = heightMat;
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();

    for (const auto& row : heightMat) {
        for (const auto& z : row) {
            if (z > 0.0f) {
                minZ = std::min(minZ, z);
            }
            maxZ = std::max(maxZ, z);
        }
    }

    for (int i = 0; i < res.size(); i++) {
        for (int j = 0; j < res[0].size(); j++) {
            if (res[i][j] > minZ) {
                res[i][j] = minZ;
            }
        }
    }

    return res;
}

std::vector<Point3> Grid::GetGlobalPoints(std::vector<Point2>& points) {
    int num_threads = omp_get_max_threads();
    const int size = points.size();

    if (size < num_threads)
        num_threads = size;

    int chunk_size = size / num_threads;

    std::vector<Point3> buffer = std::vector<Point3>(size);

    #pragma omp parallel num_threads(num_threads)
    {
        const int thread_id = omp_get_thread_num();
        const int start = thread_id * chunk_size;
        const int end = (thread_id == num_threads - 1) ? size : start + chunk_size;

        for (int i = start; i < end; i++) {
            auto point = this->GetGridPointCoord(points.at(i).x, points.at(i).y);
            buffer.at(i) = point;
        }
    }

    return buffer;
}