#pragma once
#include <istream>
#include "common.hpp"
#include <string>
#include <vector>

// tiles that form the island
struct CSVReader
{
public:
	static std::vector<std::vector<std::string>> readCSV(std::istream & in);

private:
	static std::vector<std::string> readCSVRow(const std::string& row);

};
