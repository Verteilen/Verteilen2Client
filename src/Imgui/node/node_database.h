#pragma once
#include <functional>
#include <Imgui/imgui_node_data.h>

namespace Aktion {
	namespace Database {
		//
		//  Logic part of database
		//
#pragma region Logic
		ImColor GetIconColor(PinType type);
		void Node_Database(std::vector<Node>* _node, std::function<void(Node*)> buildNodeFunc, std::function<int()> getNextIDFunc);

		// Event Type
		Node* Spawn_Start();
		Node* Spawn_Update();
		Node* Spawn_Key();
		Node* Spawn_Keydown();
		Node* Spawn_Keyup();

		// Logic
		Node* Spawn_If();
		Node* Spawn_And();
		Node* Spawn_Or();
		Node* Spawn_Not();

		// Utility
		Node* Spawn_PrintMessage();

		// Math
		Node* Spawn_Addition();
		Node* Spawn_Subtraction();
		Node* Spawn_Multiply();
		Node* Spawn_Division();
		Node* Spawn_Power();

		// Compare
		Node* Spawn_Greater();
		Node* Spawn_Greater_Equal();
		Node* Spawn_Less();
		Node* Spawn_Less_Equal();
		Node* Spawn_Equal(); 
#pragma endregion

		//
		//  Comp part of database
		//
#pragma region Comp
		Node* Spawn_ExportView();
#pragma endregion

	}
}

