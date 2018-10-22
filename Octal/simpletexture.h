#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H
#include "commonfunctions.h"
#include <QOpenGLFunctions_4_5_Core>
#include "octaltex.h"
#include "samplerfuncdefs.h"

class SimpleTexture
{
public:
	SimpleTexture(QOpenGLFunctions_4_5_Core *QTGL, glm::uvec3 _size, bool isOct = true);
	void createTexture();
	void createData();
	void bindTexture(size_t texNum = 0);

	void setPixel(glm::uvec3 p, glm::vec4 col);
	uint getIndex(glm::uvec3 p);

	void		addOctTex(size_t octIndex, OctalTex * tex);
	size_t		addOctTex(OctalTex * tex);
	size_t		getNextOctIndex();
	glm::uvec3	octIndexToPos(size_t octIndex);
	glm::uvec3	size() const { return _size; };

	void		fillWithSampler(samplerFunc sampler);

private:
	GLuint		myBuffer;
	glm::uvec3	_size,
				octSize;
	float		*myData = nullptr;
	bool		isOct;

	QOpenGLFunctions_4_5_Core *GL;
};

#endif // SIMPLETEXTURE_H
