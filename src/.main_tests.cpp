#include "TestsFactory.hpp"
#include "NewGalaxy.hpp"
#include "GalaxyPathFinder.hpp"
#include "Container.hpp"
#include <iostream>
#include <string>
#include <fstream>

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define N "\033[0m"

#include <unistd.h>

using namespace std;

class float3
{
	public:
	float	x, y, z;
	float	distanse(float3 b) const { 
		return (sqrtf(powf((b.x - x), 2) + powf((b.y - y), 2) + powf((b.z - z), 2)));
	}
};

class int3
{
	public:
	int		x, y, z;
};

struct targetPoint
{
	int		id;
	int		order_i;
	float3	pos;
};

struct box
{
	int		id;
	int		targetId;
	float	weight;
	int3	pos;
	int3	half_size;
	int		half_cap;
};

static int err_num;

void	fail(string err) {
	cerr << err << endl;
	err_num++;
	// exit(EXIT_SUCCESS);
}

float3			parseFloat3(Json::Value val) {
	float3 res;

	if (!val.isMember("x") || !val.isMember("y") || !val.isMember("z"))
		throw std::invalid_argument("Not enought fields ");
	res.x = val["x"].asFloat();
	res.y = val["y"].asFloat();
	res.z = val["z"].asFloat();
	return (res);
}

std::map<int, targetPoint>	parsePoints(Json::Value points) {
	std::map<int, targetPoint> tp;
	targetPoint p;
	if (points.empty())
		throw std::logic_error("Empty points array");
	for (int i = 0; i < points.size(); i++) {
		if (!points[i].isMember("pointId"))
			throw std::logic_error("Point: Not enought fields ");
		p.pos = parseFloat3(points[i]);
		p.id = points[i]["pointId"].asInt();
		p.order_i = -1;
		tp.insert(std::pair<int, targetPoint>(p.id, p)); 
	}
	return (tp);
}

bool	box_intersect(box b, int pos_x, int pos_y, int pos_z) {
	if (
			(
				(pos_x < b.pos.x + b.half_size.x) 
			&&
				(pos_x > b.pos.x - b.half_size.x) 
			)
		&&
			(
				(pos_y < b.pos.y + b.half_size.y) 
			&&
				(pos_y > b.pos.y - b.half_size.y) 
			)
		&&
			(
				(pos_z < b.pos.z + b.half_size.z) 
			&&
				(pos_z > b.pos.z - b.half_size.z) 
			)
		)
		// fail("Box invalid positon");
		return true;
	return  false;
}

bool	box_wall_intersect(box b, int3 maxCarryingCapacity) {
	if (
		(maxCarryingCapacity.x < b.pos.x + b.half_size.x) 
		||
		(-maxCarryingCapacity.x > b.pos.x - b.half_size.x) 
		||
		(maxCarryingCapacity.y < b.pos.y + b.half_size.y) 
		||
		(-maxCarryingCapacity.y > b.pos.y - b.half_size.y) 
		||		
		(maxCarryingCapacity.z < b.pos.z + b.half_size.z) 
		||
		(-maxCarryingCapacity.z > b.pos.z - b.half_size.z) 
		)
		// fail("Box invalid positon");
		return true;
	return  false;
}

