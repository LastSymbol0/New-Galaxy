#include "Container.hpp"
#include <iostream>
#include <cassert>  
#define max 999

ship		Container::_ship;

Json::Value	Container::_steps;
int			Container::_step_n;

Container::Container(ship s) {
	setShip(s);
	_weight = 0;
	_res_weight = 0;
	_fill_area.push_back({0, 0, 0});
	fill_area_point start = {{0, 0, 0},
					{_ship.half_size.x, 0, 0},
					{0, _ship.half_size.y, 0},
					{0, 0, _ship.half_size.z}};
	_fill_area_n.push_back(start);
	_cap = 0;
	_travel_points.push_back(0);
}

Container::Container() {
	_weight = 0;
	_res_weight = 0;
	_fill_area.push_back({0, 0, 0});
	fill_area_point start = {{0, 0, 0},
					{_ship.half_size.x, 0, 0},
					{0, _ship.half_size.y, 0},
					{0, 0, _ship.half_size.z}};
	_fill_area_n.push_back(start);
	_cap = 0;
	_travel_points.push_back(0);
}

void	Container::clear_static() {
		_ship = {{0,0,0},0,0,0,0};
		_step_n = 0;
		_steps.clear();
		_boxes.clear();
		_travel_points.clear();
		_fill_area.clear();
		_fill_area_n.clear();
}

bool		Container::isEmpty() const { return (_boxes.empty()); }

void		Container::setShip(ship ship) { _ship = ship; }

Json::Value	Container::getSteps() const { return (_steps); }
int			Container::getStepN() const { return (_step_n); }
float		Container::getResW() const { return (_res_weight); }
float		Container::getW() const { return (_weight); }

fill_area_point	Container::set_point_limits(box b, fill_area_point old, fill_area_point p) {
	//set lim_x
	if (old.lim_x.y <= b.half_size.y && old.lim_x.z <= b.half_size.z)
		p.lim_x = {_ship.half_size.x - p.pos.x, 0, 0}; // max
	else
		p.lim_x = {old.lim_x.x,
					old.lim_x.y - b.half_size.y,
					old.lim_x.z - b.half_size.z};

	//set lim_y
	if (old.lim_y.x <= b.half_size.x && old.lim_y.z <= b.half_size.z)
		p.lim_y = {0, _ship.half_size.y - p.pos.y, 0}; // max
	else
		p.lim_y = {old.lim_y.x - b.half_size.x,
					old.lim_y.y,
					old.lim_y.z - b.half_size.z};

	//set lim_z
	if (old.lim_z.y <= b.half_size.y && old.lim_z.x <= b.half_size.x)
		p.lim_z = {0, 0, _ship.half_size.z - p.pos.z}; // max
	else
		p.lim_z = {old.lim_y.x - b.half_size.x,
					old.lim_z.y - b.half_size.y,
					old.lim_z.z};
	return (p);
}

void		Container::new_limits(int3 vertex1, int3 vertex2, std::list<fill_area_point>::iterator cur) {
	for (std::list<fill_area_point>::iterator j = _fill_area_n.begin(); j != _fill_area_n.end(); j++) {
		if (j == cur)
			continue ;
		if (j->pos.x <= vertex2.x) {
			if ((vertex2.y - j->pos.y) >= 0 && j->lim_y.y >= vertex1.y - j->pos.y) {
				j->lim_y.y = vertex1.y - j->pos.y;
				j->lim_y.x = vertex2.x - j->pos.x;
				// j->lim_y.z = vertex2.z - j->pos.z;
			}
			if ((vertex2.z - j->pos.z) >= 0 && j->lim_z.y >= vertex1.z - j->pos.z) {
				j->lim_z.z = vertex1.z - j->pos.z;
				// j->lim_z.y = vertex2.y - j->pos.y;
				j->lim_z.x = vertex2.x - j->pos.x;
			}
		}
		if (j->pos.y <= vertex2.y) {
			if ((vertex2.x - j->pos.x) >= 0 && j->lim_x.x >= vertex1.x - j->pos.x) {
				j->lim_x.x = vertex1.x - j->pos.x;
				j->lim_x.y = vertex2.y - j->pos.y;
				// j->lim_x.z = vertex2.z - j->pos.z;
			}
			if ((vertex2.z - j->pos.z) >= 0 && j->lim_z.y >= vertex1.z - j->pos.z) {
				j->lim_z.z = vertex1.z - j->pos.z;
				// assert(j->lim_z.z < 0);
				j->lim_z.y = vertex2.y - j->pos.y;
				// j->lim_z.x = vertex2.x - j->pos.x;
			}
		}
		if (j->pos.z <= vertex2.z) {
			if ((vertex2.x - j->pos.x) >= 0 && j->lim_x.x >= vertex1.x - j->pos.x) {
				j->lim_x.x = vertex1.x - j->pos.x;
				// j->lim_x.y = vertex2.y - j->pos.y;
				j->lim_x.z = vertex2.z - j->pos.z;
			}
			if ((vertex2.y - j->pos.y) >= 0 && j->lim_y.y >= vertex1.y - j->pos.y) {
				j->lim_y.y = vertex1.y - j->pos.y;
				// j->lim_y.x = vertex2.x - j->pos.x;
				j->lim_y.z = vertex2.z - j->pos.z;
			}
		}
	}
}

