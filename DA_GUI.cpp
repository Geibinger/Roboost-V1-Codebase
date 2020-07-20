#include "DA_GUI.h"

// public:

DA::GUI::GUI() {

}
DA::GUI::~GUI() {
	if (this->visual_th.joinable()) this->visual_th.join();
	if (this->window_th.joinable()) this->window_th.join();
}

void DA::GUI::openWindow(const char* windowName, sf::Vector2u windowSize) {
	this->visual_th = std::thread(&GUI::_openWindow, this, windowName);
	this->windowSize = windowSize;
}

bool DA::GUI::isWindowRunning() {
	return this->windowRunning;
}

void DA::GUI::closeWindow() {
	if (this->visual_th.joinable()) this->visual_th.join();
	if (this->window_th.joinable()) this->window_th.join();
}

void DA::GUI::clearPoints() {
	std::lock_guard<std::mutex> points_lck(this->points_mtx);
	this->pointTypes.clear();
}


void DA::GUI::clearLines() {
	std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
	this->lineTypes.clear();
}

void DA::GUI::clearText() {
	std::lock_guard<std::mutex> text_lck(this->text_mtx);
	this->textTypes.clear();
}

void DA::GUI::clearRectangles() {
	std::lock_guard<std::mutex> rectangle_lck(this->rectangle_mtx);
	this->rectangles.clear();
}

void DA::GUI::clearRings() {
	std::lock_guard<std::mutex> rectangle_lck(this->rectangle_mtx);
	this->rings.clear();
}

void DA::GUI::clearTriangles() {
	std::lock_guard<std::mutex> triangles_lck(this->triangles_mtx);
	this->triangles.clear();
}

void DA::GUI::addPointType(std::string typeName, float radius, std::string colorCommand, bool moveable) {
	std::lock_guard<std::mutex> points_lck(this->points_mtx);
	pointTypeContainer tempContainer;
	tempContainer.radius = radius;
	tempContainer.color = this->evaluateCollorCommand(colorCommand);
	tempContainer.moveable = moveable;

	this->pointTypes.insert(std::pair<std::string, DA::GUI::pointTypeContainer>(typeName, tempContainer));
}

void DA::GUI::addPointType(std::string typeName, float radius, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, bool moveable) {
	std::lock_guard<std::mutex> points_lck(this->points_mtx);
	pointTypeContainer tempContainer;
	tempContainer.radius = radius;
	tempContainer.color = { r, g, b, a };
	tempContainer.moveable = moveable;

	this->pointTypes.insert(std::pair<std::string, DA::GUI::pointTypeContainer>(typeName, tempContainer));
}

void DA::GUI::addLineType(std::string typeName, std::string color1Command, std::string color2Command, bool moveable) {
	std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
	lineTypeContainer tempContainer;
	tempContainer.color1 = this->evaluateCollorCommand(color1Command);
	tempContainer.color2 = this->evaluateCollorCommand(color2Command);
	tempContainer.moveable = moveable;

	this->lineTypes.insert(std::pair<std::string, DA::GUI::lineTypeContainer>(typeName, tempContainer));
}

void DA::GUI::addLineType(std::string typeName, sf::Uint8 r1, sf::Uint8 g1, sf::Uint8 b1, sf::Uint8 a1, sf::Uint8 r2, sf::Uint8 g2, sf::Uint8 b2, sf::Uint8 a2, bool moveable) {
	std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
	lineTypeContainer tempContainer;
	tempContainer.color1 = { r1, g1, b1, a1 };
	tempContainer.color2 = { r2, g2, b2, a1 };
	tempContainer.moveable = moveable;

	this->lineTypes.insert(std::pair<std::string, DA::GUI::lineTypeContainer>(typeName, tempContainer));
}

