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

private:
    // Wraps an OpenGL VertexArrayObject (VAO)
    QOpenGLVertexArrayObject	m_vao;
    // Vertex buffer (positions and colors, interleaved storage mode).
    QOpenGLBuffer				m_vertexBufferObject;
    // Index buffer to draw two rectangles
    QOpenGLBuffer				m_indexBufferObject;
    // Holds the compiled shader programs.
    QOpenGLShaderProgram		*m_program;

};
#endif // RECTANGLEWINDOW_H
