#include "pch.h"
#include "MeshGeometry.h"
#include "Common/HelperFunctions.h"
#include <iostream>
#include <fstream>
#include <sstream>

MeshGeometry::MeshGeometry() {

	texfilename = L"Images/ground.png";

	//Set material properties
	mMeshMaterial.DiffuseColor = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f); //light color
	mMeshMaterial.SpecColor = XMVectorSet(1.0f, 1.0f, 1.0f, 8.0f); // Specular light colour
	mMeshMaterial.AmbientColor = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f); // ambient light color

	//Set shadow material properties
	mShadowMaterial.DiffuseColor = { 0.0f, 0.0f, 0.0f, 0.5f };
	mShadowMaterial.AmbientColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	mShadowMaterial.SpecColor = { 0.0f, 0.0f, 0.0f, 4.0f };

}

void MeshGeometry::Initialize(ComPtr<ID3D11Device> dev, std::string fname) {

	OBJINFO obj = ReadObjFileVertices(fname);

	VERTEX* modelVertices = std::data(obj.Vertices);
	vertArraySize = obj.Vertices.size();
		
	short* modelIndices = std::data(obj.Indices);
	indArraySize = obj.Indices.size();

	//Call method from model class
	InitIndexedGraphics(dev, modelVertices, vertArraySize, modelIndices, indArraySize);

}

OBJINFO MeshGeometry::ReadObjFileVertices(std::string filename) {

	wchar_t checkChar; //will store one char from file at a time.

	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;
	std::vector<VERTEX> modelVertices;
	std::vector<short> modelIndices;

	std::wstring facestring;
	std::wstring vertDef;

	std::vector<std::array<int, 3>> vertexInfoArray; //array to hold individual vertex information of format [vertex index, texture index, normal index]

	int modelVerticesIndex = 0;

	bool hasNormals = false;
	bool hasTexcoords = false;

	//declare input stream
	std::ifstream inFile(filename);
	
	if (inFile.good()) {


		//open file stream
		//inFile.open("test.txt");

		if (inFile.is_open()) {

			// find the length of the file
			int Length = (int)inFile.tellg();
			//std::wstring x = std::to_wstring(inFile.get());
			int text = 1;
		}


		while (!inFile.eof()) {
			//char buffer[200];
			//inFile.getline(buffer, 100);


			checkChar = inFile.get();

			switch (checkChar) {
			case '#':
				checkChar = inFile.get();
				while (checkChar != '\n') {
					checkChar = inFile.get();
				}
				break;
			case 'v':
				checkChar = inFile.get();
				if (checkChar == ' ') { //vertex position
					float vx, vy, vz;
					inFile >> vx >> vy >> vz;
					;
					vertices.push_back(XMFLOAT3(vx, vy, vz));
				}
				else if (checkChar == 'n') { //vertex normal
					float nx, ny, nz;
					inFile >> nx >> ny >> nz;

					normals.push_back(XMFLOAT3(nx, ny, nz));
				}
				else if (checkChar == 't') {
					float u, v, w;
					inFile >> u >> v >> w;

					texcoords.push_back(XMFLOAT2(u, 1-v)); //currently not using w
					hasTexcoords = true;
				}
				break;
			case 'g':
				checkChar = inFile.get();
				break;

			case 'f':

				int vertexcount;
				checkChar = inFile.get();

				facestring = L"";

				if (checkChar == ' ') {

					checkChar = inFile.get();
					vertexcount = 1;
					while (checkChar != '\n') {

						facestring += checkChar;
						checkChar = inFile.get();

						if (checkChar == ' ') {
							vertexcount++;
						}
					}
					//check for space at end of facestring
					if (facestring[facestring.length() - 1] == ' ') {
						vertexcount--;
					}

					std::wstringstream ss(facestring);

					if (facestring.length() > 0) {

						int firstIndex, lastIndex;
						for (int i = 0; i < vertexcount; i++) {
							ss >> vertDef;

							std::array<int, 3> vertInfo; // [vertex index, texture index, normal index]
							

							std::wstring delimiter = L"/";
							size_t pos = 0;
							std::wstring splitstring;

							std::vector<std::wstring> vertDefString;

							while ((pos = vertDef.find(delimiter)) != std::string::npos) { // positiion of delimeter is not at end of string.
								splitstring = vertDef.substr(0, pos); //get from 0 to position of delimeter.
								vertDefString.push_back(splitstring);
								vertDef.erase(0, pos + delimiter.length());
							}
							vertDefString.push_back(vertDef);

							vertInfo[0] = std::stoi(vertDefString[0]) - 1; //set vertex index as first char in vertDefString

							//vertex and texcoord defined i.e. #/#
							if (vertDefString[2] == L"") {
								vertInfo[1] = std::stoi(vertDefString[1]) - 1;
								vertInfo[2] = 0; //default normal
							}
							//vertex and normal defined i.e. #//#
							else if (vertDefString[1] == L"") {
								vertInfo[1] = 0; //default texture
								vertInfo[2] = std::stoi(vertDefString[2]) - 1;
							}
							//if just #
							else if ((vertDefString[1] == L"") && (vertDefString[2] == L"")) {
								vertInfo[1] = 0;
								vertInfo[2] = 0;
							}
							else { //if all defined i.e. #/#/#
								vertInfo[1] = std::stoi(vertDefString[1]) - 1;
								vertInfo[2] = std::stoi(vertDefString[2]) - 1;
							}

							if (i <= 2) {
								if (i == 0) {
									firstIndex = modelVerticesIndex;
								}
								lastIndex = modelVerticesIndex;

								modelIndices.push_back(modelVerticesIndex);

							}

							if (i > 2) {
								//new triangle
								modelIndices.push_back(firstIndex);
								modelIndices.push_back(lastIndex);
								
								lastIndex = modelVerticesIndex;

								modelIndices.push_back(modelVerticesIndex);
							}

							vertexInfoArray.push_back(vertInfo);
							modelVerticesIndex++;
						}

					}

				}

				break;
			case 'm':
				checkChar = inFile.get();
				while (checkChar != '\n') {
					checkChar = inFile.get();
				}
				break;
			case 'u':
				checkChar = inFile.get();
				while (checkChar != '\n') {
					checkChar = inFile.get();
				}
				break;
			case 's':
				checkChar = inFile.get();
				while (checkChar != '\n') {
					checkChar = inFile.get();
				}
				break;
			}
		}

		if (hasTexcoords == false) {
			for (size_t i = 0; i < vertices.size(); i++) {
				texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
			}
		}

		for (int i = 0; i < vertexInfoArray.size(); i++) {
			VERTEX newVertex = GetVertex(vertices, normals, texcoords, vertexInfoArray[i]); //get new vertex using info from file
			modelVertices.push_back(newVertex); //add to modelVertices
		}
	}
	OBJINFO obj;
	obj.Vertices = modelVertices;
	obj.Indices = modelIndices;

	return obj;
}