void DA::GUI::addTextType(std::string typeName, const char* fontLink, sf::Vector2f position, unsigned int charSize, std::string  colorCommand, bool moveable) {
	std::lock_guard<std::mutex> text_lck(this->text_mtx);
	textTypeContainer tempContainer;
	if (tempContainer.font.loadFromFile(fontLink));
	else printf_s("<VISUAL>\tERROR:\tfont not availiable!\n");
	tempContainer.position = position;
	tempContainer.charSize = charSize;
	tempContainer.color = this->evaluateCollorCommand(colorCommand);
	tempContainer.moveable = moveable;

	this->textTypes.insert(std::pair<std::string, DA::GUI::textTypeContainer>(typeName, tempContainer));
}

void DA::GUI::addTextType(std::string typeName, const char* fontLink, sf::Vector2f position, unsigned int charSize, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, bool moveable) {
	std::lock_guard<std::mutex> text_lck(this->text_mtx);
	textTypeContainer tempContainer;
	if (tempContainer.font.loadFromFile(fontLink));
	else printf_s("<VISUAL>\tERROR:\tfont not availiable!\n");
	tempContainer.position = position;
	tempContainer.charSize = charSize;
	tempContainer.color = { r, g, b, a };

	this->textTypes.insert(std::pair<std::string, DA::GUI::textTypeContainer>(typeName, tempContainer));
}

void DA::GUI::addPoints(std::vector<DA::Point> &pointVec, std::string pointType) {
	std::lock_guard<std::mutex> points_lck(this->points_mtx);
	if (!this->pointTypes.empty()) {
		this->pointTypes.at(pointType).pointValues.insert(this->pointTypes.at(pointType).pointValues.end(), pointVec.begin(), pointVec.end());
	}
	else printf_s("<VISUAL>\tERROR:\tpointtype not availiable!\n");
}

void DA::GUI::addPoint(DA::Point point, std::string pointType) {
	std::lock_guard<std::mutex> points_lck(this->points_mtx);
	if (!this->pointTypes.empty()) {
		this->pointTypes.at(pointType).pointValues.emplace_back(point);
	}
	else printf_s("<VISUAL>\tERROR:\tpointtype not availiable!\n");
}

void DA::GUI::addLines(std::vector<DA::Point> &point1Vec, std::vector<DA::Point> &point2Vec, std::string lineType) {
	std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
	if (!this->lineTypes.empty()) {
		this->lineTypes.at(lineType).size += point1Vec.size();
		this->lineTypes.at(lineType).point1.insert(this->lineTypes.at(lineType).point1.end(), point1Vec.begin(), point1Vec.end());
		this->lineTypes.at(lineType).point2.insert(this->lineTypes.at(lineType).point2.end(), point2Vec.begin(), point2Vec.end());
	}
	else printf_s("<VISUAL>\tERROR:\tlinetype not availiable!\n");
}

void DA::GUI::addLines(std::vector<DA::Point> &point1Vec, DA::Point point2, std::string lineType) {
	std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
	if (!this->lineTypes.empty()) {
		this->lineTypes.at(lineType).size += point1Vec.size();
		this->lineTypes.at(lineType).point1.insert(this->lineTypes.at(lineType).point1.end(), point1Vec.begin(), point1Vec.end());
		for (unsigned int i = 0u; i < point1Vec.size(); i++) {
			this->lineTypes.at(lineType).point2.emplace_back(point2);
		}
	}
	else printf_s("<VISUAL>\tERROR:\tlinetype not availiable!\n");
}

void DA::GUI::addText(std::string text, std::string textType) {
	std::lock_guard<std::mutex> text_lck(this->text_mtx);
	if (!this->textTypes.empty()) {
		this->textTypes.at(textType).content.append(text);
	}
	else printf_s("<VISUAL>\tERROR:\ttexttype not availiable!\n");
}

void DA::GUI::addRectangle(float length, float width, float rotation, sf::Vector2f position, std::string colorCommand, bool moveable) {
	std::lock_guard<std::mutex> rectangle_lck(this->rectangle_mtx);

	rectangleContainer tempContainer;

	tempContainer.color = this->evaluateCollorCommand(colorCommand);
	tempContainer.position = position;
	tempContainer.size = { length, width };
	tempContainer.rotation = rotation;

	this->rectangles.emplace_back(tempContainer);
}

