#ifndef RECTANGLEWINDOW_H
#define RECTANGLEWINDOW_H

#include <QMainWindow>
#define GL_SILENCE_DEPRECATION
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QtOpenGL>
#include <QOpenGLFunctions>
#include <QtOpenGLWidgets/QOpenGLWidget>

class RectangleWindow : public QOpenGLWindow {
public:
    RectangleWindow();
    virtual ~RectangleWindow() Q_DECL_OVERRIDE;

    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    // updates the scene and requests a paint update
    void updateScene();

    void animateColorsTo(const std::vector<QColor> & toColors);

    // holds the vertex colors set on next call to updateScene()
    std::vector<QColor>			m_vertexColors;

private:
    // modifies the scene a bit and call updateScene() afterwards
    // when already in the final state, doesn't do anything
    void animate();

    // Wraps an OpenGL VertexArrayObject (VAO)
    QOpenGLVertexArrayObject	m_vao;
    // Vertex buffer (positions and colors, interleaved storage mode).
    QOpenGLBuffer				m_vertexBufferObject;
    // Index buffer to draw two rectangles
    QOpenGLBuffer				m_indexBufferObject;

    // Holds the compiled shader programs.
    QOpenGLShaderProgram		*m_program;

    std::vector<float>			m_vertexBufferData;

    // Stores the target colors that we animate towards
    std::vector<QColor>			m_toColors;
    // Stores the target colors that we animate from
    std::vector<QColor>			m_fromColors;
    // number of frames used for the animation
    unsigned int				m_frameCount;
};
#endif // RECTANGLEWINDOW_H
