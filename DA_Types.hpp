#pragma once

namespace DA {
	typedef struct {
		float x, y;
		bool associated = false;
	}Point;

	struct Landmark {
		Landmark(DA::Point P) : karthesian(P) {}

		DA::Point karthesian;
		unsigned int visionCount = 1;
	};

	typedef struct {
		float k, d;
	}Function;

}