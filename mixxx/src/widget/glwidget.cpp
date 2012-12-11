#include "glwidget.h"

#include <cmath>
#include <QPainter>
#include <qgl.h>
#include <QPaintEvent>

class Background : public Renderer {
    void draw(QWidget* widget, QPainter* painter) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

class LinesGL : public Renderer {
  public:
    LinesGL() {
        m_offset = (double)((long long)this & 0xFF);
    }
    void draw(QWidget* widget, QPainter* painter) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        int width = widget->width();
        glOrtho(0, width, -255.0, 255.0, -10.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_MULTISAMPLE);

        glScalef(1.f,1.0,1.f);
        glColor3f(1.0, 1.0, 1.0);
        glLineWidth(1.0);
        //glEnable(GL_LINE_SMOOTH);
        //glHint(GL_LINE_SMOOTH, GL_NICEST);
        glBegin(GL_LINES); {
            double beatPosition = m_offset;
            while (beatPosition <= width) {
                // Both of these are no good.
                //double x = int(beatPosition + 0.5);
                //double x = floorf(beatPosition);
                double x = beatPosition;
                glVertex2f(x, 255.0);
                glVertex2f(x, -255.0);
                beatPosition += 19.314421930;
                //beatPosition += 20;
            }
            m_offset += 1.01034;
            //m_offset += 1;
            glEnd();
        }

        if (m_offset > width * 3 / 4) {
            m_offset = 0;
        }

    }
    double m_offset;
};

class LinesQt : public Renderer {
  public:
    LinesQt() {
        m_offset = (double)((long long)this & 0xFF);
    }
    void draw(QWidget* widget, QPainter* painter) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        int width = widget->width();
        int height = widget->height();

        QPen beatPen(Qt::white);
        beatPen.setWidth(1.5);

        painter->setPen(beatPen);

        {
            double beatPosition = m_offset;
            while (beatPosition <= width) {
                glVertex2f(beatPosition, 255.0);
                glVertex2f(beatPosition, -255.0);
                painter->drawLine(QPointF(beatPosition, 0.f),
                                  QPointF(beatPosition, height));
                beatPosition += 20;
            }
            m_offset += 1.0;
        }

        if (m_offset > width * 3 / 4) {
            m_offset = 0;
        }

        painter->restore();
    }
    double m_offset;
};


GLWidget::GLWidget(QWidget *parent)
        : QGLWidget(parent) {
    setAttribute(Qt::WA_PaintOutsidePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);

    m_renderers.push_back(new Background());
    m_renderers.push_back(new LinesGL());
    //m_renderers.push_back(new LinesQt());
}

void GLWidget::resizeEvent(QResizeEvent* pResizeEvent) {
}

void GLWidget::paintEvent(QPaintEvent* pPaintEvent) {

}

void GLWidget::render() {
    if (QGLContext::currentContext() != context()) {
        makeCurrent();
    }
    QPainter p(this);
    draw(this, &p);
}

void GLWidget::draw(QWidget* widget, QPainter* painter) {
    foreach (Renderer* renderer, m_renderers) {
        renderer->draw(widget, painter);
    }
}
