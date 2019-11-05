#ifndef GALAXYPATHFINDER_HPP
# define GALAXYPATHFINDER_HPP

# include "IGalaxyPathFinder.h"
# include "NewGalaxy.hpp"
# include <vector>
# include <list>
# include <forward_list>
# include <map>
# include <string>

#include "Container.hpp"

class GalaxyPathFinder : public IGalaxyPathFinder
{
private:
	std::string							_captain_name;
	ship								_ship;
	std::list<Container>				_containers;
	std::map<int, targetPoint>			_tp;
	std::map<int, targetPoint>			_tp_ordered;
	std::list<box>						_boxes;
	std::vector<int>					_bad_boxes_id;

public:
	GalaxyPathFinder();
	GalaxyPathFinder(char *name);
	~GalaxyPathFinder() {
		// _containers.clear();
		// _tp.clear();
		// _tp_ordered.clear();
		// _boxes.clear();
		// _bad_boxes_id.clear();
		_containers.front().clear_static();
	};

	int3			parseInt3(Json::Value val);
	float3			parseFloat3(Json::Value val);
	void			parseShip(Json::Value ship);
	void			parsePoints(Json::Value points);
	void			parseBoxes(Json::Value boxes);
	void			parse(const char* inputJasonFile);

	void			calculate_distances();
	void			fill_bf();

	void 			FindSolution(const char* inputJasonFile, const char* outputFileName);
	const char*		ShowCaptainName();
};

#endif /* GALAXYPATHFINDER_HPP */
