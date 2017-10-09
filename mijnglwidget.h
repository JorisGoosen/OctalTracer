#ifndef MIJNGLWIDGET_H
#define MIJNGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "hoofdscherm.h"
#include "Perlin.h"
#include "octal.h"

const long double PI = 3.141592653589793238L;
const float FPI = 3.141592653589793238f;
const float TranslatieStap = 1.0f;

class MijnGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
public:
    MijnGLWidget(QWidget *parent) : QOpenGLWidget(parent)
    {
        Translatie = QVector3D(0, 0, -4);
    }

    void keyPressEvent( QKeyEvent* e );

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();


private:
    QMatrix4x4 m_projection, m_modelview;

    bool prepareShaderProgram( const QString& vertexShaderPath,
    const QString& fragmentShaderPath );

    QOpenGLShaderProgram m_shader;
    QOpenGLBuffer m_vertexBuffer;
    QVector3D ZonPos;
    Perlin      *MijnPerlin = NULL;

    float		FovYScale			= FPI * 0.25f,
                AspectRatio			= 16.0f / 9.0f,
                NearDistance		= 0.001f,
                FarDistance			= 100.0f;
    QVector3D   Translatie;
    float       Rotate = 0.0f, RotateVert = 0.0f;
    Octal       *MijnOctal = NULL;
};

#endif // MIJNGLWIDGET_H
