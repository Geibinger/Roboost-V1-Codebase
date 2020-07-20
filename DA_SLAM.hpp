#pragma once

#include "DA_RANSAC.hpp"
#include "timer.hpp"
#include <Eigen/Dense>

namespace DA {
	class SLAM {
	public:
		SLAM(DA::RANSAC &ransac) {
			this->ransac = &ransac;
		}
		~SLAM() {}

		void landmarkAssociation(std::vector<DA::Point> &unassociatedLM) {
			for (unsigned int i = 0u; i < unassociatedLM.size(); i++) {
				bool unseen = true;
				for (unsigned int j = 0; j < this->Landmarks.size(); j++) {
					if (DA::calcDist(unassociatedLM.at(i), this->Landmarks.at(j).karthesian) < this->landmarkRadius) {
						this->Landmarks.at(j).karthesian.x = (this->Landmarks.at(j).karthesian.x + unassociatedLM.at(i).x) / 2.f;
						this->Landmarks.at(j).karthesian.y = (this->Landmarks.at(j).karthesian.y + unassociatedLM.at(i).y) / 2.f;
						this->Landmarks.at(j).visionCount++;
						unseen = false;
					}
				}
				if (unseen) {
					this->Landmarks.push_back(unassociatedLM.at(i));
				}
			}
		}

		std::vector<DA::Landmark> Landmarks;

	private:


		Eigen::Vector3f prevRobotState = Eigen::Vector3f(0.f, 0.f, 0.f);	// x, y, theta of the robot at t (preveous)			3	x 1
		Eigen::VectorXf prevMapState;									// xi, yi of the landmark at t (preveous)		2*n	x 1

		Eigen::Vector3f robotState = Eigen::Vector3f(0.f, 0.f, 0.f);	// x, y, theta of the robot at t + 1 (current)			3	x 1
		Eigen::VectorXf mapState;										// xi, yi of the landmark at t + 1 (current)		2*n	x 1

		float landmarkRadius = 10.f;

		DA::RANSAC *ransac;
	};
}