void DA::GUI::addRectangle(float length, float width, float rotation, sf::Vector2f position, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, bool moveable) {
	std::lock_guard<std::mutex> rectangle_lck(this->rectangle_mtx);

	rectangleContainer tempContainer;

	tempContainer.color = { r, g, b, a };
	tempContainer.position = position;
	tempContainer.size = { length, width };
	tempContainer.moveable = moveable;
	tempContainer.rotation = rotation;

	this->rectangles.emplace_back(tempContainer);
}

void DA::GUI::addRing(float radius, float thickness, sf::Vector2f position, std::string colorCommand, bool moveable) {
	std::lock_guard<std::mutex> rings_lck(this->rings_mtx);

	ringContainer tempContainer;

	tempContainer.color = this->evaluateCollorCommand(colorCommand);
	tempContainer.position = position;
	tempContainer.radius = radius;
	tempContainer.thickness = thickness;
	tempContainer.moveable = moveable;

	this->rings.emplace_back(tempContainer);
}

void DA::GUI::addRing(float radius, float thickness, sf::Vector2f position, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, bool moveable) {
	std::lock_guard<std::mutex> rings_lck(this->rings_mtx);

	ringContainer tempContainer;

	tempContainer.color = { r, g, b, a };
	tempContainer.position = position;
	tempContainer.radius = radius;
	tempContainer.thickness = thickness;
	tempContainer.moveable = moveable;

	this->rings.emplace_back(tempContainer);
}

void DA::GUI::addTriangle(float sidelength, float rotation, sf::Vector2f position, std::string colorCommand, bool moveable) {
	std::lock_guard<std::mutex> triangles_lck(this->triangles_mtx);

	triangleContainer tempContainer;

	tempContainer.color = this->evaluateCollorCommand(colorCommand);
	tempContainer.position = position;
	tempContainer.sidelength = sidelength;
	tempContainer.moveable = moveable;
	tempContainer.rotation = rotation;

	this->triangles.emplace_back(tempContainer);
}

void DA::GUI::addTriangle(float sidelength, float rotation, sf::Vector2f position, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, bool moveable) {
	std::lock_guard<std::mutex> triangles_lck(this->triangles_mtx);

	triangleContainer tempContainer;

	tempContainer.color = { r, g, b, a };
	tempContainer.position = position;
	tempContainer.sidelength = sidelength;
	tempContainer.moveable = moveable;
	tempContainer.rotation = rotation;

	this->triangles.emplace_back(tempContainer);
}

void DA::GUI::changePoints(std::vector<DA::Point> &pointVec, std::string pointType) {
	std::lock_guard<std::mutex> points_lck(this->points_mtx);
	if (!this->pointTypes.empty()) {
		this->pointTypes.at(pointType).pointValues.clear();
		this->pointTypes.at(pointType).pointValues.insert(this->pointTypes.at(pointType).pointValues.end(), pointVec.begin(), pointVec.end());
	}
	else printf_s("<VISUAL>\tERROR:\tpointtype not availiable!\n");
}

void DA::GUI::changeLines(std::vector<DA::Point> &point1Vec, std::vector<DA::Point> point2Vec, std::string lineType) {
	std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
	if (!this->lineTypes.empty()) {
		this->lineTypes.at(lineType).size = point1Vec.size();
		this->lineTypes.at(lineType).point1.clear();
		this->lineTypes.at(lineType).point2.clear();
		this->lineTypes.at(lineType).point1.insert(this->lineTypes.at(lineType).point1.end(), point1Vec.begin(), point1Vec.end());
		this->lineTypes.at(lineType).point2.insert(this->lineTypes.at(lineType).point2.end(), point2Vec.begin(), point2Vec.end());
	}
	else printf_s("<VISUAL>\tERROR:\tlinetype not availiable!\n");
}

