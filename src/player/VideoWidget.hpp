//////////////////////////////////////////////////////////////////////
//!\file VideoWidget.hpp
//!\brief Subclassed QVideoWidget to catch user events.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef VIDEOWIDGET_HPP
#define VIDEOWIDGET_HPP

#include <QVideoWidget>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // VIDEOWIDGET_HPP
