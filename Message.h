#pragma once

//Should be in its own header but lazyness
struct GameObject {
	int id;
	int x;
	int y;
	int radius = 100;

	GameObject(int objID, int objX, int objY) {
		id = objID;
		x = objX;
		y = objY;
	}

	GameObject() {
		id = -1;
		x = -1;
		y = -1;
	}

	void update(int objX, int objY) {
		x = objX;
		y = objY;
	}

	void moveCircle(int t) {
		x += radius * sin(t);
		y += radius * cos(t);
	}

	void randMove(int t) {
		x += (rand() % (t+1))*10;
		y += (rand() % (t+1))*10;
	}
	void print() {
		std::cout << "ID: " << id << " x: " << x << " y: " << y << std::endl;
	}
};


struct Message {
	int x, y;
	int objectID;


	Message(int id, int xO, int yO) {
		objectID = id;
		x = xO;
		y = yO;
	}

	void print() {
		std::cout << "Object ID: " << objectID << "x: " << x << "y: " << y << std::endl;
	}
};