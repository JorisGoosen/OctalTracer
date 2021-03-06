#include "mijnglwidget.h"

float MijnGLWidget::getMilliseconds()
{
	return QTime::currentTime().msecsSinceStartOfDay();
}

void MijnGLWidget::initializeGL()
{
	millisecondsFPS = getMilliseconds();
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	if(!prepareShaderProgram(m_shader_stack,		":/Shaders/Octal.vert", ":/Shaders/Octal.frag" )) return;
	if(!prepareShaderProgram(m_shader_stackless,	":/Shaders/Octal.vert", ":/Shaders/octalstackless.frag" )) return;

    float points[] = {
         1,  1, 1, 1,
        -1,  1, 1, 1,
         1, -1, 1, 1,
        -1, -1, 1, 1
    };

    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );

    if (!m_vertexBuffer.bind()) {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_vertexBuffer.allocate( points, 4 * 4 * sizeof( float ) );

    // Bind the shader program so that we can associate variables from
    // our application to the shaders
	for(
		QOpenGLShaderProgram * m_shader = &m_shader_stackless;
		m_shader != NULL;
		m_shader = (m_shader == &m_shader_stackless ? &m_shader_stack : NULL)
	) {

		if (!m_shader->bind() )
		{
			qWarning() << "Could not bind shader program to context";
			return;
		}

		// Enable the "vertex" attribute to bind it to our currently bound
		// vertex buffer.
		m_shader->setAttributeBuffer( "vertex", GL_FLOAT, 0, 4 );
		m_shader->enableAttributeArray( "vertex" );
	}

    ZonPos = QVector3D(0.0f, 100.0f, 0.0f);
    m_projection.setToIdentity();
    m_projection.perspective(FovYScale, AspectRatio, NearDistance, FarDistance);

	srand(time(NULL));
	rand();

	MijnPerlin = new Perlin(this);
	MijnOctal = new Octal(this, MijnPerlin);
	//MijnOctal->printTree();
	MijnOctal->BindBuffer();
	MijnOctal->bindTexture();

	HoofdScherm::theHoofdScherm->setShaderInUse(use_shader_stack ? "STACK USED" : "STACKLESS USED");
}

void MijnGLWidget::resizeGL(int w, int h)
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
}

void MijnGLWidget::shaderButtonPressed()
{
	use_shader_stack = !use_shader_stack;
	HoofdScherm::theHoofdScherm->setShaderInUse(use_shader_stack ? "STACK USED" : "STACKLESS USED");
}

void MijnGLWidget::paintGL()
{
	if(!hasFocus())		setFocus();
	framesFPS++;
	float curTime = getMilliseconds();
	float diff = (curTime  - millisecondsFPS) / 1000;
//	std::cout << "millisecondsFPS: " << millisecondsFPS << " curTime: "<< curTime << std::endl << std::flush;

	if(diff > 2)
	{
		millisecondsFPS = curTime;

		float FPS = framesFPS / diff;

		HoofdScherm::theHoofdScherm->setFPS(FPS);
		//std::cout << "FPS: " << FPS << std::endl << std::flush;
		framesFPS = 0;
	}

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_vertexBuffer.bind();

	QOpenGLShaderProgram & m_shader = get_current_shader();

    m_modelview.setToIdentity();
    //m_modelview.translate(Translatie);
    m_modelview.rotate(Rotate, QVector3D(0, 1, 0));
    m_modelview.rotate(RotateVert, QVector3D(1, 0, 0));


    m_shader.bind();
    m_shader.setUniformValue("Projectie", m_projection);
    m_shader.setUniformValue("ModelView", m_modelview);
    m_shader.setUniformValue("fov_y_scale", FovYScale);
    m_shader.setUniformValue("aspect", AspectRatio);
    m_shader.setUniformValue("RPos", ZonPos);
    m_shader.setUniformValue("GPos", ZonPos);
	m_shader.setUniformValue("BPos", ZonPos);

    m_shader.setUniformValue("Translation", Translatie);
    m_shader.setUniformValue( "RGBFragMultiplier", QVector3D(0.0f, 0.0f, 0.0f));
    //MijnPerlin->BindBuffers();
    //m_shader.setUniformValue("PerlinSize", PERLIN_NUM_GRADIENTS);

	m_shader.setUniformValue("OCTAL_MAX", OCTAL_MAX);
	m_shader.setUniformValue("MAXDIEPTE", MijnOctal->MaxDepth);

	m_shader.setUniformValue("TEXDIM", int(OctalTex::Dim));




    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}

bool MijnGLWidget::prepareShaderProgram(QOpenGLShaderProgram & m_shader, const QString& vertexShaderPath, const QString& fragmentShaderPath )
{

    // First we load and compile the vertex shader…
	bool result = m_shader.addShaderFromSourceFile( QOpenGLShader::Vertex, vertexShaderPath );
    if ( !result )
		std::cout << m_shader.log().toStdString() << std::endl;;

    // …now the fragment shader…
	result = m_shader.addShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShaderPath );
    if ( !result )
		std::cout << m_shader.log().toStdString() << std::endl;

    // …and finally we link them to resolve any references.
	result = m_shader.link();
    if ( !result )
		std::cout << "Could not link shader program:" << m_shader.log().toStdString() << std::endl;

    return result;
}

void MijnGLWidget::keyPressEvent( QKeyEvent* e )
{

    switch ( e->key() )
    {
    case Qt::Key_Escape:
        HoofdScherm::Instance()->close();
        break;


    case Qt::Key_W:
        Translatie += QVector3D(0, 0, TranslatieStap);
        break;

    case Qt::Key_S:
        Translatie -= QVector3D(0, 0, TranslatieStap);
        break;

    case Qt::Key_Q:
        Translatie += QVector3D(0, TranslatieStap, 0);
        break;

    case Qt::Key_E:
        Translatie -= QVector3D(0, TranslatieStap, 0);
        break;

    case Qt::Key_A:
        Translatie -= QVector3D(TranslatieStap, 0, 0);
        break;

    case Qt::Key_D:
        Translatie += QVector3D(TranslatieStap, 0, 0);
        break;

    case Qt::Key_Up:
        RotateVert -= RotatieStap * FPI;
        break;

    case Qt::Key_Down:
        RotateVert += RotatieStap * FPI;
        break;


    case Qt::Key_Left:
        Rotate -= RotatieStap * FPI;
        break;

    case Qt::Key_Right:
        Rotate += RotatieStap * FPI;
        break;

    default:
        QOpenGLWidget::keyPressEvent( e );
    }

    update();
}
