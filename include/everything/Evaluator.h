#pragma once

#include <vector>
#include <memory>

namespace evt
{

class Node;

class Evaluator
{
public:
    static void Update(const std::vector<std::shared_ptr<Node>>& nodes);
    static void Draw(const std::vector<std::shared_ptr<Node>>& nodes);

}; // Evaluator

}
