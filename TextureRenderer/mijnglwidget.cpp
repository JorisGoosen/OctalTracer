#include "mijnglwidget.h"
#include "samplerfunctions.h"

float MijnGLWidget::getMilliseconds()
{
	return QTime::currentTime().msecsSinceStartOfDay();
}

void MijnGLWidget::initializeGL()
{
	millisecondsFPS = getMilliseconds();
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	if(!prepareShaderProgram(m_shader_stack,		":/Shaders/TexRen.vert", ":/Shaders/TexRen.frag" )) return;


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


	if (!m_shader_stack.bind() )
	{
		qWarning() << "Could not bind shader program to context";
		return;
	}

	// Enable the "vertex" attribute to bind it to our currently bound
	// vertex buffer.
	m_shader_stack.setAttributeBuffer( "vertex", GL_FLOAT, 0, 4 );
	m_shader_stack.enableAttributeArray( "vertex" );


    ZonPos = QVector3D(0.0f, 100.0f, 0.0f);
    m_projection.setToIdentity();
    m_projection.perspective(FovYScale, AspectRatio, NearDistance, FarDistance);

	srand(time(NULL));
	rand();

	glEnable(GL_TEXTURE_3D);

	MijnPerlin	= new Perlin(this);
	_bricks		= new SimpleTexture(this, glm::uvec3(128), false);

	TexGrootte	= toQVectorXD(_bricks->size());

	_bricks->fillWithSampler(perlinNoise);
}

void MijnGLWidget::resizeGL(int w, int h)
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
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
	_bricks->bindTexture();

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

    m_shader.setUniformValue("Translation", Translatie);
	m_shader.setUniformValue("RGBFragMultiplier", QVector3D(0.0f, 0.0f, 0.0f));
    //MijnPerlin->BindBuffers();
    //m_shader.setUniformValue("PerlinSize", PERLIN_NUM_GRADIENTS);


	m_shader.setUniformValue("TexSize", TexGrootte);
	static QVector3D TexInv= toQVectorXD(glm::vec3(1.0f) / glm::vec3(_bricks->size()));
	m_shader.setUniformValue("TexInv", TexInv);

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
