#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "NewGalaxy.hpp"
using namespace std;

class TestsFactory
{
private:
	int		_srand;
public:
	TestsFactory(/* args */);
	~TestsFactory();

	void createTest(string name, int points_count, int box_count, int points_max_range = 200);
};
