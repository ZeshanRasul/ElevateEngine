#include "Tools/Logger.h"
#include "GltfModel.h"

GltfModel::GltfModel(std::string modelFilename, Shader* shader)
	: m_filename(modelFilename), m_shader(shader)
{
	LoadModel();
}

bool GltfModel::LoadModel()
{
	m_model = std::make_shared<tinygltf::Model>();

	size_t uniformMatrixBufferSize = 3 * sizeof(glm::mat4);
	mUniformBuffer.init(uniformMatrixBufferSize);
	Logger::log(1, "%s: matrix uniform buffer (size %i bytes) successfully created\n", __FUNCTION__, uniformMatrixBufferSize);

	tinygltf::TinyGLTF gltfLoader;
	std::string loaderErrors;
	std::string loaderWarnings;
	bool result = false;

	result = gltfLoader.LoadBinaryFromFile(m_model.get(), &loaderErrors, &loaderWarnings,
		m_filename);

	if (!loaderWarnings.empty())
	{
		Logger::log(1, "%s: warnings while loading glTF model:\n%s\n", __FUNCTION__,
			loaderWarnings.c_str());
	}

	if (!loaderErrors.empty())
	{
		Logger::log(1, "%s: errors while loading glTF model:\n%s\n", __FUNCTION__,
			loaderErrors.c_str());
	}

	if (!result)
	{
		Logger::log(1, "%s error: could not load file '%s'\n", __FUNCTION__,
			m_filename.c_str());
		return false;
	}

	meshData.resize(m_model->meshes.size());
	int planeCount = 1;
	for (size_t meshIndex = 0; meshIndex < m_model->meshes.size(); ++meshIndex) {
		const tinygltf::Mesh& mesh = m_model->meshes[meshIndex];
		GLTFMesh gltfMesh;


		std::vector<glm::vec3> meshVerts;

		for (size_t primIndex = 0; primIndex < mesh.primitives.size(); ++primIndex) {


			const tinygltf::Primitive& primitive = mesh.primitives[primIndex];
			GLTFPrimitive gltfPrim = {};
			gltfPrim.mode = primitive.mode; // usually GL_TRIANGLES

			gltfPrim.material = primitive.material;

			// --- Create VAO ---
			glGenVertexArrays(1, &gltfPrim.vao);
			glBindVertexArray(gltfPrim.vao);

			// --- Upload vertex attributes ---
			for (const auto& attrib : primitive.attributes) {
				const std::string& attribName = attrib.first; // "POSITION", "NORMAL", "TEXCOORD_0", etc.
				int accessorIndex = attrib.second;
				const tinygltf::Accessor& accessor = m_model->accessors[accessorIndex];
				const tinygltf::BufferView& bufferView = m_model->bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];

				GLuint vbo;
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);

				if (attribName == "POSITION") {
					int numPositionEntries = static_cast<int>(accessor.count);
					Logger::log(1, "%s: loaded %i vertices from glTF file\n", __FUNCTION__,
						numPositionEntries);

					// Extract vertices
					const float* positions = reinterpret_cast<const float*>(
						buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);

					for (int i = 0; i < numPositionEntries; i++)
					{
						gltfPrim.verts.push_back(glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]));
						meshVerts.push_back(glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]));
						gltfPrim.vertexCount++;
					}
				}

				const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
				size_t dataSize = accessor.count * tinygltf::GetNumComponentsInType(accessor.type) * tinygltf::GetComponentSizeInBytes(accessor.componentType);

				glBufferData(GL_ARRAY_BUFFER, dataSize, dataPtr, GL_STATIC_DRAW);

				// Determine attribute layout location (you must match your shader locations)
				GLint location = -1;
				if (attribName == "POSITION") location = 0;
				else if (attribName == "NORMAL") location = 1;
				else if (attribName == "TEXCOORD_0") location = 2;
				else if (attribName == "TEXCOORD_1") location = 3;
				else if (attribName == "TEXCOORD_2") location = 4;
				// Add JOINTS_0, WEIGHTS_0 etc. if needed

				if (location >= 0) {
					GLint numComponents = tinygltf::GetNumComponentsInType(accessor.type); // e.g. VEC3 -> 3
					GLenum glType = accessor.componentType; // GL_FLOAT, GL_UNSIGNED_SHORT, etc.

					glEnableVertexAttribArray(location);
					glVertexAttribPointer(location, numComponents, glType,
						accessor.normalized ? GL_TRUE : GL_FALSE,
						bufferView.byteStride ? bufferView.byteStride : 0,
						(const void*)0);
				}

				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}


			if (primitive.indices >= 0) {
				// Get the accessor, bufferview, and buffer for the index data
				const tinygltf::Accessor& indexAccessor = m_model->accessors[primitive.indices];
				const tinygltf::BufferView& bufferView = m_model->bufferViews[indexAccessor.bufferView];
				const tinygltf::Buffer& buffer = m_model->buffers[bufferView.buffer];



				// Pointer to the actual index data
				const unsigned char* dataPtr = buffer.data.data() + bufferView.byteOffset + indexAccessor.byteOffset;

				// Loop through and extract indices based on the component type
				for (size_t i = 0; i < indexAccessor.count; ++i) {
					switch (indexAccessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						gltfPrim.indices.push_back(static_cast<unsigned int>(reinterpret_cast<const uint8_t*>(dataPtr)[i]));
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						gltfPrim.indices.push_back(static_cast<unsigned int>(reinterpret_cast<const uint16_t*>(dataPtr)[i]));
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
						gltfPrim.indices.push_back(reinterpret_cast<const uint32_t*>(dataPtr)[i]);
						break;
					}
					default:
						Logger::log(1, " << indexAccessor.componentType, %zu", indexAccessor.componentType);
						break;
					}
				}
			}
			else {
				// No index buffer: assume the primitive is non-indexed (each vertex is used once)
				int posAccessorIndex = primitive.attributes.at("POSITION");
				const tinygltf::Accessor& posAccessor = m_model->accessors[posAccessorIndex];
				for (size_t i = 0; i < posAccessor.count; ++i) {
					gltfPrim.indices.push_back(static_cast<unsigned int>(i));
				}
			}

			if (!gltfPrim.indices.empty()) {
				glGenBuffers(1, &gltfPrim.indexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gltfPrim.indexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					gltfPrim.indices.size() * sizeof(unsigned int),
					gltfPrim.indices.data(),
					GL_STATIC_DRAW);

				gltfPrim.indexCount = static_cast<GLsizei>(gltfPrim.indices.size());
				gltfPrim.indexType = GL_UNSIGNED_INT;
			}
			else {
				gltfPrim.indexBuffer = 0;
				gltfPrim.indexCount = 0;
			}

			glBindVertexArray(0);


			gltfMesh.primitives.push_back(gltfPrim);
		}

		meshData[meshIndex] = gltfMesh;
	}

	return true;
}

