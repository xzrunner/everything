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

    static const model::BrushModel* GetBrushModel(const n0::SceneNode& node);

    static void AddMaterialComp(n0::SceneNode& node);

    static void StoreBrush(n0::SceneNode& node, std::unique_ptr<model::BrushModel>& brush_model);
    static void BuildPolymesh(n0::SceneNode& node, const model::BrushModel& brush_model);
    static void UpdatePolymesh(n0::SceneNode& node, const model::BrushModel& brush_model);

}; // NodeHelper

}