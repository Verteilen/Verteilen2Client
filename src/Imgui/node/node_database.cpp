#include "node_database.h"


std::vector<Node>* m_Nodes;
std::function<void(Node*)> BuildNode;
std::function<int()> GetNextId;

ImColor Aktion::Database::GetIconColor(PinType type)
{
  switch (type)
  {
  default:
  case PinType::Flow:     return ImColor(255, 255, 255);
  case PinType::Bool:     return ImColor(220, 48, 48);
  case PinType::Int:      return ImColor(68, 201, 156);
  case PinType::Float:    return ImColor(147, 226, 74);
  case PinType::String:   return ImColor(124, 21, 153);
  case PinType::Object:   return ImColor(51, 150, 215);
  case PinType::Function: return ImColor(218, 0, 183);
  case PinType::Delegate: return ImColor(255, 48, 48);
  }
};

void Aktion::Database::Node_Database(std::vector<Node>* _node, std::function<void(Node*)> buildNodeFunc, std::function<int()> getNextIDFunc)
{
  m_Nodes = _node;
  BuildNode = buildNodeFunc;
  GetNextId = getNextIDFunc;
}

Node* Aktion::Database::Spawn_Start()
{
  m_Nodes->emplace_back(GetNextId(), "Start", ImColor(255, 128, 128));
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Run", PinType::Flow);
  
  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Update()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Update", ImColor(255, 128, 128)));
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Run", PinType::Flow);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "time", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "delta", PinType::Float);

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Key()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Key", ImColor(255, 128, 128)));
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Run", PinType::Flow);

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Keydown()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Keydown", ImColor(255, 128, 128)));
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Run", PinType::Flow);

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Keyup()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Keyup", ImColor(255, 128, 128)));
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Run", PinType::Flow);

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}

Node* Aktion::Database::Spawn_If()
{
  m_Nodes->emplace_back(Node(GetNextId(), "If", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "Run", PinType::Flow);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "If", PinType::Flow);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Else", PinType::Flow);
  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_And()
{
  m_Nodes->emplace_back(Node(GetNextId(), "And", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "v1", PinType::Bool);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "v2", PinType::Bool);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Or()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Or", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "v1", PinType::Bool);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "v2", PinType::Bool);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Not()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Not", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "v1", PinType::Bool);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), "v2", PinType::Bool);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}

Node* Aktion::Database::Spawn_PrintMessage()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Print Message", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "Run", PinType::Flow);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "Message", PinType::Flow);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "NextRun", PinType::Flow);
  m_Nodes->back().Script = ""
    "function Event_Run ()"
    "  Print(Get_Message())"
    "  Event_NextRun()"
    "end"
    "";
  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}

Node* Aktion::Database::Spawn_Addition()
{
  m_Nodes->emplace_back(Node(GetNextId(), "+", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Float);
  m_Nodes->back().Type = NodeType::Simple;
  m_Nodes->back().Script = ""
    "function Get_Result ()"
    "  return Get_v1() + Get_v2()"
    "end"
    "";
  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Subtraction()
{
  m_Nodes->emplace_back(Node(GetNextId(), "-", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Float);
  m_Nodes->back().Type = NodeType::Simple;
  m_Nodes->back().Script = ""
    "function Get_Result ()"
    "  return Get_v1() - Get_v2()"
    "end"
    "";

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Multiply()
{
  m_Nodes->emplace_back(Node(GetNextId(), "*", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Float);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Division()
{
  m_Nodes->emplace_back(Node(GetNextId(), "/", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Float);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Power()
{
  m_Nodes->emplace_back(Node(GetNextId(), "^", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Float);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}

Node* Aktion::Database::Spawn_Greater()
{
  m_Nodes->emplace_back(Node(GetNextId(), ">", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Greater_Equal()
{
  m_Nodes->emplace_back(Node(GetNextId(), ">=", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Less()
{
  m_Nodes->emplace_back(Node(GetNextId(), "<", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Less_Equal()
{
  m_Nodes->emplace_back(Node(GetNextId(), "<=", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
Node* Aktion::Database::Spawn_Equal()
{
  m_Nodes->emplace_back(Node(GetNextId(), "==", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v1", PinType::Float);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "v2", PinType::Float);
  m_Nodes->back().Outputs.emplace_back(GetNextId(), &m_Nodes->back(), "Result", PinType::Bool);
  m_Nodes->back().Type = NodeType::Simple;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}

Node* Aktion::Database::Spawn_ExportView()
{
  m_Nodes->emplace_back(Node(GetNextId(), "Export View", ImColor(255, 128, 128)));
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "Monitor", PinType::Int);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "Fullscreen", PinType::Bool);
  m_Nodes->back().Inputs.emplace_back(GetNextId(), &m_Nodes->back(), "Rect", PinType::Object);
  m_Nodes->back().Type = NodeType::Blueprint;

  BuildNode(&m_Nodes->back());
  return &m_Nodes->back();
}
