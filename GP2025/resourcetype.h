#ifndef __RESOURCETYPE_H__
#define __RESOURCETYPE_H__

#include <string>

enum class ResourceType
{
	DIRT,
	GEM,
	STONE,
	COUNT
};

inline std::string ResourceTypeToString(ResourceType type) 
{
    switch (type) 
    {
    case ResourceType::DIRT:  return "Dirt";
    case ResourceType::GEM:   return "Gem";
    case ResourceType::STONE: return "Stone";
    default:                  return "Unknown Resource";
    }
}

#endif // !__RESOURCETYPE_H__
