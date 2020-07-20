#pragma once

#include <SFML/Graphics.hpp>
#include "DA_Types.hpp"
#include <thread>
#include <vector>
#include <map>
#include <string.h>
#include <mutex>
#include <iostream>

namespace DA {
	class GUI {
	public:
		GUI();
		~GUI();

		sf::Vector2u windowSize = { 1020u, 640u };
		float zoomDevisor = 10.f;
		int posTransX = 0;
		int posTransY = 0;

		void openWindow(const char* windowName, sf::Vector2u windowSize);
		bool isWindowRunning();
		void closeWindow();

		void clearPoints();
		void clearLines();
		void clearText();
		void clearRectangles();
		void clearRings();
		void clearTriangles();

		void addPointType(std::string typeName, float radius = 0.5f, std::string colorCommand = "Black", bool moveable = false);
		void addPointType(std::string typeName, float radius = 0.5f, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, bool moveable = false);
		
		void addLineType(std::string typeName, std::string color1Command = "Black", std::string color2Command = "Black", bool moveable = false);
		void addLineType(std::string typeName, sf::Uint8 r1 = 0u, sf::Uint8 g1 = 0u, sf::Uint8 b1 = 0u, sf::Uint8 a1 = 0u, sf::Uint8 r2 = 0u, sf::Uint8 g2 = 0u, sf::Uint8 b2 = 0u, sf::Uint8 a2 = 0u, bool moveable = false);
		
		void addTextType(std::string typeName, const char* fontLink, sf::Vector2f position, unsigned int charSize = 12u, std::string  colorCommand = "Black", bool moveable = false);
		void addTextType(std::string typeName, const char* fontLink, sf::Vector2f position, unsigned int charSize = 12u, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, bool moveable = false);

		void addPoints(std::vector<DA::Point> &pointVec, std::string pointType);
		void addPoint(DA::Point point, std::string pointType);

		void addLines(std::vector<DA::Point> &point1Vec, std::vector<DA::Point> &point2, std::string lineType);
		void addLines(std::vector<DA::Point> &point1Vec, DA::Point point2, std::string lineType);

		void addText(std::string text, std::string textType);

		void addRectangle(float length, float width, float rotation, sf::Vector2f position, std::string colorCommand = "Black", bool moveable = false);
		void addRectangle(float length, float width, float rotation, sf::Vector2f position, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, bool moveable = false);

		void addRing(float radius, float thickness, sf::Vector2f position, std::string colorCommand = "Black", bool moveable = false);
		void addRing(float radius, float thickness, sf::Vector2f position, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, bool moveable = false);

		void addTriangle(float sidelength, float rotation,  sf::Vector2f position, std::string colorCommand = "Black", bool moveable = false);
		void addTriangle(float sidelength, float rotation,  sf::Vector2f position, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, bool moveable = false);

		void changePoints(std::vector<DA::Point> &pointVec, std::string pointType);

		void changeLines(std::vector<DA::Point> &point1Vec, std::vector<DA::Point> point2Vec, std::string lineType);
		void changeLines(std::vector<DA::Point> &point1Vec, DA::Point point2, std::string lineType);

		void changeText(std::string text, std::string textType);
		void changeText(std::string text, std::string textType, sf::Vector2f newPos);

		void changeRectangle(float length, float width, float rotation, sf::Vector2f position, std::string colorCommand = "Black", unsigned char rectangleType = 0u, bool moveable = false);
		void changeRectangle(float length, float width, float rotation, sf::Vector2f position, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, unsigned char rectangleType = 0u, bool moveable = false);

		void changeRing(float radius, float thickness, sf::Vector2f position, std::string colorCommand = "Black", unsigned char ringType = 0u, bool moveable = false);
		void changeRing(float radius, float thickness, sf::Vector2f position, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, unsigned char ringType = 0u, bool moveable = false);
		
		void changeTriangle(float sidelength, float rotation, sf::Vector2f position, std::string colorCommand = "Black", unsigned char triangleType = 0u, bool moveable = false);
		void changeTriangle(float sidelength, float rotation, sf::Vector2f position, sf::Uint8 r = 0u, sf::Uint8 g = 0u, sf::Uint8 b = 0u, sf::Uint8 a = 0u, unsigned char triangleType = 0u, bool moveable = false);

	private:
		struct pointTypeContainer {
			std::vector<DA::Point> pointValues;

			float radius = 0.5f;

			sf::Color color = sf::Color::White;

			bool moveable = false;
		};

		struct lineTypeContainer {
			std::vector<DA::Point> point1;
			std::vector<DA::Point> point2;

			unsigned int size = 0u;

			sf::Color color1 = sf::Color::Black;
			sf::Color color2 = sf::Color::Black;

			bool moveable = false;
		};

		struct textTypeContainer {
			sf::Vector2f position = { 0.f,0.f };
			float rotation = 0.f;

			std::string content;

			unsigned int charSize = 12u;

			sf::Font font;
			sf::Color color;

			bool moveable = false;
		};

		struct rectangleContainer {
			sf::Vector2f position = { 0.f,0.f };
			sf::Vector2f size = { 0.f,0.f };
			float rotation = 0.f;

			sf::Color color;

			bool moveable = false;
		};

		struct ringContainer {
			sf::Vector2f position = { 0.f,0.f };
			float radius = 10.f;
			float thickness = 0.5f;

			sf::Color color;

			bool moveable = false;
		};

		struct triangleContainer {
			sf::Vector2f position = { 0.f,0.f };
			float rotation = 0.f;

			float sidelength = 0.5f;

			sf::Color color = sf::Color::White;

			bool moveable = false;
		};

		void _openWindow(const char* windowName);

		void _windowHandler();

		sf::Color evaluateCollorCommand(std::string colorCommand);

		std::map<std::string, pointTypeContainer> pointTypes;
		std::map<std::string, lineTypeContainer> lineTypes;
		std::map<std::string, textTypeContainer> textTypes;
		std::vector<rectangleContainer> rectangles;
		std::vector<ringContainer> rings;
		std::vector<triangleContainer> triangles;

		std::mutex zoomFactor_mtx;
		std::mutex posTransX_mtx;
		std::mutex posTransY_mtx;

		std::mutex points_mtx;
		std::mutex lines_mtx;
		std::mutex text_mtx;
		std::mutex rectangle_mtx;
		std::mutex rings_mtx;
		std::mutex triangles_mtx;

		void pushPointsToDrawBuffer();
		void pushLinesToDrawBuffer();
		void pushStringToDrawBuffer();
		void pushRectanglesToDrawBuffer();
		void pushRingsToDrawBuffer();
		void pushTrianglesToDrawBuffer();

		sf::RenderWindow *window;
		std::thread window_th;
		std::thread visual_th;

		bool windowRunning = false;

		const char* windowName;

		sf::Color backgroundColor = sf::Color::White;

	};
}