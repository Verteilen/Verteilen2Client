#include "model.h"
#include <utility/dependencyInclude.h>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define DEFAULT_ASSIMP_FLAG \
	aiProcess_CalcTangentSpace | \
	aiProcess_Triangulate | \
	aiProcess_GenNormals | \
	aiProcess_JoinIdenticalVertices | \
	aiProcess_SortByPType


void Aktion::Engine::Model_Init(AktionModel& model)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(aiVector3D) * model.mesh.mNumVertices;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

std::vector<Aktion::Engine::AktionModel> Aktion::Engine::Model_Import(const char* file)
{
	if (!std::filesystem::exists(file)) return std::vector<Aktion::Engine::AktionModel>(0);
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, DEFAULT_ASSIMP_FLAG);
	std::vector<Aktion::Engine::AktionModel> result = std::vector<Aktion::Engine::AktionModel>(scene->mNumMeshes);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		result[i] = { *scene->mMeshes[i] };
	}

	return result;
}

std::vector<Aktion::Engine::AktionModel> Aktion::Engine::Model_Import(const void* memory, size_t size)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(memory, size, DEFAULT_ASSIMP_FLAG);
	std::vector<Aktion::Engine::AktionModel> result = std::vector<Aktion::Engine::AktionModel>(scene->mNumMeshes);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		result[i] = { *scene->mMeshes[i] };
	}

	return result;
}
