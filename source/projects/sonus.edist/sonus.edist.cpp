/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>
#include <vector>

using namespace c74::min;

class edist : public object<edist>
{
public:
	MIN_DESCRIPTION {"Calculate the euclidean distance between 2 n-dimensional points"};
	MIN_TAGS {"math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in_1 {this, "(list) Coordinates of the first point"};
	inlet<>  in_2 {this, "(list) Coordinates of the second point"};
	outlet<> out {this, "(list) Euclidean distance"};

	message<> list
	{
		this,
		"list",
		"Coordinates of the point.",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				point_1_.clear();

				for (auto n = 0; n < args.size(); n++)
				{
					point_1_.push_back(double(args[n]));
				}
			}
			if (inlet == 1)
			{
				point_2_.clear();

				for (auto n = 0; n < args.size(); n++)
				{
					point_2_.push_back(double(args[n]));
				}
			}

			distance_();

			return {};
		}
    };

	private:
	std::vector<double> point_1_;
	std::vector<double> point_2_;
	
	inline void distance_()
	{
		double distance = 0.0;

		if (!point_1_.empty() && !point_2_.empty())
		{
			auto dims = std::min(point_1_.size(), point_2_.size());

			for (auto d = 0; d < dims; d++)
			{
				distance += std::pow(point_1_.at(d) - point_2_.at(d), 2.0);
			}

			distance = std::sqrt(distance);
		}

		out.send(distance);
	}
};

MIN_EXTERNAL(edist);