void DA::GUI::changeLines(std::vector<DA::Point> &point1Vec, DA::Point point2, std::string lineType) {
	std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
	if (!this->lineTypes.empty()) {
		this->lineTypes.at(lineType).size = point1Vec.size();
		this->lineTypes.at(lineType).point1.clear();
		this->lineTypes.at(lineType).point2.clear();
		this->lineTypes.at(lineType).point1.insert(this->lineTypes.at(lineType).point1.end(), point1Vec.begin(), point1Vec.end());
		for (unsigned int i = 0u; i < point1Vec.size(); i++) {
			this->lineTypes.at(lineType).point2.emplace_back(point2);
		}
	}
	else printf_s("<VISUAL>\tERROR:\tlinetype not availiable!\n");
}

void DA::GUI::changeText(std::string text, std::string textType) {
	std::lock_guard<std::mutex> text_lck(this->text_mtx);
	if (!this->textTypes.empty()) {
		this->textTypes.at(textType).content.clear();
		this->textTypes.at(textType).content.append(text);
	}
	else printf_s("<VISUAL>\tERROR:\ttexttype not availiable!\n");
}

void DA::GUI::changeText(std::string text, std::string textType, sf::Vector2f newPos) {
	std::lock_guard<std::mutex> text_lck(this->text_mtx);
	if (!this->textTypes.empty()) {
		this->textTypes.at(textType).content.clear();
		this->textTypes.at(textType).content.append(text);
		this->textTypes.at(textType).position = newPos;
	}
	else printf_s("<VISUAL>\tERROR:\ttexttype not availiable!\n");
}

void DA::GUI::changeRectangle(float length, float width, float rotation, sf::Vector2f position, std::string colorCommand, unsigned char rectangleType, bool moveable) {
	std::lock_guard<std::mutex> rectangle_lck(this->rectangle_mtx);
	if (this->rectangles.size() > rectangleType) {
		this->rectangles.at(rectangleType).color = this->evaluateCollorCommand(colorCommand);
		this->rectangles.at(rectangleType).position = position;
		this->rectangles.at(rectangleType).rotation = rotation;
		this->rectangles.at(rectangleType).size = { length, width };
		this->rectangles.at(rectangleType).moveable = moveable;
	}
	else printf_s("<VISUAL>\tERROR:\trectangletype not availiable!\n");
}

void DA::GUI::changeRectangle(float length, float width, float rotation, sf::Vector2f position, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, unsigned char rectangleType, bool moveable) {
	std::lock_guard<std::mutex> rectangle_lck(this->rectangle_mtx);
	if (this->rectangles.size() > rectangleType) {
		this->rectangles.at(rectangleType).color = { r, g, b, a };
		this->rectangles.at(rectangleType).position = position;
		this->rectangles.at(rectangleType).rotation = rotation;
		this->rectangles.at(rectangleType).size = { length, width };
		this->rectangles.at(rectangleType).moveable = moveable;
	}
	else printf_s("<VISUAL>\tERROR:\trectangletype not availiable!\n");
}

void DA::GUI::changeRing(float radius, float thickness, sf::Vector2f position, std::string colorCommand, unsigned char ringType, bool moveable) {
	std::lock_guard<std::mutex> rings_lck(this->rings_mtx);
	if (this->rings.size() > ringType) {
		this->rings.at(ringType).color = this->evaluateCollorCommand(colorCommand);
		this->rings.at(ringType).position = position;
		this->rings.at(ringType).radius = radius;
		this->rings.at(ringType).thickness = thickness;
		this->rings.at(ringType).moveable = moveable;
	}
	else printf_s("<VISUAL>\tERROR:\tringtype not availiable!\n");
}

void DA::GUI::changeRing(float radius, float thickness, sf::Vector2f position, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, unsigned char ringType, bool moveable) {
	std::lock_guard<std::mutex> rings_lck(this->rings_mtx);
	if (this->rings.size() > ringType) {
		this->rings.at(ringType).color = { r, g, b, a };
		this->rings.at(ringType).position = position;
		this->rings.at(ringType).radius = radius;
		this->rings.at(ringType).thickness = thickness;
		this->rings.at(ringType).moveable = moveable;
	}
	else printf_s("<VISUAL>\tERROR:\tringtype not availiable!\n");
}

