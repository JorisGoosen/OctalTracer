#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QVector3D>
#include <QVector4D>
#include <stdlib.h>
#include <time.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <algorithm>

inline float randfloat(float min, float max) { return min+((((float)(rand()%RAND_MAX))/((float)RAND_MAX))* (max-min)); }

inline QVector4D randQVector4D(QVector4D min, QVector4D max)    { return QVector4D(randfloat(min.x(), max.x()), randfloat(min.y(), max.y()), randfloat(min.z(), max.z()), randfloat(min.w(), max.w())); }
inline QVector4D randQVector4D(float min, float max)            { return QVector4D(randfloat(min, max), randfloat(min, max), randfloat(min, max), randfloat(min, max)); }
inline QVector3D randQVector3D(float min, float max)            { return QVector3D(randfloat(min, max), randfloat(min, max), randfloat(min, max)); }

inline glm::vec3 randcolor()                            { return glm::vec3(randfloat(0.0f, 1.0f), randfloat(0.0f, 1.0f), randfloat(0.0f, 1.0f));	}
inline glm::vec4 randvec4(glm::vec4 min, glm::vec4 max) { return glm::vec4(randfloat(min.x, max.x), randfloat(min.y, max.y), randfloat(min.z, max.z), randfloat(min.w, max.w)); }
inline glm::vec4 randvec4(float min, float max)         { return glm::vec4(randfloat(min, max), randfloat(min, max), randfloat(min, max), randfloat(min, max)); }
inline glm::vec3 randvec3(float min, float max)         { return glm::vec3(randfloat(min, max), randfloat(min, max), randfloat(min, max)); }

inline void InitRandSeed() { srand(time_t(NULL)); }

inline float unisin(float ang) { return 0.5f + ( 0.5f * sinf(ang) ); }
inline float unicos(float ang) { return 0.5f + ( 0.5f * cosf(ang) ); }

inline glm::vec3 mixCol(glm::vec3 a, glm::vec3 b, float r) { r = std::min(1.0f, std::max(0.0f, r)); return ( a * (1.0f - r) ) + ( b * r ); }

inline glm::vec3 toGLM(const QVector3D & in) { return glm::vec3(in.x(), in.y(), in.z()); }
inline glm::vec4 toGLM(const QVector4D & in) { return glm::vec4(in.x(), in.y(), in.z(), in.w()); }

inline QVector3D toQVectorXD(const glm::vec3 & in) { return QVector3D(in.x, in.y, in.z); }
inline QVector4D toQVectorXD(const glm::vec4 & in) { return QVector4D(in.x, in.y, in.z, in.w); }

inline QVector3D toQVectorXD(const glm::uvec3 & in) { return QVector3D(in.x, in.y, in.z); }
inline QVector4D toQVectorXD(const glm::uvec4 & in) { return QVector4D(in.x, in.y, in.z, in.w); }



const float pi = 3.14159265359f;

#endif // COMMONFUNCTIONS_H
