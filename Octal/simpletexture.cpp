#include "simpletexture.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

SimpleTexture::SimpleTexture(QOpenGLFunctions_4_5_Core *QTGL, glm::uvec3 newsize, bool isOct) : GL(QTGL),  _size(newsize * uint(isOct ? OctalTex::Dim : 1)), octSize(newsize), isOct(isOct)
{
	GL->glGenTextures(1, &myBuffer);
	GL->glBindTexture(GL_TEXTURE_3D, myBuffer);
	createData();
}

void SimpleTexture::createTexture()
{
	assert(myData != nullptr);

	bindTexture(0);
	glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
	GL->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	GL->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	GL->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	GL->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GL->glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, _size.x, _size.y, _size.z, 0, GL_RGBA, GL_FLOAT, myData);

	//delete[] myData;
	//myData = nullptr;
}

void SimpleTexture::createData()
{
	myData = new float[4 * _size.x * _size.y * _size.z];
}

void SimpleTexture::setPixel(glm::uvec3 p, glm::vec4 col)
{
	if(myData != nullptr)
		std::memcpy(myData + getIndex(p), glm::value_ptr(col), sizeof(float) * 4);
}

void SimpleTexture::bindTexture(size_t texNum)
{
	GL->glActiveTexture(GL_TEXTURE0 + texNum);
	GL->glBindTexture(GL_TEXTURE_3D, myBuffer);
}

size_t SimpleTexture::getNextOctIndex()
{
	if(!isOct)
		throw std::runtime_error("This is not an oct texture!");

	static size_t idx = 0;

	if(idx > octSize.x * octSize.y * octSize.z)
		throw std::runtime_error("Too big octIndex for SimpleTexture!");

	return idx++;
}

uint SimpleTexture::getIndex(glm::uvec3 p)
{
	//std::cout << "SimpleTexture::getIndex received pos ("<<p.x <<", "<<p.y<<", " << p.z <<") for size (" << size.x << ", " << size.y << ", " << size.z <<")" << std::endl;
	if(p.x >= _size.x || p.y >= _size.y || p.z >= _size.z)
	{
		std::cout << "SimpleTexture::getIndex received wrong pos ("<<p.x <<", "<<p.y<<", " << p.z <<") for size (" << _size.x << ", " << _size.y << ", " << _size.z <<")" << std::endl;

		throw std::runtime_error("Too big pos for SimpleTexture!");
	}

	return (p.x + (_size.x * p.y) + (_size.x * _size.y * p.z)) * 4;
}

glm::uvec3 SimpleTexture::octIndexToPos(size_t octIndex)
{
	if(!isOct)
		throw std::runtime_error("This is not an oct texture!");

	glm::uvec3 p;
	p.x	= octIndex % octSize.x;
	p.y = ((octIndex - p.x) / octSize.x) % octSize.y;
	p.z = (((octIndex - p.x) / octSize.x) - p.y) / octSize.y;

	p *= OctalTex::Dim;

	return p;
}

size_t SimpleTexture::addOctTex(OctalTex * tex)
{
	size_t idx = getNextOctIndex();
	addOctTex(idx, tex);
	return idx;
}

void SimpleTexture::addOctTex(size_t octIndex, OctalTex * tex)
{
	glm::uvec3 p = octIndexToPos(octIndex);

	glm::uvec3 offset;
	for(offset.x = 0; offset.x < OctalTex::Dim; offset.x++)
		for(offset.y = 0; offset.y < OctalTex::Dim; offset.y++)
			for(offset.z = 0; offset.z < OctalTex::Dim; offset.z++)
				setPixel(p + offset, tex->getPixel(offset));
}

void SimpleTexture::fillWithSampler(samplerFunc sampler)
{
	if(isOct)
		throw std::runtime_error("This is an oct texture!");

	static glm::vec3 fsize(_size), halfSize(fsize * 0.5f);

	glm::uvec3 p;
	for(p.x = 0; p.x < _size.x; p.x++)
		for(p.y = 0; p.y < _size.y; p.y++)
			for(p.z = 0; p.z < _size.z; p.z++)
				setPixel(p, sampler(glm::vec3(-1.0f) + (glm::vec3(p) / halfSize)));

	createTexture();
}
