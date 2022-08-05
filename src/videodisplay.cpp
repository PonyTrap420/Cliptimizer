#include "videodisplay.h"

VideoDisplay::VideoDisplay(QWidget* parent)
{
}

void VideoDisplay::SetVideo(QVideoWidget* video) {
	this->video = video;
}
void VideoDisplay::resizeEvent(QResizeEvent* event) {

	if (video != nullptr) {
		video->resize(size());
	}

	QGraphicsView::resizeEvent(event);
}

VideoDisplay::~VideoDisplay()
{}
