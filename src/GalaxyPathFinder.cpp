#include "GalaxyPathFinder.hpp"
#include <fstream>
#include <iostream>
// #include <algorithm>
#include <exception>

GalaxyPathFinder::GalaxyPathFinder() : _captain_name("aillia") {}

GalaxyPathFinder::GalaxyPathFinder(char *name) : _captain_name((name)) {}

const char*		GalaxyPathFinder::ShowCaptainName() { return (_captain_name.c_str()); }

int3			GalaxyPathFinder::parseInt3(Json::Value val) {
	int3 res;

	if (!val.isMember("half_x") || !val.isMember("half_y") || !val.isMember("half_z"))
		throw std::invalid_argument("Not enought fields ");
	res.x = val["half_x"].asInt();
	res.y = val["half_y"].asInt();
	res.z = val["half_z"].asInt();
	if (res.x < 0 || res.y < 0 || res.z < 0 )
		throw std::invalid_argument("Invalid size value");
	return (res);
}

float3			GalaxyPathFinder::parseFloat3(Json::Value val) {
	float3 res;

	if (!val.isMember("x") || !val.isMember("y") || !val.isMember("z"))
		throw std::invalid_argument("Not enought fields ");
	res.x = val["x"].asFloat();
	res.y = val["y"].asFloat();
	res.z = val["z"].asFloat();
	return (res);
}

void			GalaxyPathFinder::parseShip(Json::Value ship) {
	if (!ship.isMember("maxCarryingWeight") || !ship.isMember("maxResourcesWeight")
	|| !ship.isMember("resourcesConsumption") || !ship.isMember("maxCarryingCapacity"))
		throw std::invalid_argument("Ship: Not enought fields ");
	_ship.max_carrying_w = ship["maxCarryingWeight"].asFloat();
	_ship.max_res_w = ship["maxResourcesWeight"].asFloat();
	_ship.res_consumption = ship["resourcesConsumption"].asFloat();
	_ship.half_size = parseInt3(ship["maxCarryingCapacity"]);
	_ship.half_cap = _ship.half_size.x * _ship.half_size.y * _ship.half_size.z;
	if (_ship.max_carrying_w < 0 || _ship.max_res_w < 0 || _ship.res_consumption < 0 )
		throw std::invalid_argument("Invalid ship weight or res consumption value");
}

void			GalaxyPathFinder::parsePoints(Json::Value points) {
	targetPoint p;
	if (points.empty())
		throw std::logic_error("Empty points array");
	for (int i = 0; i < points.size(); i++) {
		if (!points[i].isMember("pointId"))
			throw std::logic_error("Point: Not enought fields ");
		p.pos = parseFloat3(points[i]);
		p.id = points[i]["pointId"].asInt();
		p.order_i = -1;
		_tp.insert(std::pair<int, targetPoint>(p.id, p)); 
	}
	if (_tp.find(0) == _tp.end())
		throw std::logic_error("Point with id 0 must present");
}

void			GalaxyPathFinder::parseBoxes(Json::Value boxes) {
	box b;
	if (boxes.empty())
		throw std::invalid_argument("Empty boxes array");
	for (int i = 0; i < boxes.size(); i++) {
		if (!boxes[i].isMember("weight") || !boxes[i].isMember("targetPointId")
									|| !boxes[i].isMember("boxId"))
			throw std::invalid_argument("Box: Not enought fields");
		b.half_size = parseInt3(boxes[i]);
		b.weight = boxes[i]["weight"].asFloat();
		b.targetId = boxes[i]["targetPointId"].asInt();
		b.id = boxes[i]["boxId"].asInt();
		b.half_cap = (b.half_size.x * b.half_size.y * b.half_size.z);
		if (b.weight < 0)
			throw std::invalid_argument("Box: Invalid weight value");
		if (_tp.find(b.targetId) == _tp.end())
			// throw std::logic_error("Box: Target point id not found");
			_bad_boxes_id.push_back(b.id);
		else
			_boxes.push_back(b);
	}
}

void			GalaxyPathFinder::parse(const char* inputJasonFile) {
	std::ifstream	ifs(inputJasonFile, std::ifstream::binary);
	Json::Value		root;
	
	try {
		if (!ifs.is_open())
			throw std::invalid_argument("Invalid input file");
		ifs >> root;
		ifs.close();
		parseShip(root["ship"]);
		_containers.push_back(Container(_ship));
		// _containers.back().setShip(_ship);
		parsePoints(root["targetPoints"]);
		parseBoxes(root["boxes"]);
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}

}

// O(n^n)
void			GalaxyPathFinder::calculate_distances() {

	int order_i = 0;
	for (std::map<int, targetPoint>::iterator it = _tp.begin(); !_tp.empty();) {
		float	tmp;
		float	dist = __FLT_MAX__;
		int		id;
		for (auto j : _tp) {
			if (j.second.id == it->second.id || j.second.id == 0)
				continue ;
			if ((tmp = it->second.pos.distanse(j.second.pos)) < dist) {
				dist = tmp;
				id = j.second.id;
			}
		}
		it->second.order_i = order_i++;
		_tp_ordered.insert(*it);
		_tp.erase(it);
		it = _tp.find(id);
	}
}

void			GalaxyPathFinder::fill_bf() {
	_boxes.sort(
		[this](const box& a, const box& b) {
			return (a.targetId == b.targetId ?
						a.half_cap > b.half_cap :
						_tp_ordered.find(a.targetId)->second.order_i < _tp_ordered.find(b.targetId)->second.order_i);
		}
	);

	while (_boxes.begin() != _boxes.end())
	{
		bool isset = false;
		for(auto j = _containers.begin(); j != _containers.end(); j++)
		{
			if (j->setBox(*_boxes.begin(), _tp_ordered)) {
				_boxes.erase(_boxes.begin());
				isset = true;
				break ;
			}
		}
		if (!isset) {
			if (!_containers.back().isEmpty())
				_containers.push_back(Container());
			if (_containers.back().setBox(*_boxes.begin(), _tp_ordered))
				_boxes.erase(_boxes.begin());
			else {
				_bad_boxes_id.push_back(_boxes.begin()->id);
				_boxes.erase(_boxes.begin());
			}
		}
	}
}

void			GalaxyPathFinder::FindSolution(const char* inputJasonFile, const char* outputFileName) {
	parse(inputJasonFile);
	calculate_distances();
	fill_bf();

	for(std::list<Container>::iterator i = _containers.begin(); i != _containers.end(); i++) {
		i->printSteps(_tp_ordered);
	}
	Json::Value		j_out = _containers.front().getSteps();
	if (!_bad_boxes_id.empty()) {
		int step = _containers.front().getStepN();
		j_out["steps"][step]["destinationPointId"] = 0;
		j_out["steps"][step]["shippedResources"] = 0;
		for (int i = 0; i < _bad_boxes_id.size(); i++) {
				j_out["steps"][step]["shippedBoxes"][i]["boxId"] = _bad_boxes_id[i];
				j_out["steps"][step]["shippedBoxes"][i]["x"] = 0;
				j_out["steps"][step]["shippedBoxes"][i]["y"] = 0;
				j_out["steps"][step]["shippedBoxes"][i]["z"] = 0;
		}
	}

	/// tests
	float sum_res = 0;
	for (auto i : _containers) {
		sum_res += i.getResW();
		// std::cout << i.getResW() << "\t";
	}
	std::cout << std::endl <<  "Steps " << _containers.front().getStepN() << " Sum res: " << sum_res << std::endl;
	///

	std::ofstream	ofs(outputFileName, std::ifstream::binary);
	try {
		ofs << j_out;
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}
