#pragma once

#include <node0/typedef.h>

#include <memory>

namespace model { class BrushModel; }
namespace pm3 { struct Brush; }

namespace evt
{

class Node;

class NodeHelper
{
public:
    static n0::SceneNodePtr ClonePrevSceneObj(const Node& node, int prev_idx);

    static const model::BrushModel* GetBrushModel(const n0::SceneNodePtr& node);
    static const std::shared_ptr<pm3::Brush> GetBrush(const n0::SceneNodePtr& node);

    static void UpdateModelFromBrush(n0::SceneNode& node, const model::BrushModel& brush);
    static void UpdateModelFromBrush(n0::SceneNode& node, const std::shared_ptr<pm3::Brush>& brush);

}; // NodeHelper

}