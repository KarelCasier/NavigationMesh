#pragma once
#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert> // Remove for release - To catch bugs

template <typename Resource, typename Identifier>
class ResourceHolder
{
public:
	void load(Identifier id, const std::string& filename);

	template <typename Parameter>
	void load(Identifier id, const std::string& filename, const Parameter& secondParam); //Overloaded function for resource types that need multiple parameteres: Music, Shader... etc 

	Resource& get(Identifier id);
	const Resource& get(Identifier id) const;  //Overloaded function if accessing from a const ResourceHolder


private:
	void insertResource(Identifier id, std::unique_ptr<Resource> resource);

private:
	std::map<Identifier, std::unique_ptr<Resource>>	mResourceMap;
};
#include "ResourceHolder.inl"
