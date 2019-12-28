#ifndef MODEL_H_
#define MODEL_H_

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <string>
#include <sstream>
#include<iostream>
#include<vector>
#include<string.h>
#include "Mesh.h"
using namespace std;

class Material  // 材质下面管理一系列面
{
public:
	string mtlName;

	double Ns; //反射指数
	double Ni; //折射指数
	double d; //渐隐指数

	string map_ka;
	string map_kd;
	string map_ks;

	int illum; //光照模型描述格式

	float Ka[3]; //环境反射
	float Kd[3]; //漫反射
	float Ks[3]; //镜反射
	float Ke[3];

	float Tf[3];//滤光透射率

	bool hasTexture;
	GLuint texture; // 材料对应的纹理

	Material() :hasTexture(false) {}
};

class Point3
{
public:
	double x, y, z;

	Point3(double x, double y, double z) :x(x), y(y), z(z) {}
};

class int3
{
public:
	int vertex, texture, normal;

	//int3() :vertex(0), texture(0), normal(0) {}

	int3(int x, int y, int z) :vertex(x), texture(y), normal(z) {}
};

template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

class Model{
public:
	Model(const char* path, const char* path2, bool gamma = false):gammaCorrection(gamma) {
		loadModel(path,path2);
	}

	void Draw(Shader shader);

private:
	// 临时数据 可以后期释放
	vector<Point3> vertexs;
	vector<Point3> textures;
	vector<Point3> normals;
	vector<int3> Faces;
	vector<int3> Indices;
	vector<Material*> m_mtls;
	int triangle;
	int rectangle;
	int total;
	int textureNum; //光照纹理数量
	void loadObjFile(string filename);
	void loadMtlFile(string filename);

	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	bool gammaCorrection;

	void loadModel(std::string path, std::string path2);
	Mesh processMesh();
	std::vector<Texture> loadMaterialtextures(std::string typeName);
};


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

//渲染
void Model::Draw(Shader shader) {
	for (int i = 0; i < meshes.size(); ++i)
		meshes[0].Draw(shader);
}

//加载模型
void Model::loadModel(std::string path,std::string path2) {
	loadObjFile(path);
	loadMtlFile(path2);

	directory = path.substr(0, path.find_last_of('\\'));
	meshes.push_back(processMesh());
	
}