VERTEX MeshGeometry::GetVertex(std::vector<XMFLOAT3> vertices, std::vector<XMFLOAT3> normals, std::vector<XMFLOAT2> texcoords, std::array<int, 3> vertexInfo) {

	VERTEX newVertex;

	newVertex.X = vertices[vertexInfo[0]].x;
	newVertex.Y = vertices[vertexInfo[0]].y;
	newVertex.Z = vertices[vertexInfo[0]].z;

	newVertex.U = texcoords[vertexInfo[1]].x;
	newVertex.V = texcoords[vertexInfo[1]].y;

	newVertex.NX = normals[vertexInfo[2]].x;
	newVertex.NY = normals[vertexInfo[2]].y;
	newVertex.NZ = normals[vertexInfo[2]].z;

	return newVertex;

}

std::vector<VERTEX> MeshGeometry::CombineVerticesFromFile(std::vector<XMFLOAT3> vertices, std::vector<XMFLOAT3> normals, std::vector<XMFLOAT2> texcoords) {

	std::vector<VERTEX> modelVertices;
	for (size_t i = 0; i < normals.size(); i++) {

		VERTEX temp;

		temp.X = vertices[i].x;
		temp.Y = vertices[i].y;
		temp.Z = vertices[i].z;

		temp.NX = normals[i].x;
		temp.NY = normals[i].y;
		temp.NZ = normals[i].z;

		temp.U = texcoords[i].x;
		temp.V = texcoords[i].y;

		modelVertices.push_back(temp);
	}
	return modelVertices;
}

void MeshGeometry::DrawObject(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject) {

	//Assign material to constant buffer
	cbPerObject.gMaterial = mMeshMaterial;

	//Assign world matrix and rotation matrix to constant buffer
	cbPerObject.matWorld = GetWorldMatrix();
	cbPerObject.matRotate = GetRotationMatrix();

	//Call method from model class
	DrawIndexedGraphics(devcon, m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, cbPerObject, indArraySize);
}

void MeshGeometry::DrawShadow(ComPtr<ID3D11DeviceContext1> devcon, ComPtr<ID3D11Buffer> m_cbufferPerObject, CBUFFERPEROBJECT cbPerObject) {

	//Set material to that of a shadow
	cbPerObject.gMaterial = mShadowMaterial;

	//Assign world matrix and rotation to back buffer to be used by vertex shader.
	cbPerObject.matWorld = GetWorldMatrix();
	cbPerObject.matRotate = GetRotationMatrix();

	//Call method from model class
	DrawIndexedShadows(devcon, m_cbufferPerObject, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, cbPerObject, indArraySize);

}