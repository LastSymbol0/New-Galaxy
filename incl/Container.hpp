#ifndef CONTAINER_HPP
# define CONTAINER_HPP

# include "NewGalaxy.hpp"
# include <vector>
# include <list>

struct	fill_area_point {
	int3	pos;
	int3	lim_x;
		// lim_x.x - limit for box.size.x
		// lim_x.y - limit for lim_x by Y axis
		//		(if new pushing point is farther (by Y) then this val -> new point has no lim_x
		//			else new point have same lim_x.x, but new_p.lim_x.y = old_p.lim_x.y - box.y)
		// lim_x.z - limit for lim_x by Z axis
	int3	lim_y;
	int3	lim_z;
};

class Container
{
private:
	std::list<box>		_boxes;
	float				_weight;
	int					_cap;
	float				_res_weight;
	std::vector<int>	_travel_points;
	std::list<int3>		_fill_area;
	std::list<fill_area_point>		_fill_area_n;

	static ship			_ship;
	static Json::Value	_steps;
	static int			_step_n;

public:
	Container(ship s);
	Container();

	void				clear_static();

	bool				isEmpty() const;
	void				setShip(ship ship);
	Json::Value			getSteps() const ;
	int					getStepN() const ;
	float				getResW() const ;
	float				getW() const ;

	fill_area_point		set_point_limits(box b, fill_area_point old, fill_area_point p);
	void				new_limits(int3 vertex1, int3 vertex2, std::list<fill_area_point>::iterator cur);
	void				push_new_points(box b, fill_area_point old, int3 vertex );
	bool				setBox(box b, std::map<int, targetPoint> tp);
	void				printSteps(std::map<int, targetPoint> tp);

};

#endif /* CONTAINER_HPP */
