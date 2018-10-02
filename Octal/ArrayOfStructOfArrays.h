#pragma once

#include "../Framebufferabstract/commondefines.h"
#include <vector>
#include <list>
#include <algorithm>

//GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT
template <typename FT>	inline GLenum GetGLType() 		{ return GL_FLOAT; }
template <> 		inline GLenum GetGLType<char>() 	{ return GL_BYTE; }
template <> 		inline GLenum GetGLType<uint8_t>() 	{ return GL_UNSIGNED_BYTE; }
template <> 		inline GLenum GetGLType<short>() 	{ return GL_SHORT; }
template <> 		inline GLenum GetGLType<uint16_t>() 	{ return GL_UNSIGNED_SHORT; }
template <> 		inline GLenum GetGLType<int32_t>() 	{ return GL_INT; }
template <> 		inline GLenum GetGLType<uint32_t>() 	{ return GL_UNSIGNED_INT; }

template <typename FT>	inline uint32_t GetGLBytes()		{ return 4; }
template <> 		inline uint32_t GetGLBytes<char>() 	{ return 1; }
template <> 		inline uint32_t GetGLBytes<uint8_t>() 	{ return 1; }
template <> 		inline uint32_t GetGLBytes<short>() 	{ return 2; }
template <> 		inline uint32_t GetGLBytes<uint16_t>() 	{ return 2; }
template <> 		inline uint32_t GetGLBytes<int32_t>() 	{ return 4; }
template <> 		inline uint32_t GetGLBytes<uint32_t>() 	{ return 4; }

struct StructOfArrays
{
	StructOfArrays(GLuint NwArrayBuffer, GLuint NwVertexAttribLocation, GLuint NwNumFields) :
		ArrayBuffer(NwArrayBuffer),
		VertexAttribLocation(NwVertexAttribLocation),
		NumFields(NwNumFields) 
		{}
		
	GLuint ArrayBuffer, VertexAttribLocation, NumFields;
};

class ArrayOfStructOfArrays
{
public:
	ArrayOfStructOfArrays(int BUFSIZE) : MIJNBUFSIZE(BUFSIZE) 
	{
		glCreateVertexArrays(1, &VertexArray);
		glErrorToConsole("glCreateVertexArrays: ");
		//printf("VertexArray: %d\n", VertexArray);
	}
	
	template <typename TypeOfField> int AddBufferType(int NumFields, TypeOfField * Data, GLuint VertexAttribLocation, bool Normalize = false)
	{
		BindVertexArray();
		
		GLuint ArrayBuffer;
		glCreateBuffers(1, &ArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, MIJNBUFSIZE * NumFields * GetGLBytes<TypeOfField>(), (void *)Data, GL_DYNAMIC_DRAW);
		glErrorToConsole("glBufferData: ");
		//printf("BufferData toegewezen\n");
		
		glEnableVertexAttribArray(VertexAttribLocation);
		glErrorToConsole("AddBufferType glEnableVertexAttribArray: ");
		
		glVertexAttribPointer(VertexAttribLocation, NumFields, GetGLType<TypeOfField>(), Normalize, 0, 0);
		glErrorToConsole("glVertexAttribPointer: ");
		//printf("VertexAttribPointer gezet\n");

		
		SOAs.push_back(StructOfArrays(ArrayBuffer, VertexAttribLocation, NumFields));
		
		//glBindVertexArray(0);
		
		return SOAs.size() - 1;
	}
	
	template <typename TypeOfField> void BindNewData(int SOA, TypeOfField * Data, int Size = -1, bool Normalize = false)
	{
		if(Size < 0 || Size > MIJNBUFSIZE)
			Size = MIJNBUFSIZE;
		
		BindVertexArray();
		glErrorToConsole("BindNewData BindVertexArray: ");
		
		glBindBuffer(GL_ARRAY_BUFFER, SOAs[SOA].ArrayBuffer);
		glErrorToConsole("BindNewData glBindBuffer: ");
		
		
		glBufferData(GL_ARRAY_BUFFER, Size * SOAs[SOA].NumFields * GetGLBytes<TypeOfField>(), (void *)Data, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(SOAs[SOA].VertexAttribLocation);
		glVertexAttribPointer(SOAs[SOA].VertexAttribLocation, SOAs[SOA].NumFields, GetGLType<TypeOfField>(), Normalize, 0, 0);
		glErrorToConsole("BindNewData glBufferData: ");
		
		//glBindVertexArray(0);
	}
	
	void BindVertexArray()
	{
		glBindVertexArray(VertexArray);
		glErrorToConsole("ArrayOfStructOfArrays::BindVertexArray(): ");
	}
	
	const int MIJNBUFSIZE;
	
private:
	vector<StructOfArrays> SOAs;
	GLuint VertexArray; 
};
