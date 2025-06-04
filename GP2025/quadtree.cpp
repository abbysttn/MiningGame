#include "quadtree.h"

#include "gameobject.h"

QuadTree::QuadTree(const Box& bounds) : boundary(bounds) {}

QuadTree::~QuadTree()
{
	clear();
}

void QuadTree::clear()
{
	objects.clear();

	for (auto& child : children) {
		if (child) {
			child->clear();
		}
	}

	children.clear();
	isDivided = false;
}

void QuadTree::subdivide()
{
	float hw = boundary.width / 2;
	float hh = boundary.height / 2;

	children.emplace_back(new QuadTree(Box(boundary.x, boundary.y, hw, hh)));
	children.emplace_back(new QuadTree(Box(boundary.x + hw, boundary.y, hw, hh)));
	children.emplace_back(new QuadTree(Box(boundary.x, boundary.y + hh, hw, hh)));
	children.emplace_back(new QuadTree(Box(boundary.x + hw, boundary.y + hh, hw, hh)));

	isDivided = true;
}

void QuadTree::insert(GameObject* obj, const Box& bounds)
{
	if (!boundary.intersects(bounds)) return;

	if (objects.size() < capacity && !isDivided) {
		objects.push_back({ obj, bounds });
		return;
	}

	if (!isDivided) subdivide();

	for (auto& child : children) {
		child->insert(obj, bounds);
	}
}

vector<GameObject*> QuadTree::queryRange(const Box& range)
{
	vector<GameObject*> foundObjects;

	if (!boundary.intersects(range)) return foundObjects;

	for (auto& item : objects) {
		if (range.intersects(item.bounds)) {
			foundObjects.push_back(item.obj);
		}
	}

	if (isDivided) {
		for (auto& child : children) {
			auto childResults = child->queryRange(range);
			foundObjects.insert(foundObjects.end(), childResults.begin(), childResults.end());
		}
	}

	return foundObjects;
}