void	out_test(string out, float maxCarryingWeight, float resourcesConsumption,
							float maxResourcesWeight, int3 maxCarryingCapacity,
							std::map<int, targetPoint> points) {
	std::ifstream	ifs(out, std::ifstream::binary);
	Json::Value		root;
	
	try {
		if (!ifs.is_open())
			throw std::invalid_argument("Invalid input file");
		ifs >> root;
		ifs.close();
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < root["steps"].size(); i++) {
		float w = 0;
		for (int b = 0; b < root["steps"][i]["shippedBoxes"].size(); b++) {
			w += root["steps"][i]["shippedBoxes"][b]["w"].asFloat();

			box _b;
			_b.pos.x = root["steps"][i]["shippedBoxes"][b]["x"].asInt();
			_b.pos.y = root["steps"][i]["shippedBoxes"][b]["y"].asInt();
			_b.pos.z = root["steps"][i]["shippedBoxes"][b]["z"].asInt();

			_b.half_size.x = root["steps"][i]["shippedBoxes"][b]["half_size_x"].asInt();
			_b.half_size.y = root["steps"][i]["shippedBoxes"][b]["half_size_y"].asInt();
			_b.half_size.z = root["steps"][i]["shippedBoxes"][b]["half_size_z"].asInt();

			if (box_wall_intersect(_b, maxCarryingCapacity))
				fail("File: " + out + "\tStep: " + to_string(i)
					+ " \tb: " + to_string(b) + ";\tBox-Wall intersect\n" +
					"b:\tpos: x: " + to_string(_b.pos.x) + ", y: "+ to_string(_b.pos.y) + ", z: "+ to_string(_b.pos.z) + 
						"; size: x: " + to_string(_b.half_size.x) + ", y: "+ to_string(_b.half_size.y) + ", z: "+ to_string(_b.half_size.z) +"\n");
	
			for(int b_i = 0; b_i < root["steps"][i]["shippedBoxes"].size() ; b_i++)
			{
				if (b_i == b)
					continue ;
				// cout << "b: " << b << "; b_i: " << b_i << endl;
				box _b_tmp;
				_b_tmp.pos.x = root["steps"][i]["shippedBoxes"][b_i]["x"].asInt();
				_b_tmp.pos.y = root["steps"][i]["shippedBoxes"][b_i]["y"].asInt();
				_b_tmp.pos.z = root["steps"][i]["shippedBoxes"][b_i]["z"].asInt();

				_b_tmp.half_size.x = root["steps"][i]["shippedBoxes"][b_i]["half_size_x"].asInt();
				_b_tmp.half_size.y = root["steps"][i]["shippedBoxes"][b_i]["half_size_y"].asInt();
				_b_tmp.half_size.z = root["steps"][i]["shippedBoxes"][b_i]["half_size_z"].asInt();
				
				if (
					box_intersect(_b,	_b_tmp.pos.x + _b_tmp.half_size.x,
										_b_tmp.pos.y + _b_tmp.half_size.y,
										_b_tmp.pos.z + _b_tmp.half_size.z)
					||
					box_intersect(_b,	_b_tmp.pos.x - _b_tmp.half_size.x,
										_b_tmp.pos.y + _b_tmp.half_size.y,
										_b_tmp.pos.z + _b_tmp.half_size.z)
					||
					box_intersect(_b,	_b_tmp.pos.x + _b_tmp.half_size.x,
										_b_tmp.pos.y - _b_tmp.half_size.y,
										_b_tmp.pos.z + _b_tmp.half_size.z)
					||
					box_intersect(_b,	_b_tmp.pos.x + _b_tmp.half_size.x,
										_b_tmp.pos.y + _b_tmp.half_size.y,
										_b_tmp.pos.z - _b_tmp.half_size.z)
					||
					box_intersect(_b,	_b_tmp.pos.x - _b_tmp.half_size.x,
										_b_tmp.pos.y - _b_tmp.half_size.y,
										_b_tmp.pos.z + _b_tmp.half_size.z)
					||
					box_intersect(_b,	_b_tmp.pos.x - _b_tmp.half_size.x,
										_b_tmp.pos.y + _b_tmp.half_size.y,
										_b_tmp.pos.z - _b_tmp.half_size.z)
					||
					box_intersect(_b,	_b_tmp.pos.x + _b_tmp.half_size.x,
										_b_tmp.pos.y - _b_tmp.half_size.y,
										_b_tmp.pos.z - _b_tmp.half_size.z)
					||
					box_intersect(_b,	_b_tmp.pos.x - _b_tmp.half_size.x,
										_b_tmp.pos.y - _b_tmp.half_size.y,
										_b_tmp.pos.z - _b_tmp.half_size.z)
					)
					fail("File: " + out + "\tStep: " + to_string(i)
					+ " \tb: " + to_string(b) + "; b_i: " + to_string(b_i)
					+ "\tBoxes intersects\n" +
					"b1:\tpos: x: " + to_string(_b.pos.x) + ", y: "+ to_string(_b.pos.y) + ", z: "+ to_string(_b.pos.z) + 
						"; size: x: " + to_string(_b.half_size.x) + ", y: "+ to_string(_b.half_size.y) + ", z: "+ to_string(_b.half_size.z) +"\n" +
					"b2:\tpos: x: " + to_string(_b_tmp.pos.x) + ", y: "+ to_string(_b_tmp.pos.y) + ", z: "+ to_string(_b_tmp.pos.z) + 
						"; size: x: " + to_string(_b_tmp.half_size.x) + ", y: "+ to_string(_b_tmp.half_size.y) + ", z: "+ to_string(_b_tmp.half_size.z) + "\n");
			}	
		}
		float	res_w = root["steps"][i]["shippedResources"].asFloat();
		if (w + res_w > maxCarryingWeight)
			fail("File: " + out + "\tStep: " + to_string(i) + "\tMax weight exceeded");
		if (res_w > maxResourcesWeight)
			fail("File: " + out + "\tStep: " + to_string(i) + "\tMax res weight exceeded");

		float expected_res_w = 0;
		int next = root["steps"][i]["destinationPointId"].asInt();
		for (int cur = 0; next != 0; i++) {
			
			expected_res_w += resourcesConsumption * points[cur].pos.distanse(points[next].pos);
			cur = next;
			next = root["steps"][i + 1]["destinationPointId"].asInt();
		}
		expected_res_w += resourcesConsumption * points[root["steps"][i]["destinationPointId"].asInt()].pos.distanse(points[0].pos);

		if (expected_res_w > res_w)
			fail("File: " + out + " \tStep: " + to_string(i) + "\tInvalid res weight\n" +
				"res weight: " + to_string(res_w) + ";\twhen expected: " + to_string(expected_res_w));
		if (root["steps"][i]["destinationPointId"].asInt() == 0) {
			// cout << out << ": w\t" << w << endl;
			w = 0;
		}
	}
}

