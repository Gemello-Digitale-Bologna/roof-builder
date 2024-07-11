#include "Building.h"

std::vector<std::shared_ptr<Building>> BuildingFactory::CreateBuildings(const std::vector<std::string>& entries) {
    std::vector<std::shared_ptr<Building>> m_Buildings;

    for (const auto& line : entries) {
        auto building = BuildingFactory::CreateBuilding(line);
        m_Buildings.push_back(building);
    }

    return m_Buildings;
}

std::vector<std::shared_ptr<Building>> BuildingFactory::CreateBuildingsParallel(const std::vector<std::string>& entries) {
    const int num_threads = omp_get_max_threads();
    const int size = entries.size();
    const int chunk_size = size / num_threads;

    std::vector<std::shared_ptr<Building>> m_Buildings = std::vector<std::shared_ptr<Building>>(size);

    #pragma omp parallel
    {
        const int thread_id = omp_get_thread_num();
        const int start = thread_id * chunk_size;
        const int end = (thread_id == num_threads - 1) ? size : start + chunk_size;

        for (int i = start; i < end; i++) {
            auto building = BuildingFactory::CreateBuilding(entries.at(i));
            m_Buildings.at(i) = building;
        }
    }

    return m_Buildings;
}

std::shared_ptr<Building> BuildingFactory::CreateBuilding(const std::string& entry) {
	std::locale::global(std::locale("en_US.utf8"));

	std::string token;
	std::istringstream tokenStream(entry);

	uint16_t codiceOggetto;
	float quotaGronda;
	float tolleranza;
	std::vector<geos::geom::Coordinate> points;
	std::unique_ptr<geos::geom::Polygon> polygon;
	std::vector<std::string> tiles;
	int i = 0;

	while (std::getline(tokenStream, token, ';')) {
		if (i == 0) {
			codiceOggetto = std::stoi(token);
		}
		else if (i == 1) {
			quotaGronda = std::stof(token);
		}
		else if (i == 2) {
			points = getPoints(token);
			polygon = getPolygon(points);
		}
		else if(i == 3) {
			tolleranza = std::stof(token);
		}
		else {
			tiles = getTiles(token);
		}

		i++;
	}
	return std::make_shared<Building>(codiceOggetto, quotaGronda, points, tolleranza, std::move(polygon), tiles);
}

std::vector<std::string> BuildingFactory::getTiles(const std::string& entry) {
	std::string str = entry;

	str.erase(std::remove(str.begin(), str.end(), '['), str.end());
	str.erase(std::remove(str.begin(), str.end(), ']'), str.end());

	std::stringstream ss(str);
	std::string token;

	std::vector<std::string> resultVector;

	if (str.find(',') != std::string::npos) {
		while (std::getline(ss, token, ',')) {
			token.erase(std::remove(token.begin(), token.end(), '\''), token.end());
			// Uncomment if the CSV was generated from Windows
			//token.erase(std::remove(token.begin(), token.end(), '\r'), token.end());
			resultVector.push_back(token);
		}
	}
	else {
		str.erase(std::remove(str.begin(), str.end(), '\''), str.end());
		// Uncomment if the CSV was generated from Windows
		//str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
		resultVector.push_back(str);
	}
	return resultVector;
}

std::vector<geos::geom::Coordinate> BuildingFactory::getPoints(const std::string& entry) {
	std::locale::global(std::locale("en_US.utf8"));
	std::string newToken;
	std::istringstream pointStream(entry);
	std::vector<geos::geom::Point> points;
	std::vector<geos::geom::Coordinate> coords;

	while (std::getline(pointStream, newToken, ']')) {

		size_t pos;
		while ((pos = newToken.find('[')) != std::string::npos) {
			newToken.erase(pos, 1);
		}

		if (newToken.substr(0, 2) == ", ") {
			newToken.erase(0, 2);
		}

		std::vector<double> res;

		if (newToken.find(',') != std::string::npos) {
			std::stringstream ss(newToken);

			while (ss.good()) {
				std::string substr;
				std::getline(ss, substr, ',');
				res.push_back(std::stod(substr));
			}
		}

		if (res.size() == 2) {
			geos::geom::Coordinate c(res.at(0), res.at(1));
			coords.push_back(c);
		}
	}

	coords.push_back(coords.at(0));

	return coords;
}

std::unique_ptr<geos::geom::Polygon> BuildingFactory::getPolygon(const std::vector<geos::geom::Coordinate>& coords) {
	auto factory = geos::geom::GeometryFactory::create();

	geos::geom::CoordinateSequence sequence(0, 2);

	for (auto c : coords) {
		sequence.add(c);
	}

	std::unique_ptr<geos::geom::LinearRing> shell = factory->createLinearRing(sequence);
	auto polygon = factory->createPolygon(std::move(shell));

	return polygon;
}

void Building::ToString() {
    std::cout << "Codice: " << m_CodiceOggetto << std::endl;
    std::cout << "Quota: " << m_QuotaGronda << std::endl;
    std::cout << "Tolleranza: " << m_Tolleranza << std::endl;
    std::cout << "Tiles: " << m_Tiles.size() << std::endl;
	for (const auto& val : m_Tiles) {
        std::cout << val << " - ";
    }
    std::cout << std::endl;
}