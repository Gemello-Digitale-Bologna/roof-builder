#include "NpyReader.h"

NpyReader::NpyReader(const std::string& filename) {
    data = npy::read_npy<double>(filename);
}

NpyReader::~NpyReader() {
    Flush();
}

std::vector<Point3>* NpyReader::Read3D() {
    std::vector<double> points = data.data;
    
    const int num_threads = omp_get_max_threads();
    const int size = points.size() / 3;
    const int chunk_size = size / num_threads;

    std::vector<Point3> buffer = std::vector<Point3>(size);

    #pragma omp parallel
    {
        const int thread_id = omp_get_thread_num();
        const int start = thread_id * chunk_size;
        const int end = (thread_id == num_threads - 1) ? size : start + chunk_size;

        for (int i = start; i < end; i++) {
            Point3 point(static_cast<float>(points[i]), static_cast<float>(points[size + i]), static_cast<float>(points[2 * size + i]));
            buffer.at(i) = point;
        }
    }

    points3d = buffer;
    buffer.clear();

    return &points3d;
}

std::vector<Point2>* NpyReader::Read2D() {
    std::vector<double> points = data.data;

    if (points.size() == 1 && !(points.at(0) == points.at(0))) {
        return nullptr;
    }
    
    int num_threads = omp_get_max_threads();
    const int size = points.size() / 2;

    if (size < num_threads)
        num_threads = size;

    int chunk_size = size / num_threads;

    std::vector<Point2> buffer = std::vector<Point2>(size);
    
    #pragma omp parallel num_threads(num_threads)
    {
        const int thread_id = omp_get_thread_num();
        const int start = thread_id * chunk_size;
        const int end = (thread_id == num_threads - 1) ? size : start + chunk_size;

        for (int i = start; i < end; i++) {
            auto point = Point2(static_cast<float>(points[i*2]), static_cast<float>(points[i*2 + 1]));
            buffer.at(i) = point;
        }
    }

    points2d = buffer;
    buffer.clear();

    return &points2d;
}

std::vector<std::vector<float>>* NpyReader::ReadMatInt() {
    std::vector<double> points = data.data;

    int rows = data.shape[0];
    int cols = data.shape[1];

    mat = std::vector<std::vector<float>>(rows, std::vector<float>(cols));

    int num_threads = omp_get_max_threads();
    const int size = rows * cols;

    if (size < num_threads)
        num_threads = size;

    int chunk_size = size / num_threads;

    #pragma omp parallel num_threads(num_threads)
    {
        const int thread_id = omp_get_thread_num();
        const int start = thread_id * chunk_size;
        const int end = (thread_id == num_threads - 1) ? size : start + chunk_size;

        for (int i = start; i < end; i++) {
            int row = i/cols;
            int col = i%cols;

            mat[row][col] = static_cast<float>(points[i]);
        }
    }

    return &mat;
}

std::vector<Point3>* NpyReader::Get3D() {
    return &points3d;
}

std::vector<Point2>* NpyReader::Get2D() {
    return &points2d;
}

std::vector<std::vector<float>>* NpyReader::GetMat() {
    return &mat;
}


void NpyReader::Flush() {
    points2d.clear();
    points3d.clear();
    mat.clear();
    data.data.clear();
}
