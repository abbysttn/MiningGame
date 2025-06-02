#ifndef _QUADTREE_H__
#define _QUADTREE_H__

#include <vector>
#include <memory>

using namespace std;

struct Box {
	float x, y;
	float width, height;

	Box() : x(0), y(0), height(0), width(0) {}

	Box(float x, float y, float width, float height) : x(x), y(y), height(height), width(width) {}

	bool contains(float pointX, float pointY) const {
		return (pointX >= x && pointX <= x + width && pointY >= y && pointY <= y + height);
	}

	bool intersects(const Box& box) const {
		return !(box.x > x + width || box.x + box.width < x
			|| box.y > y + height || box.y + box.height < y);
	}
};

class GameObject;


class QuadTree {
public:
	QuadTree(const Box& bounds);
	~QuadTree();

	void clear();
	void subdivide();
	void insert(GameObject* obj, const Box& bounds);
	vector<GameObject*> queryRange(const Box& range);

protected:
	QuadTree(const QuadTree& quadTree);
	QuadTree& operator=(const QuadTree& quadTree);

private:
	struct QuadTreeItem {
		GameObject* obj;
		Box bounds;
	};

public:

protected:
	static const int capacity = 4;
	Box boundary;
	vector<QuadTreeItem> objects;
	vector<unique_ptr<QuadTree>> children;

	bool isDivided = false;

};

#endif // !_QUADTREE_H__