void DA::GUI::changeTriangle(float sidelength, float rotation, sf::Vector2f position, std::string colorCommand, unsigned char triangleType, bool moveable) {
	std::lock_guard<std::mutex> triangles_lck(this->triangles_mtx);
	if (this->triangles.size() > triangleType) {
		this->triangles.at(triangleType).color = this->evaluateCollorCommand(colorCommand);
		this->triangles.at(triangleType).position = position;
		this->triangles.at(triangleType).rotation = rotation;
		this->triangles.at(triangleType).sidelength = sidelength;
		this->triangles.at(triangleType).moveable = moveable;
	}
	else printf_s("<VISUAL>\tERROR:\ttriangletype not availiable!\n");
}

void DA::GUI::changeTriangle(float sidelength, float rotation, sf::Vector2f position, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a, unsigned char triangleType, bool moveable) {
	std::lock_guard<std::mutex> triangles_lck(this->triangles_mtx);
	if (this->triangles.size() > triangleType) {
		this->triangles.at(triangleType).color = { r, g, b, a };
		this->triangles.at(triangleType).position = position;
		this->triangles.at(triangleType).rotation = rotation;
		this->triangles.at(triangleType).sidelength = sidelength;
		this->triangles.at(triangleType).moveable = moveable;
	}
	else printf_s("<VISUAL>\tERROR:\ttriangletype not availiable!\n");
}

// private:

void DA::GUI::_windowHandler() {
	this->window->setActive(true);

	while (this->window->isOpen()) {
		window->clear(this->backgroundColor);

		{
			std::lock_guard<std::mutex> rings_lck(this->rings_mtx);
			std::lock_guard<std::mutex> rectangle_lck(this->rectangle_mtx);
			std::lock_guard<std::mutex> points_lck(this->points_mtx);
			std::lock_guard<std::mutex> lines_lck(this->lines_mtx);
			std::lock_guard<std::mutex> text_lck(this->text_mtx);

			std::lock_guard<std::mutex> posTransX_lck(this->posTransX_mtx);
			std::lock_guard<std::mutex> posTransY_lck(this->posTransY_mtx);
			std::lock_guard<std::mutex> zoomFactor_lck(this->zoomFactor_mtx);

			this->pushRingsToDrawBuffer();
			this->pushPointsToDrawBuffer();
			this->pushLinesToDrawBuffer();
			this->pushRectanglesToDrawBuffer();
			this->pushStringToDrawBuffer();
			this->pushTrianglesToDrawBuffer();
		}

		this->window->display();
	}
}

void DA::GUI::pushPointsToDrawBuffer() {
	std::map<std::string, pointTypeContainer>::iterator it;
	for (it = this->pointTypes.begin(); it != this->pointTypes.end(); it++) {
		if (it->second.moveable) {
			for (unsigned int j = 0; j < it->second.pointValues.size(); j++) {
				sf::CircleShape tempDrawablePoint;

				tempDrawablePoint.setPosition(it->second.pointValues.at(j).x / this->zoomDevisor, it->second.pointValues.at(j).y / this->zoomDevisor);
				tempDrawablePoint.setFillColor(it->second.color);
				tempDrawablePoint.setRadius(it->second.radius);
				tempDrawablePoint.move(this->posTransX - tempDrawablePoint.getRadius() + this->windowSize.x / 2.f, this->posTransY - tempDrawablePoint.getRadius() + this->windowSize.y / 2.f);

				this->window->draw(tempDrawablePoint);
			}
		}
		else {
			for (unsigned int j = 0; j < it->second.pointValues.size(); j++) {
				sf::CircleShape tempDrawablePoint;

				tempDrawablePoint.setPosition(it->second.pointValues.at(j).x, it->second.pointValues.at(j).y);
				tempDrawablePoint.setFillColor(it->second.color);
				tempDrawablePoint.setRadius(it->second.radius);

				this->window->draw(tempDrawablePoint);
			}
		}

	}
}