//处理网格
Mesh Model::processMesh() {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> texturess;

	for (int i = 0; i < Faces.size(); ++i) {
		Vertex vertex;
		//处理顶点，法线和纹理坐标
		glm::vec3 vector;
		vector.x = vertexs[Faces[i].vertex-1].x;
		vector.y = vertexs[Faces[i].vertex - 1].y;
		vector.z = vertexs[Faces[i].vertex - 1].z;
		vertex.Position = vector;

		vector.x = normals[Faces[i].normal - 1].x;
		vector.y = normals[Faces[i].normal - 1].y;
		vector.z = normals[Faces[i].normal - 1].z;
		vertex.Normal = vector;


		glm::vec2 vec;
		vec.x = textures[Faces[i].texture - 1].x;
		vec.y = 1 - textures[Faces[i].texture - 1].y;
		vertex.TexCoords = vec;
		vertices.push_back(vertex);
	}
	int kk;
	kk = 1;
	//处理索引
	for (int i = 0; i < triangle+rectangle*2; ++i) {
		indices.push_back(Indices[i].vertex);
		indices.push_back(Indices[i].texture);
		indices.push_back(Indices[i].normal);
	}
	//处理材质
	if (m_mtls.size() > 0) {

		std::vector<Texture> diffuseMaps = loadMaterialtextures("texture_diffuse");
		texturess.insert(texturess.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialtextures("texture_specular");
		texturess.insert(texturess.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> ambientMaps = loadMaterialtextures("texture_ambient");
		texturess.insert(texturess.end(), ambientMaps.begin(), ambientMaps.end());	

	}
	
	return Mesh(vertices, indices, texturess);
}

//加载材质纹理，如果之前加载过就跳过
std::vector<Texture> Model::loadMaterialtextures(std::string typeName) {
	std::vector<Texture> textures;
	for (int i = 0; i < 1 ; ++i) {
		string str;
		if (typeName=="texture_diffuse") str = m_mtls[i]->map_kd;
		else if(typeName == "texture_specular")str = m_mtls[i]->map_ks;
		else str = m_mtls[i]->map_ka;
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); ++j) {
			if (std::strcmp(textures_loaded[j].path.c_str(), str.c_str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.c_str(), directory);
			texture.type = typeName;
			texture.path = str.c_str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}

	return textures;
}

//从文件读取纹理，并绑定
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma) {
	std::string filename = path;
	filename = directory + '\\' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "纹理加载失败，路径：" << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void  Model::loadObjFile(string filename)
{
	cout << "load obj" << endl;
	ifstream f;
	f.open(filename, ios::in);
	if (!f)
	{
		cout << "Error: OBJ File cannot be opened!";
		exit(-2);
	}

	int id = -1;   // 当前正在添加面的材质Id

	char buffer[300], word[300];

	while (f.getline(buffer, 300))
	{
		vector<string> strList(0);
		string str(buffer);
		string sTmp;
		istringstream istr(str);
		int length=0;
		while (!istr.eof())
		{
			istr >> sTmp; //get a word
			strList.push_back(sTmp);
			length = length + sTmp.length()+1;
			if (length == str.length())
			{
				break;
			}
		}

		if (strList[0] == "v")
		{
			Point3 tmp(stringToNum<double>(strList[1]), stringToNum<double>(strList[2]), stringToNum<double>(strList[3]));
			vertexs.push_back(tmp);
		}
		if (strList[0] == "vt")
		{
			Point3 tmp(stringToNum<double>(strList[1]), stringToNum<double>(strList[2]), 0.00000);
			textures.push_back(tmp);
		}
		if (strList[0] == "vn")
		{
			Point3 tmp(stringToNum<double>(strList[1]), stringToNum<double>(strList[2]), stringToNum<double>(strList[3]));
			normals.push_back(tmp);
		}

		if (strList[0] == "f")
		{
			vector<string> List;
			char *buff;
			char str[300] = "";
			int MM;
			for (int i = 1; i < strList.size(); i++) {

				for (MM = 0; MM < strList[i].length(); MM++)
					str[MM] = strList[i][MM];
				str[MM] = '\0';
				char * pch;

				pch = strtok_s(str, "/", &buff);
				while (pch != NULL)
				{
					List.push_back(pch);
					pch = strtok_s(NULL, "/", &buff);
				}
			}

			if (List.size() == 9) {
				for (int i = 0; i < 3; i++)
				{
					int3 tmp(stringToNum<int>(List[0+i*3]), stringToNum<int>(List[1+i*3]), stringToNum<int>(List[2+i*3]));
					Faces.push_back(tmp);
				}
				triangle++;
				int3 tmp(total , total+1 , total+2);
				Indices.push_back(tmp);
				total=total+3;
			}
			if (List.size() == 12) {
				for (int i = 0; i < 4; i++)
				{
					int3 tmp(stringToNum<int>(List[0 + i*3]), stringToNum<int>(List[1 + i*3]), stringToNum<int>(List[2 + i*3]));
					Faces.push_back(tmp);
				}
				rectangle++;
				int3 tmp(total, total + 1, total + 2);
				Indices.push_back(tmp);
				int3 tmp2(total, total + 2, total + 3);
				Indices.push_back(tmp2);
				total = total + 4;
			}
		}
		strList.clear();
	}
}

void Model::loadMtlFile(string filename)
{
	ifstream f;
	f.open(filename, ios::in);

	if (!f)
	{
		cout << "Error: Mtl File cannot be opened!";
		exit(-2);
	}

	int index = -1; // 当前材质的下标

	char buffer[300], word[300];

	while (f.getline(buffer, 300)) {
		vector<string> ls;
		string str(buffer);
		string sTmp;
		istringstream istr(str);
		while (!istr.eof())
		{
			istr >> sTmp; //get a word
			ls.push_back(sTmp);
		}

		if (ls[0] == "newmtl") {
			Material* tMtl = new Material();

			tMtl->mtlName = ls[1];
			m_mtls.push_back(tMtl);  // 获得一个新的材质
			cout << m_mtls.size() << endl;
			index = m_mtls.size() - 1;
		}

		if (ls[0] == "Ns")
		{
			int i;
			for (i = 0; i < ls[1].length(); i++)
				word[i] = ls[1][i];
			word[i] = '\0';
			m_mtls[index]->Ns = strtod(word, NULL);
		}
		if (ls[0] == "Ka")
		{
			m_mtls[index]->Ka[0] = stringToNum<double>(ls[1]);
			m_mtls[index]->Ka[1] = stringToNum<double>(ls[2]);
			m_mtls[index]->Ka[2] = stringToNum<double>(ls[3]);
		}
		if (ls[0] == "Kd")
		{
			m_mtls[index]->Kd[0] = stringToNum<double>(ls[1]);
			m_mtls[index]->Kd[1] = stringToNum<double>(ls[2]);
			m_mtls[index]->Kd[2] = stringToNum<double>(ls[3]);
		}
		if (ls[0] == "Ks")
		{
			m_mtls[index]->Ks[0] = stringToNum<double>(ls[1]);
			m_mtls[index]->Ks[1] = stringToNum<double>(ls[2]);
			m_mtls[index]->Ks[2] = stringToNum<double>(ls[3]);
		}
		if (ls[0] == "map_Ka")
		{
			string imgPath = ls[1];
			m_mtls[index]->hasTexture = true;
			m_mtls[index]->map_ka = imgPath;

		}
		if (ls[0] == "map_Kd")
		{
			string imgPath = ls[1];
			m_mtls[index]->hasTexture = true;
			m_mtls[index]->map_kd = imgPath;

		}
		
		if (ls[0] == "map_Ks")
		{
			string imgPath = ls[1];
			m_mtls[index]->hasTexture = true;
			m_mtls[index]->map_ks = imgPath;
		}
		
		ls.clear();
	}
}
#endif