void GltfModel::CreateVertexBuffers()
{
	const tinygltf::Primitive& primitives = m_model->meshes.at(0).primitives.at(0);
	m_vertexVbo.resize(primitives.attributes.size() + 1);
	m_attribAccessors.resize(primitives.attributes.size());

	std::vector<glm::vec3> tangents(m_vertices.size(), glm::vec3(0.0f));

	std::vector<glm::vec3> vertPositions(m_vertices.size(), glm::vec3(0.0f));
	std::vector<glm::vec3> normals(m_vertices.size(), glm::vec3(0.0f));
	std::vector<glm::vec2> texCoords(m_vertices.size(), glm::vec2(0.0f));

	for (const auto& attrib : primitives.attributes)
	{
		const std::string attribType = attrib.first;
		const int accessorNum = attrib.second;

		const tinygltf::Accessor& accessor = m_model->accessors.at(accessorNum);
		const tinygltf::BufferView& bufferView = m_model->bufferViews.at(accessor.bufferView);
		const tinygltf::Buffer& buffer = m_model->buffers.at(bufferView.buffer);

		if ((attribType.compare("POSITION") != 0) && (attribType.compare("NORMAL") != 0)
			&& (attribType.compare("TEXCOORD_0") != 0) && (attribType.compare("JOINTS_0") != 0)
			&& (attribType.compare("WEIGHTS_0") != 0) && (attribType.compare("COLOR_0") != 0)
			&& (attribType.compare("COLOR_1") != 0 && (attribType.compare("TEXCOORD_1") != 0)))
		{
			Logger::log(1, "%s: skipping attribute type %s\n", __FUNCTION__, attribType.c_str());
			continue;
		}

		Logger::log(1, "%s: data for %s uses accessor %i\n", __FUNCTION__, attribType.c_str(),
			accessorNum);

		if (attribType.compare("POSITION") == 0) {
			int numPositionEntries = accessor.count;
			Logger::log(1, "%s: loaded %i vertices from glTF file\n", __FUNCTION__,
				numPositionEntries);

			// Extract vertices
			auto positions = reinterpret_cast<const float*>(
				buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);

			m_vertices.resize(numPositionEntries);
			vertPositions.resize(numPositionEntries);
			tangents.resize(numPositionEntries);
			normals.resize(numPositionEntries);
			texCoords.resize(numPositionEntries);
			for (int i = 0; i < numPositionEntries; ++i)
			{
				m_vertices[i] = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
				vertPositions[i] = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
			}
		}
		else if (attribType.compare("NORMAL") == 0)
		{
			int numNormalEntries = (int)accessor.count;
			auto normalsData = reinterpret_cast<const float*>(
				buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);
			normals.resize(numNormalEntries);
			for (int i = 0; i < numNormalEntries; ++i)
			{
				normals[i] = glm::vec3(normalsData[i * 3 + 0], normalsData[i * 3 + 1], normalsData[i * 3 + 2]);
			}
		}
		else if (attribType.compare("TEXCOORD_0") == 0)
		{
			int numTexCoordEntries = (int)accessor.count;
			auto texCoordsData = reinterpret_cast<const float*>(
				buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);
			texCoords.resize(numTexCoordEntries);
			for (int i = 0; i < numTexCoordEntries; ++i)
			{
				texCoords[i] = glm::vec2(texCoordsData[i * 2 + 0], texCoordsData[i * 2 + 1]);
			}

			const tinygltf::Accessor& indexAccessor = m_model->accessors[primitives.indices];
			const tinygltf::BufferView& indBufferView = m_model->bufferViews[indexAccessor.bufferView];
			const tinygltf::Buffer& indbuffer = m_model->buffers[bufferView.buffer];

			const unsigned char* dataPtr = indbuffer.data.data() + indBufferView.byteOffset + indexAccessor.byteOffset;

			std::vector<unsigned int> indices(indexAccessor.count);

			if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			{
				auto buf = reinterpret_cast<const unsigned short*>(dataPtr);
				for (size_t i = 0; i < indexAccessor.count; ++i)
				{
					indices[i] = static_cast<unsigned int>(buf[i]);
				}
			}
			else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			{
				auto buf = reinterpret_cast<const unsigned int*>(dataPtr);
				for (size_t i = 0; i < indexAccessor.count; ++i)
				{
					indices[i] = buf[i];
				}
			}

			// Calculate tangents
			for (size_t i = 0; i < indices.size(); i += 3)
			{
				// Get vertex indices
				int idx0 = indices[i];
				int idx1 = indices[i + 1];
				int idx2 = indices[i + 2];

				// Get vertex positions and texture coordinates
				glm::vec3 v0 = vertPositions[idx0];
				glm::vec3 v1 = vertPositions[idx1];
				glm::vec3 v2 = vertPositions[idx2];

				glm::vec2 uv0 = texCoords[idx0];
				glm::vec2 uv1 = texCoords[idx1];
				glm::vec2 uv2 = texCoords[idx2];

				// Calculate edges and delta UVs
				glm::vec3 deltaPos1 = v1 - v0;
				glm::vec3 deltaPos2 = v2 - v0;

				glm::vec2 deltaUV1 = uv1 - uv0;
				glm::vec2 deltaUV2 = uv2 - uv0;

				// Tangent calculation
				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

				// Accumulate tangents
				tangents[idx0] += tangent;
				tangents[idx1] += tangent;
				tangents[idx2] += tangent;
			}

			// Normalize tangents
			for (size_t i = 0; i < tangents.size(); ++i)
			{
				tangents[i] = normalize(tangents[i]);
			}
		}

		{
			m_attribAccessors.at(m_attributes.at(attribType)) = accessorNum;
		}

		int dataSize = 1;
		switch (accessor.type)
		{
		case TINYGLTF_TYPE_SCALAR:
			dataSize = 1;
			break;
		case TINYGLTF_TYPE_VEC2:
			dataSize = 2;
			break;
		case TINYGLTF_TYPE_VEC3:
			dataSize = 3;
			break;
		case TINYGLTF_TYPE_VEC4:
			dataSize = 4;
			break;
		default:
			Logger::log(1, "%s error: accessor %i uses data size %i\n", __FUNCTION__,
				accessorNum, accessor.type);
			break;
		}

		GLuint dataType = GL_FLOAT;
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			dataType = GL_FLOAT;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			dataType = GL_UNSIGNED_SHORT;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			dataType = GL_UNSIGNED_BYTE;
			break;
		default:
			//Logger::log(1, "%s error: accessor %i uses unknown data type %i\n", __FUNCTION__,
		//		accessorNum, accessor.componentType);
			break;
		}

		{
			glGenBuffers(1, &m_vertexVbo.at(m_attributes.at(attribType)));
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo.at(m_attributes.at(attribType)));

			glVertexAttribPointer(m_attributes.at(attribType), dataSize, dataType, GL_FALSE,
				0, static_cast<void*>(nullptr));
			glEnableVertexAttribArray(m_attributes.at(attribType));

			if (attribType == "WEIGHTS_0")
			{
				GLuint tangentBuffer;
				glGenBuffers(1, &tangentBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
				glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);

				const GLuint tangentLocation = m_attributes["TANGENT"];

				glVertexAttribPointer(tangentLocation, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(nullptr));
				glEnableVertexAttribArray(tangentLocation);

				m_vertexVbo.push_back(tangentBuffer);
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
}

void GltfModel::CreateIndexBuffer()
{
	glGenBuffers(1, &m_indexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVbo);

}

void GltfModel::SetPosition(const glm::vec3& position)
{
	m_position = position;
	RecalculateModelMatrix();
}

void GltfModel::SetPosition(const elevate::Vector3& position)
{
	SetPosition(glm::vec3(
		static_cast<float>(position.x),
		static_cast<float>(position.y),
		static_cast<float>(position.z)
	));
}

void GltfModel::SetScale(float uniformScale)
{
	m_scale = glm::vec3(uniformScale);
	RecalculateModelMatrix();
}

void GltfModel::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
	RecalculateModelMatrix();
}