void DA::GUI::pushLinesToDrawBuffer() {
	std::map<std::string, lineTypeContainer>::iterator it;
	for (it = this->lineTypes.begin(); it != this->lineTypes.end(); it++) {
		if (it->second.moveable) {
			for (unsigned int j = 0; j < it->second.size; j++) {
				sf::Vertex tempDrawableLine[] =
				{
					sf::Vertex(sf::Vector2f((it->second.point1.at(j).x / this->zoomDevisor) + this->posTransX + this->windowSize.x / 2.f, (it->second.point1.at(j).y / this->zoomDevisor) + this->posTransY + this->windowSize.y / 2.f), it->second.color1),

					sf::Vertex(sf::Vector2f((it->second.point2.at(j).x / this->zoomDevisor) + this->posTransX + this->windowSize.x / 2.f, (it->second.point2.at(j).y / this->zoomDevisor) + this->posTransY + this->windowSize.y / 2.f), it->second.color2)
				};


				this->window->draw(tempDrawableLine, 2, sf::Lines);
			}
		}
		else {
			for (unsigned int j = 0; j < it->second.size; j++) {
				sf::Vertex tempDrawableLine[] =
				{
					sf::Vertex(sf::Vector2f(it->second.point1.at(j).x, it->second.point1.at(j).y), it->second.color1),

					sf::Vertex(sf::Vector2f(it->second.point2.at(j).x, it->second.point2.at(j).y), it->second.color2)
				};

				this->window->draw(tempDrawableLine, 2, sf::Lines);
			}
		}
	}
}

void DA::GUI::pushStringToDrawBuffer() {
	std::map<std::string, textTypeContainer>::iterator it;
	for (it = this->textTypes.begin(); it != this->textTypes.end(); it++) {
		if (it->second.moveable) {
			sf::Text tempText;
			tempText.setString(it->second.content);
			tempText.setFont(it->second.font);
			tempText.setFillColor(it->second.color);
			tempText.setPosition(it->second.position.x, it->second.position.y);
			tempText.setCharacterSize(it->second.charSize);

			this->window->draw(tempText);
		}
		else {
			sf::Text tempText;
			tempText.setString(it->second.content);
			tempText.setFont(it->second.font);
			tempText.setFillColor(it->second.color);
			tempText.setPosition(it->second.position);
			tempText.setCharacterSize(it->second.charSize);

			this->window->draw(tempText);
		}
	}
}

void DA::GUI::pushRectanglesToDrawBuffer() {
	for (unsigned int i = 0; i < this->rectangles.size(); i++) {
		sf::RectangleShape tempRectangle;
		if (this->rectangles.at(i).moveable) {
			tempRectangle.setFillColor(this->rectangles.at(i).color);
			tempRectangle.setPosition((this->rectangles.at(i).position.x / this->zoomDevisor) + this->posTransX + this->windowSize.x / 2.f, (this->rectangles.at(i).position.y / this->zoomDevisor) + this->posTransY + this->windowSize.y / 2.f);
			tempRectangle.setSize(this->rectangles.at(i).size);
			tempRectangle.rotate(this->rectangles.at(i).rotation);
		}
		else {
			tempRectangle.setFillColor(this->rectangles.at(i).color);
			tempRectangle.setPosition(this->rectangles.at(i).position);
			tempRectangle.setSize(this->rectangles.at(i).size);
			tempRectangle.rotate(this->rectangles.at(i).rotation);
		}
		this->window->draw(tempRectangle);
	}
}

