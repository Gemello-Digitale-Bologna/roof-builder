#pragma once
#include <vector>
#include "../Geometry.h"
#include "StreamWriter.h"

class StlWriter {
public:
    static int ExportStl(Mesh& mesh, std::string& fileName) {
        std::ostringstream os;
        os.imbue(std::locale::classic());
        os << std::fixed << std::setprecision(3);
        os << "solid\n";
        for (const auto& t : mesh.triangles) {
            os << "outer loop\n";
            os << "vertex " << mesh.globalVertices.at(t.i1) << "\n";
            os << "vertex " << mesh.globalVertices.at(t.i2) << "\n";
            os << "vertex " << mesh.globalVertices.at(t.i3) << "\n";
            os << "endloop\n";
        }
        os << "endsolid\n";

        return StreamWriter::Write(fileName, os.str());
    }

    static int ExportStl(std::vector<Mesh>& meshes, std::string& fileName) {
        std::ostringstream os;
        os.imbue(std::locale::classic());
        os << std::fixed << std::setprecision(3);
        for (Mesh m : meshes) {
            os << "solid\n";
            for (const auto& t : m.triangles) {
                os << "outer loop\n";
                os << "vertex " << m.globalVertices.at(t.i1) << "\n";
                os << "vertex " << m.globalVertices.at(t.i2) << "\n";
                os << "vertex " << m.globalVertices.at(t.i3) << "\n";
                os << "endloop\n";
            }
            os << "endsolid\n";
        }

        return StreamWriter::Write(fileName, os.str());
    }
};