#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QPainter>
#include <QList>

class Renderer {
  public:
    virtual void draw(QWidget* widget, QPainter* painter) = 0;
};

class GLWidget : public QGLWidget, public Renderer
{
    Q_OBJECT
  public:
    explicit GLWidget(QWidget *parent = 0);
    virtual void paintEvent(QPaintEvent* pEvent);
    virtual void draw(QWidget* widget, QPainter* painter);

  private:
    QList<Renderer*> m_renderers;
};

#endif // GLWIDGET_H
