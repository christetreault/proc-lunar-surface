#ifndef CITY_H
#define CITY_H

#include "SceneGraph.hpp"
#include "Util.hpp" // RNG

class City : public Group /* or whatever else sort of Node suits you */
{
public:
  City(unsigned int seed/* TODO: other params */);
private:
};

#endif
