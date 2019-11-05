#ifndef NEWGALAXY_HPP
# define NEWGALAXY_HPP

# define LAYER
# define BF

# define EPSILON 20

# include <json/json.h>
# include <json/json-forwards.h>
# include <cmath>

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
	// int3	operator*(int a) { return ((int3){x * a, y * a, z * a}); }
};

struct int2
{
	int		x, y;
};

struct targetPoint
{
	int		id;
	int		order_i;
	// float	distance_to_next;
	// float	distance_to_root;
	float3	pos;
};

struct box
{
	int		id;
	int		targetId;
	float	weight;
	int3	pos;
	// int3	size;
	int3	half_size;
	int		half_cap;
};

struct ship
{
	// int3	size;
	int3	half_size;
	float	max_res_w;
	float	max_carrying_w;
	float	res_consumption;
	// float	max_distanse;
	int		half_cap;
};

struct step
{
	std::vector<box>	shippedBoxes;
    float				shippedResources;
    int					destinationPointId;
};


#endif /* NEWGALAXY_HPP */
