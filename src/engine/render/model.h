#pragma once
#include <assimp/mesh.h>
#include <cinttypes>
#include <vector>

typedef struct aiMesh;

namespace Aktion::Engine {

	struct AktionModel 
	{
		aiMesh mesh;
	};


	void Model_Init(AktionModel& model);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="file"></param>
	/// <param name="target"></param>
	/// <returns>How many model does the file have</returns>
	std::vector<AktionModel> Model_Import(const char* file);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="memory"></param>
	/// <param name="size"></param>
	/// <param name="target"></param>
	/// <returns>How many model does the file have</returns>
	std::vector<AktionModel> Model_Import(const void* memory, size_t size);

}