#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>

#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>

#include <omp.h>
#include "Geometry.h"


class Building {

public:
	Building(uint16_t codice, float quota, std::vector<geos::geom::Coordinate> shape, float toll, std::unique_ptr<geos::geom::Polygon> polygon, std::vector<std::string> tiles) : m_CodiceOggetto(codice), m_QuotaGronda(quota), m_GeoShapePoints(shape), m_Tolleranza(toll), m_Polygon(std::move(polygon)), m_Tiles(tiles){
		m_Factory = geos::geom::GeometryFactory::create();
	}
	~Building() {}

	uint16_t GetCodiceOggetto() const { return m_CodiceOggetto; }
	float GetQuotaGronda() const { return m_QuotaGronda; }
	float GetTolleranza() const { return m_Tolleranza; }
	std::vector<geos::geom::Coordinate> GetGeoShapePoints() const { return m_GeoShapePoints; }
	std::shared_ptr<geos::geom::Polygon> GetPolygon() { return m_Polygon; }
	std::vector<std::string> GetTiles() const { return m_Tiles; }
    void ToString();

	std::vector<std::pair<Point3, Point3>> GetEdges() const {
		std::vector<std::pair<Point3, Point3>> edges;
		if (m_GeoShapePoints.empty()) return edges;

		for (size_t i = 0; i < m_GeoShapePoints.size(); ++i) {
			const auto& p1 = m_GeoShapePoints[i];
			const auto& p2 = m_GeoShapePoints[(i + 1) % m_GeoShapePoints.size()];

			edges.emplace_back(Point3(p1.x, p1.y, 0), Point3(p2.x, p2.y, 0));
		}

		return edges;
	}

	bool Contains(Point3 p) {
		auto lTol = m_QuotaGronda;
        auto hTol = lTol + m_Tolleranza;

		if (p.z >= lTol && p.z <= hTol)
			return m_Polygon->contains(m_Factory->createPoint(geos::geom::Coordinate(p.x, p.y, 0)).get());
		return false;
	}

private:
	geos::geom::GeometryFactory::Ptr m_Factory;

	uint16_t m_CodiceOggetto;
	float m_QuotaGronda;
	std::vector<std::string> m_Tiles;
	std::vector<geos::geom::Coordinate> m_GeoShapePoints;
	std::shared_ptr<geos::geom::Polygon> m_Polygon;
	float m_Tolleranza;
};


class BuildingFactory {

public:
	static std::shared_ptr<Building> CreateBuilding(const std::string& entry);
    static std::vector<std::shared_ptr<Building>> CreateBuildings(const std::vector<std::string>& entries);
    static std::vector<std::shared_ptr<Building>> CreateBuildingsParallel(const std::vector<std::string>& entries);

private:
	static std::vector<geos::geom::Coordinate> getPoints(const std::string& entry);
	static std::unique_ptr<geos::geom::Polygon> getPolygon(const std::vector<geos::geom::Coordinate>& coords);
	static std::vector<std::string> getTiles(const std::string& entry);
};