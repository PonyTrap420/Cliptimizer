#pragma once

#include "stdafx.h"

class VideoDisplay : public QGraphicsView
{
    Q_OBJECT

public:
    VideoDisplay(QWidget* parent = nullptr);
    ~VideoDisplay();

    void SetVideo(QVideoWidget* video);
protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QVideoWidget* video = nullptr;
};