void DA::GUI::pushRingsToDrawBuffer() {
	for (unsigned int i = 0; i < this->rings.size(); i++) {
		if (this->rings.at(i).moveable) {
			sf::CircleShape tempRing(this->rings.at(i).radius / this->zoomDevisor);

			tempRing.setFillColor({ 255u,255u,255u,0u });
			tempRing.setOutlineThickness(this->rings.at(i).thickness);
			tempRing.setOutlineColor(this->rings.at(i).color);
			tempRing.setPosition(this->rings.at(i).position);
			tempRing.move(this->posTransX - tempRing.getRadius() + this->windowSize.x / 2.f, this->posTransY - tempRing.getRadius() + this->windowSize.y / 2.f);

			this->window->draw(tempRing);
		}
		else {
			sf::CircleShape tempRing(this->rings.at(i).radius);

			tempRing.setFillColor({ 255u,255u,255u,0u });
			tempRing.setOutlineThickness(this->rings.at(i).thickness);
			tempRing.setOutlineColor(this->rings.at(i).color);
			tempRing.setPosition(this->rings.at(i).position);

			this->window->draw(tempRing);
		}
	}
}

void DA::GUI::pushTrianglesToDrawBuffer() {
	for (unsigned int i = 0; i < this->triangles.size(); i++) {
		if (this->triangles.at(i).moveable) {
			sf::CircleShape tempTriangle(this->triangles.at(i).sidelength, 3);

			tempTriangle.setFillColor(this->triangles.at(i).color);
			tempTriangle.setPosition(this->triangles.at(i).position);
			tempTriangle.move(this->posTransX + this->windowSize.x / 2.f - this->triangles.at(i).sidelength, this->posTransY + this->windowSize.y / 2.f - this->triangles.at(i).sidelength);
			tempTriangle.rotate(this->triangles.at(i).rotation);

			this->window->draw(tempTriangle);
		}
		else {
			sf::CircleShape tempTriangle(this->triangles.at(i).sidelength, 3);

			tempTriangle.setFillColor(this->triangles.at(i).color);
			tempTriangle.setPosition(this->triangles.at(i).position);
			tempTriangle.move(this->triangles.at(i).sidelength, this->triangles.at(i).sidelength);
			tempTriangle.rotate(this->triangles.at(i).rotation);

			this->window->draw(tempTriangle);
		}
	}
}

sf::Color DA::GUI::evaluateCollorCommand(std::string colorCommand) {
	if (colorCommand._Equal("White")) {
		return sf::Color::White;
	}
	else if (colorCommand._Equal("Red")) {
		return sf::Color::Red;
	}
	else if (colorCommand._Equal("Black")) {
		return sf::Color::Black;
	}
	else if (colorCommand._Equal("Blue")) {
		return sf::Color::Blue;
	}
	else {
		printf_s("<VISUAL>\tERROR:\tcolor not availiable!\n");
		return sf::Color::White;
	}
}

