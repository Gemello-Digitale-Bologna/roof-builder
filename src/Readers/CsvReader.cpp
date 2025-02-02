#include "CsvReader.h"

CsvReader::CsvReader() {}

CsvReader::~CsvReader() {
    Flush();
}

void CsvReader::Read() {}

void CsvReader::Read(const std::string& filePath) {
	std::ifstream file(filePath);
	std::string line;

	if (!file.is_open()) {
		std::cerr << "Errore nell'apertura del file: " << filePath << std::endl;
		return;
	}

	while (std::getline(file, line)) {
		if (line.find("CODICE_OGGETTO") != std::string::npos)
			continue;

		m_Lines.push_back(line);
	}
}

std::vector<std::string>* CsvReader::GetPtr() {
	return &m_Lines;
}

std::vector<std::string> CsvReader::Get() {
	return m_Lines;
}

void CsvReader::Flush() {
	if (m_Lines.size() > 0)
		m_Lines.clear();
}