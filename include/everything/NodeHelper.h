#pragma once

#include <model/BrushModel.h>
#include <node0/typedef.h>

#include <memory>

namespace evt
{

class Node;

class NodeHelper
{
public:
    static n0::SceneNodePtr ClonePrevSceneObj(const Node& node, int prev_idx);

    static const model::BrushModel* GetBrushModel(const n0::SceneNodePtr& node);

    static void UpdateModelFromBrush(n0::SceneNode& node, const model::BrushModel& brush);

}; // NodeHelper

}