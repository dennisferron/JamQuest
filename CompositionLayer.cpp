#include "CompositionLayer.hpp"

#include <iostream>

CompositionLayer::CompositionLayer(std::string name)
    : name(name)
{
}

CompositionLayer::~CompositionLayer()
{
}

void CompositionLayer::update(uint32_t delta_ms)
{
}

void CompositionLayer::debug_print(int indent) const
{
    for (int i=0; i<indent; i++)
        std::cout << "   ";

    std::cout << name << "\n";
}