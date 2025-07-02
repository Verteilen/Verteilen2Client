#pragma once
#include <system/config.h>
#include <imgui_node_editor.h>
#include <string>
#include <vector>

enum class PinType
{
    Flow,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
    Delegate,
};

enum class PinKind
{
    Output,
    Input
};

enum class NodeType
{
    Blueprint,
    Simple,
    Tree,
    Comment,
    Houdini
};

struct Node;

struct Pin
{
    ax::NodeEditor::PinId   ID;
    ::Node*     Node;
    std::string Name;
    PinType     Type;
    PinKind     Kind;

    Pin(int id, const char* name, PinType type):
        ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
    {
    }
    Pin(int id, ::Node* host, const char* name, PinType type) :
      ID(id), Node(host), Name(name), Type(type), Kind(PinKind::Input)
    {
    }
};

struct Node
{
    ax::NodeEditor::NodeId ID;
    std::string Name;
    std::string Meta;
    std::string Description;
    std::string Script;
    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;
    ImColor Color;
    NodeType Type;
    ImVec2 Size;

    std::string State;
    std::string SavedState;


    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
        ID(id), Name(name), Meta(""), Color(color), Type(NodeType::Blueprint), Size(0, 0),
      Inputs(std::vector<Pin>()), Outputs(std::vector<Pin>())
    {
    }

    Node(int id, const char* name, const char* meta, ImColor color = ImColor(255, 255, 255)) :
      ID(id), Name(name), Meta(meta), Color(color), Type(NodeType::Blueprint), Size(0, 0),
      Inputs(std::vector<Pin>()), Outputs(std::vector<Pin>())
    {
    }
};

struct Link
{
    ax::NodeEditor::LinkId ID;

    ax::NodeEditor::PinId StartPinID;
    ax::NodeEditor::PinId EndPinID;

    ImColor Color;

    Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId):
        ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
    {
    }
};

struct NodeIdLess
{
    bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};