void		Container::push_new_points(box b, fill_area_point old, int3 vertex) {
	
	_fill_area_n.push_back(set_point_limits(b, old, {{vertex.x, old.pos.y, old.pos.z}, {0,0,0}, {0,0,0}, {0,0,0}}));
	_fill_area_n.push_back(set_point_limits(b, old, {{old.pos.x, vertex.y, old.pos.z}, {0,0,0}, {0,0,0}, {0,0,0}}));
	_fill_area_n.push_back(set_point_limits(b, old, {{old.pos.x, old.pos.y, vertex.z}, {0,0,0}, {0,0,0}, {0,0,0}}));
}

bool		Container::setBox(box b, std::map<int, targetPoint> tp) {
	// if container full
	if (_ship.half_cap < _cap + b.half_cap || _weight + b.weight > _ship.max_carrying_w)
		return false;
	float res_to_delive_and_back = 0;
	float minus_res_w = 0;
	// if box have new target id
	if (_travel_points.back() != b.targetId) {
		// TODO: if target point exist
		minus_res_w = tp[_travel_points.back()].pos.distanse(tp[0].pos) * _ship.res_consumption;
		res_to_delive_and_back = (tp[_travel_points.back()].pos.distanse(tp[b.targetId].pos)
								+ (tp[b.targetId].pos.distanse(tp[0].pos)))
									* _ship.res_consumption;
		// if box delive wants more resoourses than possible
		if (res_to_delive_and_back + _res_weight - minus_res_w > _ship.max_res_w
		|| res_to_delive_and_back + _weight + b.weight - minus_res_w> _ship.max_carrying_w)
			return false;
	}
	
	// try to set box in container 
	for (std::list<fill_area_point>::iterator j = _fill_area_n.begin(); j != _fill_area_n.end(); j++) {

		// if (b.half_size.x <= j->lim_x.x && b.half_size.y <= j->lim_y.y && b.half_size.z <= j->lim_z.z &&
			// j->pos.x + b.half_size.x <= _ship.half_size.x && j->pos.y + b.half_size.y <= _ship.half_size.y && j->pos.z + b.half_size.z <= _ship.half_size.z) {
		if (b.half_size.x <= j->lim_x.x && b.half_size.y <= j->lim_y.y && b.half_size.z <= j->lim_z.z) {

			// here we checks every existing point, and set them new limits if it necessary
			new_limits(j->pos, {j->pos.x + b.half_size.x,
								j->pos.y + b.half_size.y,
								j->pos.z + b.half_size.z}, j);

			// push new possible points
			push_new_points(b, *j, {j->pos.x + b.half_size.x,
									j->pos.y + b.half_size.y,
									j->pos.z + b.half_size.z});
			// _fill_area.push_back({x, j->y, j->z});
			// _fill_area.push_back({j->x, y, j->z});
			// _fill_area.push_back({j->x, j->y, z});

			// set real (not half) position
			b.pos = {	b.half_size.x + (j->pos.x * 2) - _ship.half_size.x,
						b.half_size.y + (j->pos.y * 2) - _ship.half_size.y,
						b.half_size.z + (j->pos.z * 2) - _ship.half_size.z};

			_weight -= minus_res_w;
			_res_weight -= minus_res_w;
			_res_weight += res_to_delive_and_back;
			_weight += b.weight + res_to_delive_and_back;
			if (_travel_points.back() != b.targetId)
				_travel_points.push_back(b.targetId);
			_cap += b.half_cap;
			_fill_area_n.erase(j);
			// _fill_area_n.erase(std::next(j).base());
			_boxes.push_back(b);
			return true;
		}
	}			
	return false;
}

void		Container::printSteps(std::map<int, targetPoint> tp) {
	if (_boxes.empty())
		return ;
	_steps["steps"][_step_n]["shippedResources"] = _res_weight;

	int len = _travel_points.size() - 1 + _step_n;
	// for every point (step)
	for (int i = 0; _step_n < len; _step_n++, i++) {
		int box_index = 0;
		for (std::list<box>::iterator j = _boxes.begin(); j != _boxes.end();) {
			if (j->targetId == _travel_points[i])
				j = _boxes.erase(j);
			else {
				_steps["steps"][_step_n]["shippedBoxes"][box_index]["boxId"] = j->id;
				_steps["steps"][_step_n]["shippedBoxes"][box_index]["w"] = j->weight;
				// _steps["steps"][_step_n]["shippedBoxes"][box_index]["targetId"] = j->targetId;
				_steps["steps"][_step_n]["shippedBoxes"][box_index]["x"] = j->pos.x;
				_steps["steps"][_step_n]["shippedBoxes"][box_index]["y"] = j->pos.y;
				_steps["steps"][_step_n]["shippedBoxes"][box_index]["z"] = j->pos.z;

				_steps["steps"][_step_n]["shippedBoxes"][box_index]["half_size_x"] = j->half_size.x;
				_steps["steps"][_step_n]["shippedBoxes"][box_index]["half_size_y"] = j->half_size.y;
				_steps["steps"][_step_n]["shippedBoxes"][box_index]["half_size_z"] = j->half_size.z;
				box_index++;
				j++;
			}			
		}
		if (i != 0)
			_steps["steps"][_step_n]["shippedResources"] = 0;
		_steps["steps"][_step_n]["destinationPointId"] = _travel_points[i + 1];
	}

	_steps["steps"][_step_n]["shippedBoxes"] = Json::arrayValue;
	_steps["steps"][_step_n]["shippedResources"] = 0;
	_steps["steps"][_step_n]["destinationPointId"] = 0;
	_step_n++;
}
