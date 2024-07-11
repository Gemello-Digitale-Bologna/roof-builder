#define ROUND(x) static_cast<int>(floor(x))

#define PY_LAS PYTHON_PATH "lasToNpy.py"
#define PY_EDG PYTHON_PATH "cv_pipeline_edge.py"
#define PY_RDG PYTHON_PATH "cv_pipeline_ridge.py"

// TESTING: temporaneo
#define TARGET_LAS "32_684000_4930000.las"

#include "Program.h"
#include "Timer.h"
#include "Building.h"

#include "Readers/NpyReader.h"
#include "Readers/CsvReader.h"

#include "Dbscan/Dbscan.h"
#include "Grid/Grid.h"
#include "Triangle/TriangleWrap.h"

#include "npy.hpp"
#include "py/PyCaller.h"
#include "Writers/NpyWriter.h"
#include "Writers/StlWriter.h"

static Program* s_Instance = nullptr;

Program::Program() {
    s_Instance = this;
    env.load_dotenv(CONFIG_PATH "config.env");

    BUILDINGS_CSV = env["IN_PATH"] + env["CSV_PATH"];
    PATH_LAS = env["IN_PATH"] + env["LIDAR_PATH"] + "32_684000_4930000.las"; // TESTING: temporaneo
    PATH_LAS_NP = env["IN_PATH"] + env["TMP_PATH"] + "lidar_temp.npy";

    PATH_EDG_IO = env["IN_PATH"] + env["TMP_PATH"] + "edge_IO.npy";
    PATH_EDG_MAT = env["IN_PATH"] + env["TMP_PATH"] + "edge_mat.npy";
    PATH_RDG_IO = env["IN_PATH"] + env["TMP_PATH"] + "ridge_IO.npy";

    PATH_OUT = env["OUT_PATH"] + "output.stl"; // TESTING: temporaneo
}

Program::~Program() {
    s_Instance = nullptr;
}

Program& Program::Get() {
    return *s_Instance;
}

