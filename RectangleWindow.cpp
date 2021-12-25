#include "RectangleWindow.h"

#include <QDebug>

RectangleWindow::RectangleWindow() :
    m_vertexColors{ 		QColor("#f6a509"),
                            QColor("#cb2dde"),
                            QColor("#0eeed1"),
                            QColor("#068918") },
    m_program(nullptr),
    m_frameCount(5000)
{
}


RectangleWindow::~RectangleWindow() {
    // resource cleanup

    // since we release resources related to an OpenGL context,
    // we make this context current before cleaning up our resources
    makeCurrent();

    // resource cleanup
    m_vao.destroy();
    m_vertexBufferObject.destroy();
    m_indexBufferObject.destroy();
    delete m_program;
}


void RectangleWindow::initializeGL() {
    // this function is called once, when the window is first shown, i.e. when
    // the the window content is first rendered

    // build and compile our shader program
    // ------------------------------------

    m_program = new QOpenGLShaderProgram();

    // read the shader programs from the resource
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "/Users/battleranger/Documents/C++-Projekte/openGl/SimpleVertexShader.vert"))
        qDebug() << "Vertex shader errors:\n" << m_program->log();

    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "/Users/battleranger/Documents/C++-Projekte/openGl/SimpleFragmentShader.frag"))
        qDebug() << "Fragment shader errors:\n" << m_program->log();

    if (!m_program->link())
        qDebug() << "Shader linker errors:\n" << m_program->log();


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices[] = {
         0.8f,  0.8f, 0.0f,  // top right
         0.8f, -0.8f, 0.0f,  // bottom right
        -0.8f, -0.8f, 0.0f,  // bottom left
        -0.8f,  0.8f, 0.0f   // top left
    };

    // resize buffer for 2 interleaved attributes: position and color, 4 vertices, 3 floats each
    m_vertexBufferData.resize(2*4*3);
    // create new data buffer - the following memory copy stuff should
    // be placed in some convenience class in later tutorials
    // copy data in interleaved mode with pattern p0c0|p1c1|p2c2|p3c3
    float * buf = m_vertexBufferData.data();
    for (int v=0; v<4; ++v, buf += 6) {
        // coordinates
        buf[0] = vertices[3*v];
        buf[1] = vertices[3*v+1];
        buf[2] = vertices[3*v+2];
        // colors
        buf[3] = m_vertexColors[v].redF();
        buf[4] = m_vertexColors[v].greenF();
        buf[5] = m_vertexColors[v].blueF();
    }

    // create a new buffer for the vertices and colors, interleaved storage
    m_vertexBufferObject = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vertexBufferObject.create();
    m_vertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBufferObject.bind();
    // now copy buffer data over: first argument pointer to data, second argument: size in bytes
    m_vertexBufferObject.allocate(m_vertexBufferData.data(), m_vertexBufferData.size()*sizeof(float) );

    // create and bind Vertex Array Object - must be bound *before* the element buffer is bound,
    // because the VAO remembers and manages element buffers as well
    m_vao.create();
    m_vao.bind();

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // create a new buffer for the indexes
    m_indexBufferObject = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer); // Mind: use 'IndexBuffer' here
    m_indexBufferObject.create();
    m_indexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBufferObject.bind();
    m_indexBufferObject.allocate(indices, sizeof(indices) );

    // stride = number of bytes for one vertex (with all its attributes) = 3+3 floats = 6*4 = 24 Bytes
    int stride = 6*sizeof(float);

    // layout location 0 - vec3 with coordinates
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    // layout location 1 - vec3 with colors
    m_program->enableAttributeArray(1);
    int colorOffset = 3*sizeof(float);
    m_program->setAttributeBuffer(1, GL_FLOAT, colorOffset, 3, stride);
}


void RectangleWindow::paintGL() {
    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // use our shader program
    m_program->bind();
    // bind the vertex array object, which in turn binds the vertex buffer object and
    // sets the attribute buffer in the OpenGL context
    m_vao.bind();
    // For old Intel drivers you may need to explicitely re-bind the index buffer, because
    // these drivers do not remember the binding-state of the index/element-buffer in the VAO
    //	m_indexBufferObject.bind();

    // now draw the two triangles via index drawing
    // - GL_TRIANGLES - draw individual triangles via elements
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    // finally release VAO again (not really necessary, just for completeness)
    m_vao.release();

    animate();
}


void RectangleWindow::updateScene() {
    // for now we only update colors

    // first update our vertex buffer memory, but only those locations that are actually changed
    float * buf = m_vertexBufferData.data();
    for (int v=0; v<4; ++v, buf += 6) {
        // colors
        buf[3] = m_vertexColors[v].redF();
        buf[4] = m_vertexColors[v].greenF();
        buf[5] = m_vertexColors[v].blueF();
    }

    // make this OpenGL context current
    makeCurrent();

    // bind the vertex buffer
    m_vertexBufferObject.bind();
    // now copy buffer data over: first argument pointer to data, second argument: size in bytes
    m_vertexBufferObject.allocate(m_vertexBufferData.data(), m_vertexBufferData.size()*sizeof(float) );

    // and request an update
    update();
}


void RectangleWindow::animateColorsTo(const std::vector<QColor> & toColors) {
    // current colors are set to "fromColors", toColors are store in m_toColors and
    // animation counter is reset

    m_fromColors = m_vertexColors;
    m_toColors = toColors;
    m_frameCount = 0;

    animate();
}


void RectangleWindow::animate() {
    const unsigned int FRAMECOUNT = 120;
    // if already at framecount end, stop
    if (++m_frameCount > FRAMECOUNT)
        return; // this will also stop the frame rendering

    // update the colors
    double alpha = double(m_frameCount)/FRAMECOUNT;

    // linear blending in HSV space will probably look "interesting", but it's simple
    for (unsigned int i=0; i<m_vertexColors.size(); ++i) {
        float fromH, fromS, fromV;
        m_fromColors[i].getHsvF(&fromH, &fromS, &fromV);
        float toH, toS, toV;
        m_toColors[i].getHsvF(&toH, &toS, &toV);

        m_vertexColors[i] = QColor::fromHsvF(toH*alpha + fromH*(1-alpha),
                                              toS*alpha + fromS*(1-alpha),
                                              toV*alpha + fromV*(1-alpha));
    }

    updateScene();
}
