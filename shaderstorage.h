#pragma once
#include <QOpenGLFunctions_4_5_Core>
#include "commonfunctions.h"

#define MAXRETRYDEPTHSCHRIJF 10

template <class DataStructuur> class shaderstorage
{
public:
                    shaderstorage(int HoeGroot, QOpenGLFunctions_4_5_Core *QTGL);
	void			BindBufferStorage(GLuint Index);
	void			SchrijfWeg(int StartIndex, DataStructuur *Blok, int BlokGrootte, int Diepte=0);
	void			SchrijfWeg();
	DataStructuur*	LeesUit();
	int				Lengte()				{ return Grootte; }
	void			SluitInleesRange();
	DataStructuur& 	operator[](int idx)		{ return Data[idx]; }
	
private:
	DataStructuur 	*Data;
	int				Grootte;
	GLuint			BufferIndex,
					VertexArrayIndex;
	bool			HasVertexArrayFeature = false,
					ReadingCurrently = false;

    QOpenGLFunctions_4_5_Core *GL;
};

template <class DataStructuur> shaderstorage<DataStructuur>::shaderstorage(int HoeGroot, QOpenGLFunctions_4_5_Core *QTGL)
{
    GL = QTGL;
	//buffers enzo aanmaken
	Grootte			= HoeGroot;
	Data			= new DataStructuur[Grootte];
    GL->glGenBuffers(1,	&BufferIndex);
	
    GL->glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferIndex);
    GL->glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DataStructuur)*Grootte, NULL, GL_STATIC_DRAW);
}

template <class DataStructuur> void shaderstorage<DataStructuur>::BindBufferStorage(GLuint Index)
{
    GL->glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferIndex);
    GL->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Index, BufferIndex);
}

template <class DataStructuur> void shaderstorage<DataStructuur>::SchrijfWeg()
{
	SchrijfWeg(0, Data, Grootte, 0);
}

template <class DataStructuur> void shaderstorage<DataStructuur>::SchrijfWeg(int StartIndex, DataStructuur *Blok, int BlokGrootte, int Diepte)
{
	if(StartIndex + BlokGrootte > Lengte() || StartIndex < 0 || BlokGrootte < 1)
	{
		printf("StartIndex %d of BlokGrootte %d is fucking bullshit!\n", StartIndex, BlokGrootte);
		return;
	}
	
	//Buffer binden dan data mappen ,vullen en unmappen
    GL->glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferIndex);
    DataStructuur *GpuData = (DataStructuur *) GL->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, StartIndex, sizeof(DataStructuur)*BlokGrootte, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    //glErrorToConsole("ShaderStorage heeft net glMapBufferRange gedaan!");

	memcpy(GpuData, Blok, sizeof(DataStructuur)*BlokGrootte);
		
    if(GL->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER) != GL_TRUE)
	{
		printf("De gpu-data is corrupt geraakt aan de andere kant! Binnen: SchrijfWeg Diepte %d..\n", Diepte);
		if(Diepte < MAXRETRYDEPTHSCHRIJF)
			SchrijfWeg(StartIndex, Blok, BlokGrootte, Diepte+1);
	}		
}

template <class DataStructuur> DataStructuur*	shaderstorage<DataStructuur>::LeesUit()
{
    GL->glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferIndex);
    DataStructuur *GpuData = (DataStructuur *) GL->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(DataStructuur)*Grootte, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	memcpy(Data, GpuData, sizeof(DataStructuur)*Lengte());
	
    //glErrorToConsole("shaderstorage<DataStructuur>::LeesUit()");
	
    if(GL->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER) != GL_TRUE)
		printf("shaderstorage<DataStructuur>::LeesUit() says: De gpu-data is corrupt geraakt aan de andere kant!\n");
		
	return Data;	
}
