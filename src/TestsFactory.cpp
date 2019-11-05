#include "TestsFactory.hpp"

void	TestsFactory::createTest(string name, int points_count, int box_count, int points_max_range) {
		Json::Value root;
		int box_size_range = 150;
		int box_max_w = 80;
		
		srand(_srand);
		_srand += time(NULL);
		root["ship"]["maxCarryingCapacity"]["half_x"] = rand() % 1000; 
		root["ship"]["maxCarryingCapacity"]["half_y"] = rand() % 1000; 
		root["ship"]["maxCarryingCapacity"]["half_z"] = rand() % 1000; 

		// root["ship"]["maxCarryingWeight"] = rand() % 1000;
		// root["ship"]["maxResourcesWeight"] = rand() % root["ship"]["maxCarryingWeight"].asInt();
		// root["ship"]["resourcesConsumption"] = static_cast<float>(rand() % 1000) / 1000.f;
		root["ship"]["resourcesConsumption"] = 0.1;
		root["ship"]["maxCarryingWeight"] = 600;
		root["ship"]["maxResourcesWeight"] = 350;
	
		for (int i = 0; i < points_count; i++)
		{
			root["targetPoints"][i]["x"] = (rand() % points_max_range) - (points_max_range/2);
			root["targetPoints"][i]["y"] = (rand() % points_max_range) - (points_max_range/2);
			root["targetPoints"][i]["z"] = (rand() % points_max_range) - (points_max_range/2);
			root["targetPoints"][i]["pointId"] = i;
		}

		for (int i = 0; i < box_count; i++)
		{
			root["boxes"][i]["half_x"] = rand() % box_size_range;
			root["boxes"][i]["half_y"] = rand() % box_size_range;
			root["boxes"][i]["half_z"] = rand() % box_size_range;
			root["boxes"][i]["weight"] = rand() % box_max_w;
			root["boxes"][i]["targetPointId"] = rand() % points_count;
			root["boxes"][i]["boxId"] = i;
		}

		ofstream ofs(name);
		if (!ofs.is_open())
			throw std::invalid_argument("Something wrong with output file");
		ofs << root;
		ofs.close();
}

TestsFactory::TestsFactory(/* args */)
{
	_srand = time(NULL);
}

TestsFactory::~TestsFactory()
{
}
