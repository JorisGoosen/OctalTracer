#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QVector3D>
#include <QVector4D>
#include <stdlib.h>
#include <time.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

inline float randfloat(float min, float max) { return min+((((float)(rand()%RAND_MAX))/((float)RAND_MAX))* (max-min)); }

inline QVector4D randQVector4D(QVector4D min, QVector4D max)    { return QVector4D(randfloat(min.x(), max.x()), randfloat(min.y(), max.y()), randfloat(min.z(), max.z()), randfloat(min.w(), max.w())); }
inline QVector4D randQVector4D(float min, float max)            { return QVector4D(randfloat(min, max), randfloat(min, max), randfloat(min, max), randfloat(min, max)); }
inline QVector3D randQVector3D(float min, float max)            { return QVector3D(randfloat(min, max), randfloat(min, max), randfloat(min, max)); }

inline glm::vec3 randcolor()                            { return glm::vec3(randfloat(0.0f, 1.0f), randfloat(0.0f, 1.0f), randfloat(0.0f, 1.0f));	}
inline glm::vec4 randvec4(glm::vec4 min, glm::vec4 max) { return glm::vec4(randfloat(min.x, max.x), randfloat(min.y, max.y), randfloat(min.z, max.z), randfloat(min.w, max.w)); }
inline glm::vec4 randvec4(float min, float max)         { return glm::vec4(randfloat(min, max), randfloat(min, max), randfloat(min, max), randfloat(min, max)); }
inline glm::vec3 randvec3(float min, float max)         { return glm::vec3(randfloat(min, max), randfloat(min, max), randfloat(min, max)); }

inline void InitRandSeed() { srand(time_t(NULL)); }

#endif // COMMONFUNCTIONS_H