void Program::Execute() {
    //############## DATA LOAD ##############

    std::cout << "Reading LAS file..." << std::endl;
    // TESTING: name hardcoded
    PyCaller pyCaller;

    std::string lasToPy = PY_LAS;
    pyCaller.Load(lasToPy);
    std::vector<std::string> lp_args = { PATH_LAS, PATH_LAS_NP };
    pyCaller.Execute(lp_args);

    NpyReader lasNpy(PATH_LAS_NP);
    auto points3d = lasNpy.Read3D();

    // if points is empty exit
    if (points3d->empty())
        return;

    CsvReader readerCsv;
    Timer timer;

    std::cout << "Reading CSV file: " << BUILDINGS_CSV << std::endl;

    timer.Start();
    // TESTING: name hardcoded
    readerCsv.Read(BUILDINGS_CSV);
    std::vector<std::string> lines = readerCsv.Get();

    std::vector<std::shared_ptr<Building>> Buildings = BuildingFactory::CreateBuildingsParallel(lines);
    timer.Stop("Data Load time: ");

    timer.Start();
    int b_tile = 0;
    int b_sp = 0;

    std::vector<Mesh> meshes;
    for (std::shared_ptr<Building> building : Buildings) {
        /*
        if (building->GetCodiceOggetto() != 24069)
            continue;
        //building->ToString();
        
        std::cout << "Building: " << building->GetCodiceOggetto() << std::endl;
        */
        
        //############## FILTER TARGET POINTS ##############

        std::vector<Point3> targetPoints;
        auto lTol = building->GetQuotaGronda();
        auto hTol = lTol + building->GetTolleranza();

        std::vector<std::string> tiles = building->GetTiles();
        int found = 0;
       
        for (const std::string& tile : tiles) {
            // TESTING: Target Las hardcoded
            if (tile.find(TARGET_LAS) != std::string::npos) {
                found++;
                b_tile++;
                
                for (Point3 p : *points3d) {
                    if (building->Contains(p)) {
                        targetPoints.push_back(p);
                    }
                }   
            }
        }
        if (found == 0)
            continue;
        if (targetPoints.size() <= 20)
            continue;
        b_sp++;

        //############## CLUSTERING (remove outliers) ##############

        std::vector<Point3> mainCluster = Dbscan::GetMainCluster(std::span(targetPoints), 0.8, 10);

        if (mainCluster.size() == 0)
			continue;

        //############## GRID CREATION (organized point-cloud) ##############

        Grid grid;
		grid.Init(mainCluster, 0.1, 2.0, 0.2);
		grid.FillHoles(3, 3);
		targetPoints.clear();

        //############## EDGE FEATURE EXTRACTION ##############

        std::vector<std::vector<float>> br = grid.GetBooleanRoof();

        NpyWriter edgeWriter(PATH_EDG_IO);
        edgeWriter.WriteMat(br);

        pyCaller.Load(PY_EDG);

        int buildingCornerNumb = building->GetPolygon()->getNumPoints() - 1;
        float safetyFactor = 3.0;
        int maxFeatures = buildingCornerNumb * safetyFactor;
        std::vector<std::string> edg_args = {PATH_EDG_IO, PATH_EDG_MAT, std::to_string(maxFeatures)};
        pyCaller.Execute(edg_args);

        NpyReader readerEdge(PATH_EDG_IO);
        auto edgePts = readerEdge.Read2D();

        if (edgePts == nullptr)
            continue;

        NpyReader readerMat(PATH_EDG_MAT);
        std::vector<std::vector<float>>* edgeFillMat = readerMat.ReadMatInt();

        TriangleWrapper triWrap;
        triWrap.Initialize();
        triWrap.UploadPoints(*edgePts);

        std::vector<Triangle> tris = triWrap.Triangulate();

        //############## OUTSIDE TRIANGLE REMOVAL ##############
        //Note: - with the check on the boolean roof (br[c_x][br[0].size()-c_y] != 0) in case the roof has holes we remove some inside triangles
        //      - with the check on the edge filled matrix (edgeFillMat->at(c_x)[br[0].size()-c_y] != 0) we have a problem with edges that don't fully close up
        //So why try to check both to make it more robust

        std::vector<Triangle> tempTris;
		for (int i = 0; i < tris.size(); /* no increment here */) {
			int c_x = ROUND((edgePts->at(tris[i].i1).x + edgePts->at(tris[i].i2).x + edgePts->at(tris[i].i3).x) / 3.0);
            int c_y = ROUND((edgePts->at(tris[i].i1).y + edgePts->at(tris[i].i2).y + edgePts->at(tris[i].i3).y) / 3.0);

            if (edgeFillMat->at(c_x)[br[0].size()-c_y] != 0 || br[c_x][br[0].size()-c_y] != 0) {
				tempTris.push_back(tris[i]);

				++i; // only increment if we didn't erase
			}
			else {
				// remove this entry from the array
				tris.erase(tris.begin() + i);
			}
		}

        std::map<std::pair<int, int>, int> edgeFrequency;

		for (const auto& tri : tris) {
            std::pair<int, int> edge1 = std::minmax(tri.i1, tri.i2);
            edgeFrequency[edge1]++;

            std::pair<int, int> edge2 = std::minmax(tri.i2, tri.i3);
            edgeFrequency[edge2]++;

            std::pair<int, int> edge3 = std::minmax(tri.i1, tri.i3);
            edgeFrequency[edge3]++;
		}

		std::list<std::pair<int, int>> externalEdges;

		for (const auto& item : edgeFrequency) {
			if (item.second == 1) {
				externalEdges.push_back(item.first);
			}
		}

        //############## RIDGE FEATURE EXTRACTION ##############

        NpyWriter ridgeWriter(PATH_RDG_IO);
        ridgeWriter.WriteMat(grid.GetHeightMat());

        pyCaller.Load(PY_RDG);

        std::vector<std::string> ridge_args = { PATH_RDG_IO };
        pyCaller.Execute(ridge_args);

        NpyReader readerRidge(PATH_RDG_IO);
        auto ridgePts = readerRidge.Read2D();

        if (ridgePts == nullptr)
            continue;
        
        //############## DUPLICATE POINTS PURGE ##############

        for (auto i = ridgePts->begin(); i != ridgePts->end(); i++) {
			for (auto j = edgePts->begin(); j != edgePts->end(); j++) {
				float dist = i->distance(*j);

				if (dist < 2.0) {
					i = ridgePts->erase(i);
					i--;
					break;
				}
			}
		}

        //############## MESH TRIANGULATION ##############

        std::vector<std::pair<int, int>> polyEdge = UtilsTriangle::Polygonize(externalEdges);

        triWrap.Initialize();
		triWrap.UploadPoints(*edgePts, *ridgePts, polyEdge);
		std::vector<Triangle> fullTriangles = triWrap.TriangulateConstrained();

        std::vector<Point2> vertices(edgePts->begin(), edgePts->end());
        vertices.insert(vertices.end(), ridgePts->begin(), ridgePts->end());

        std::vector<Point3> globVerts = grid.GetGlobalPoints(vertices);

        //############## LOCAL TO GLOBAL TRANSFORMATION ##############

        meshes.push_back(Mesh(vertices, globVerts, fullTriangles));
    }
    timer.Stop("Computation time: ");
    lasNpy.Flush();
    std::cout << "B_num: " << b_tile << std::endl;
    std::cout << "B_sp: " << b_sp << std::endl;
    std::cout << "Meshes num: " << meshes.size() << std::endl;

    //############## MESH SAVE ##############

    timer.Start();
    std::string stl_path = PATH_OUT;
    StlWriter::ExportStl(meshes, stl_path);
    timer.Stop("Writing time: ");
}