void DA::GUI::_openWindow(const char* windowName) {

	this->windowName = windowName;
	sf::RenderWindow tempWindow(sf::VideoMode(this->windowSize.x, this->windowSize.y), this->windowName, sf::Style::Close);
	this->window = &tempWindow;
	this->window->setActive(false);
	this->window->setFramerateLimit(40u);

	this->window_th = std::thread(&GUI::_windowHandler, this);

	this->addTextType("comments", "Roboto-Medium.ttf", { 0.f, 0.f }, 10u, "Black", true);

	sf::Vector2i mousePos = sf::Mouse::getPosition(*this->window);

	sf::Event event;
	// run the program as long as the window is open
	while (this->window->isOpen())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(10));

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			mousePos = sf::Mouse::getPosition(*this->window);

			this->posTransX = mousePos.x - (int)this->windowSize.x / 2;
			this->posTransY = mousePos.y - (int)this->windowSize.y / 2;

			//std::cout << "mouse: " << mousePos.x << "\t" << mousePos.y << "window: " << this->posTransX << "\t" << this->posTransY << std::endl;

			this->changeText("", "comments");
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			mousePos = sf::Mouse::getPosition(*this->window);


			std::string pointInfo;

			pointInfo.append(std::to_string((mousePos.x - this->posTransX - (int)this->windowSize.x / 2) * this->zoomDevisor));
			pointInfo.append(" : ");
			pointInfo.append(std::to_string((mousePos.y - this->posTransY - (int)this->windowSize.y / 2) * this->zoomDevisor));
			this->changeText(pointInfo, "comments", { ((float)mousePos.x), (float)mousePos.y });
		}

		// check all the window's events that were triggered since the last iteration of the loop
		while (this->window->pollEvent(event))
		{
			
			switch (event.type) {
			case sf::Event::Closed:
				this->windowRunning = false;
				this->window->close();
				break;
			case sf::Event::MouseWheelMoved:
				this->changeText("", "comments");
				if (event.mouseWheel.delta == 1) {
					std::lock_guard<std::mutex> zoomFactor_lck(this->zoomFactor_mtx);
					if (this->zoomDevisor >= 4) {
						this->zoomDevisor -= 0.5f;
					}
					else if (this->zoomDevisor >= 1 && this->zoomDevisor < 4) {
						this->zoomDevisor -= 0.1f;
					}
					else this->zoomDevisor *= 0.99f;
				}
				else {
					std::lock_guard<std::mutex> zoomFactor_lck(this->zoomFactor_mtx);
					if (this->zoomDevisor >= 4) {
						this->zoomDevisor += 0.5f;
					}
					else if (this->zoomDevisor >= 1 && this->zoomDevisor < 4) {
						this->zoomDevisor += 0.1f;
					}
					else this->zoomDevisor += 0.05f;
				}
				break;
			case sf::Event::Resized:
				this->windowSize = sf::Vector2u(event.size.width, event.size.height);
				break;
				//this->window->getDefaultView().setSize(sf::Vector2f( static_cast<float>(event.size.width),  static_cast<float>(event.size.height)));
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::W:
				{
					std::lock_guard<std::mutex> posTransY_lck(this->posTransY_mtx);
					this->posTransY += 1;
					break;
				}
				case sf::Keyboard::S:
				{
					std::lock_guard<std::mutex> posTransY_lck(this->posTransY_mtx);
					this->posTransY -= 1;
					break;
				}
				case sf::Keyboard::A:
				{
					std::lock_guard<std::mutex> posTransX_lck(this->posTransX_mtx);
					this->posTransX += 1;
					break;
				}
				case sf::Keyboard::D:
				{
					std::lock_guard<std::mutex> posTransX_lck(this->posTransX_mtx);
					this->posTransX -= 1;
					break;
				}
				case sf::Keyboard::R:
				{
					this->changeText("", "comments");
					std::lock_guard<std::mutex> zoomFactor_lck(this->zoomFactor_mtx);
					std::lock_guard<std::mutex> posTransY_lck(this->posTransY_mtx);
					std::lock_guard<std::mutex> posTransX_lck(this->posTransX_mtx);
					this->posTransX = 0;
					this->posTransY = 0;
					this->zoomDevisor = 10.f;
					break;
				}
				case sf::Keyboard::Add:
				{
					this->changeText("", "comments");
					std::lock_guard<std::mutex> zoomFactor_lck(this->zoomFactor_mtx);
					if (this->zoomDevisor >= 4) {
						this->zoomDevisor -= 0.5f;
					}
					else if (this->zoomDevisor >= 1 && this->zoomDevisor < 4) {
						this->zoomDevisor -= 0.1f;
					}
					else this->zoomDevisor *= 0.99f;
					break;
				}
				case sf::Keyboard::Subtract:
				{
					this->changeText("", "comments");
					std::lock_guard<std::mutex> zoomFactor_lck(this->zoomFactor_mtx);
					if (this->zoomDevisor >= 4) {
						this->zoomDevisor += 0.5f;
					}
					else if (this->zoomDevisor >= 1 && this->zoomDevisor < 4) {
						this->zoomDevisor += 0.1f;
					}
					else this->zoomDevisor += 0.05f;
					break;
				}
				default: printf_s("<Visualizer> INFO:\t Key not memorized!\n");
				}
				break;

				
			}
		}
	}

	if (this->window_th.joinable()) this->window_th.join();
}