void	test(string in, string out) {
	GalaxyPathFinder	pf;
	clock_t				start;
	clock_t				end;


	start = clock();
	pf.FindSolution(in.c_str(), out.c_str());
	end = clock();
	std::cout << in << ", time: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;
	
	
	std::ifstream	ifs(in, std::ifstream::binary);
	Json::Value		root;
	
	try {
		if (!ifs.is_open())
			throw std::invalid_argument("Invalid input file");
		ifs >> root;
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}
	
	err_num = 0;
	out_test(out, root["ship"]["maxCarryingWeight"].asFloat(),
				root["ship"]["resourcesConsumption"].asFloat(),
				root["ship"]["maxResourcesWeight"].asFloat(),
				{	root["ship"]["maxCarryingCapacity"]["half_x"].asInt(), 
					root["ship"]["maxCarryingCapacity"]["half_y"].asInt(),
					root["ship"]["maxCarryingCapacity"]["half_z"].asInt(),},
					parsePoints(root["targetPoints"]));
	ifs.close();
	cout << (err_num == 0 ? GREEN : RED) << 
		"\tFor: " << out << "; Errors: " << err_num <<
		N << endl << endl << endl;
}

int main() {
	TestsFactory tf;

	try {
		for (size_t i = 0; i < 20; i++) {
			string in_name = "./tests/in/customInput" + to_string(i) + ".json";
			string out_name = "./tests/out/customOutput" + to_string(i) + ".json";
			tf.createTest(in_name, 20, 200);
			test(in_name, out_name);
		}

		tf.createTest("20kboxesInput.json", 40, 20000, 100);
		test("20kboxesInput.json", "20kboxesOutput.json");

		test("inputData1.json", "f1.json");
		test("inputData2.json", "f2.json");
		test("inputData3.json", "f3.json");
		test("inputData4.json", "f4.json");
		test("inputData5.json", "f5.json");
		test("inputData6.json", "f6.json");
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}