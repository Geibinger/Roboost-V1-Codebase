#pragma once

#include "DA_Types.hpp"
#include "DA_LIDAR.h"
#include <math.h>
#include <vector>

// useful functions
namespace DA {
	DA::Function fitPointsToLine(const DA::Point &A, const DA::Point &B) {
		DA::Function f = { 0,0 };
		if ((B.x - A.x == 0.f) || (&B == &A)) return f;
		else {
			f.k = ((B.y - A.y) / (B.x - A.x));
			f.d = (A.y - f.k * A.x);
		}

		return f;
	}
	DA::Function leastSquares(const std::vector<DA::Point> &points) {
		DA::Function func;

		unsigned int count = points.size();

		DA::Point sum = { 0.f,0.f };
		DA::Point avr = { 0.f,0.f };

		for (unsigned int i = 0; i < count; i++) {
			sum.x += points.at(i).x;
			sum.y += points.at(i).y;
		}
		avr.x = sum.x / count;
		avr.y = sum.y / count;

		float sumXY = 0;
		float sumXX = 0;

		for (unsigned int i = 0; i < count; i++) {
			sumXY += points.at(i).x * points.at(i).y;
			sumXX += points.at(i).x * points.at(i).x;
		}

		func.k = (sumXY - count * avr.x*avr.y) / (sumXX - count * avr.x*avr.x);
		func.d = avr.y - avr.x * func.k;

		return func;
	}

	DA::Point randomAvailablePoint(const std::vector<DA::Point> &laserReadings) {
		for (unsigned int i = 0u; i < 600u; i++) {
			unsigned int randomNum = rand() % laserReadings.size();
			if (!laserReadings.at(randomNum).associated) return laserReadings.at(randomNum);
			else continue;
		}
	}

	DA::Point functionIntersection(const DA::Function &f1, const DA::Function &f2) {
		DA::Point intersec = { 0,0 };
		intersec.x = (f2.d - f1.d) / (f1.k - f2.k);
		intersec.y = f1.k*intersec.x + f1.d;
		return intersec;
	}
	DA::Point intersectionPerpToPoint(const DA::Function &y1, const DA::Point &point) {
		DA::Point intersec;
		DA::Function y2 = { 0,0 };
		y2.k = -1 / y1.k;
		y2.d = point.y - y2.k*point.x;
		return functionIntersection(y1, y2);
	}

	float calcDist(const DA::Point &A, const DA::Point &B) {
		return sqrt((A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y));
	}

	float distPerpToPoint(const DA::Function &y1, const DA::Point &point) {
		DA::Point intersec = intersectionPerpToPoint(y1, point);
		return DA::calcDist(intersec, point);
	}	
}

namespace DA {
	class RANSAC {

	public:
		RANSAC(){}
		~RANSAC(){}

		void RANdomSAmpleConsensus(std::vector<DA::Point> &pointCloud) {
			const unsigned int count = pointCloud.size();
			std::vector<DA::Point> landMarks;

			unsigned int  unassociatedLaserReadings = count;
			std::vector<DA::Point> leastSquaresReadings;
			unsigned int pointsWithinThreshhold = 0;

			for (unsigned int i = 0; unassociatedLaserReadings > this->consens && i <= maxTrials; ++i) {

				DA::Function linearFit = DA::fitPointsToLine(DA::randomAvailablePoint(pointCloud), DA::randomAvailablePoint(pointCloud));
				if (linearFit.d == 0.f && linearFit.k == 0.f) continue;

				pointsWithinThreshhold = 0;

				for (unsigned int l = 0; l < count; l++) {
					if ((DA::distPerpToPoint(linearFit, pointCloud.at(l)) <= threshold) && !pointCloud.at(l).associated) {
						leastSquaresReadings.emplace_back(pointCloud.at(l));
					}
				}

				linearFit = DA::leastSquares(leastSquaresReadings);

				for (unsigned int m = 0; m < count; m++) {
					if ((DA::distPerpToPoint(linearFit, pointCloud.at(m)) <= threshold) && !pointCloud.at(m).associated) {
						pointsWithinThreshhold++;
					}
				}

				// check if certain number of point lie within the threshhold		
				if (pointsWithinThreshhold >= minPointsWithinThreshhold) {
					for (unsigned int n = 0; n < count; n++) {
						if ((DA::distPerpToPoint(linearFit, pointCloud.at(n)) <= threshold) && !pointCloud.at(n).associated) {
							pointCloud.at(n).associated = true;
							unassociatedLaserReadings--;
						}
					}
					landMarks.emplace_back(DA::intersectionPerpToPoint(linearFit, this->origin));	// BFL with correction
				}


				/*
					0) Punkte: Attribut bool associated = false
					1) random 2 Points mit associated = false -> Gerade aufstellen
					2) wenn in threshhold mindestens ~50 Daten mit associated=false dann:
								least squares von diesen ~50 Daten => Funktion fit1
					3) fit1 threshhold durchführen => alle im Threshhold liegenden Punkte dieser fit1 zuordnen (associated = true);
					4) alle neuen Punkte mit associated=true bei int unsassociatedLaserReadings abziehen
					5) Wiederhole 1)-4) bis unassociatedLaserReading < ~50 ist.
				*/

				leastSquaresReadings.clear();
			}
			this->landMarks = landMarks; // this->filterLandmarks(landMarks);}
		}

		void setThreshold(float newThreshold){
			this->threshold = newThreshold;
		}
		void setConsens(unsigned int newConsens){
			this->consens = newConsens;
		}
		void setMinPointsWT(unsigned int newMinPointsWT){
			this->minPointsWithinThreshhold = newMinPointsWT;
		}
		void setMaxTrials(unsigned int newMaxTrials){
			this->maxTrials = newMaxTrials;
		}
		void setLMRadius(float newLMRadius){
			this->landmarkRadius = newLMRadius;
		}

		float getThreshold(){
			return this->threshold;
		}
		unsigned int getConsens(){
			return this->consens;
		}
		unsigned int getMinPointsWT(){
			return this->minPointsWithinThreshhold;
		}
		unsigned int getMaxTrials(){
			return this->maxTrials;
		}
		float getLMRadius(){
			return this->landmarkRadius;
		}
		std::vector<DA::Point> getLandMarks(){
			return this->landMarks;
		}

	private:
		DA::Point origin = { 0 };	// gets updated via SLAM
		std::vector<DA::Point> landMarks;

		// PARAMETERS:
		float threshold = 16.f;
		unsigned int consens = 10u;	// min unassociated laser readings (points within line deviation)
		unsigned int minPointsWithinThreshhold = 35u;
		unsigned int maxTrials = 100u;
		float landmarkRadius = 30.f;

		std::vector<DA::Point> filterLandmarks(std::vector<DA::Point> LM) {
			std::vector<DA::Point> filteredLM;
			bool valid = true;
			for (unsigned int i = 0u; i < LM.size(); i++) {
				for (unsigned int j = i + 1u; j < LM.size(); j++) {
					if ((DA::calcDist(LM.at(i), LM.at(j)) < this->landmarkRadius)) valid = false;
				}
				if (valid) {
					filteredLM.emplace_back(LM.at(i));
				}
				else valid = true;
			}
			return filteredLM;
		}
	};
}
