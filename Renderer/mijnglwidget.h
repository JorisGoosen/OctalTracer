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
#include <QTime>

const long double PI = 3.141592653589793238L;
const float FPI = 3.141592653589793238f;
const float TranslatieStap = 0.1f, RotatieStap = 0.5f;

class MijnGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
public:
    MijnGLWidget(QWidget *parent) : QOpenGLWidget(parent)
    {
		Translatie = QVector3D(0, 0, -8);
    }

    void keyPressEvent( QKeyEvent* e );
	inline QOpenGLShaderProgram & get_current_shader() { return use_shader_stack ? m_shader_stack : m_shader_stackless; }

	float getMilliseconds();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

public slots:
	void shaderButtonPressed();

private:
    QMatrix4x4 m_projection, m_modelview;

	bool prepareShaderProgram(QOpenGLShaderProgram & shader, const QString& vertexShaderPath, const QString& fragmentShaderPath );

	QOpenGLShaderProgram m_shader_stack, m_shader_stackless;
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
	bool		use_shader_stack = false;
	float		millisecondsFPS=0;
	float		framesFPS=0;
};

#endif // MIJNGLWIDGET_H