void GltfModel::SetScale(const elevate::Vector3& scale)
{
	SetScale(glm::vec3(
		static_cast<float>(scale.x),
		static_cast<float>(scale.y),
		static_cast<float>(scale.z)
	));
}

void GltfModel::SetOrientation(const glm::quat& orientation)
{
	m_orientation = glm::normalize(orientation);
	RecalculateModelMatrix();
}

const glm::mat4& GltfModel::GetModelMatrix() const
{
	return m_modelMatrix;
}

void GltfModel::RecalculateModelMatrix()
{
	glm::mat4 translationMatrix =
		glm::translate(glm::mat4(1.0f), m_position);

	glm::mat4 rotationMatrix =
		glm::mat4_cast(m_orientation);

	glm::mat4 scaleMatrix =
		glm::scale(glm::mat4(1.0f), m_scale);

	m_modelMatrix =
		translationMatrix *
		rotationMatrix *
		scaleMatrix;
}

void GltfModel::Draw(glm::mat4 view, glm::mat4 proj, glm::mat4 world)
{
	glDisable(GL_CULL_FACE);

	m_shader->use();

	std::vector<glm::mat4> matrixData;
	matrixData.push_back(view);
	matrixData.push_back(proj);
	matrixData.push_back(m_modelMatrix);
	mUniformBuffer.uploadUboData(matrixData, 0);

	for (size_t meshIndex = 0; meshIndex < meshData.size(); ++meshIndex)
	{
		for (size_t primIndex = 0; primIndex < meshData[meshIndex].primitives.size(); ++primIndex)
		{
			const GLTFPrimitive& prim = meshData[meshIndex].primitives[primIndex];

			glBindVertexArray(prim.vao);

			if (prim.indexBuffer)
			{
				glDrawElements(GL_TRIANGLES, prim.indexCount, prim.indexType, 0);
			}
			else
			{
				glDrawArrays(prim.mode, 0, prim.vertexCount);
			}

			glBindVertexArray(0);
		}
	}
}

void GltfModel::UploadVertexBuffers()
{
	for (int i = 0; i < 4; ++i)
	{
		const tinygltf::Accessor& accessor = m_model->accessors.at(i);
		const tinygltf::BufferView& bufferView = m_model->bufferViews.at(accessor.bufferView);
		const tinygltf::Buffer& buffer = m_model->buffers.at(bufferView.buffer);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo.at(i));
		glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void GltfModel::UploadIndexBuffer()
{
	/* buffer for vertex indices */
	const tinygltf::Primitive& primitives = m_model->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAccessor = m_model->accessors.at(primitives.indices);
	const tinygltf::BufferView& indexBufferView = m_model->bufferViews.at(indexAccessor.bufferView);
	const tinygltf::Buffer& indexBuffer = m_model->buffers.at(indexBufferView.buffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferView.byteLength,
		&indexBuffer.data.at(0) + indexBufferView.byteOffset, GL_STATIC_DRAW);
}