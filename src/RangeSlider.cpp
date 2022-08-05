
#include "stdafx.h"
#include "RangeSlider.h"


namespace
{

    const int scHandleSideLength = 20;
    const int scSliderBarHeight = 5;
    const int scLeftRightMargin = 20;

}


RangeSlider::RangeSlider(QWidget* aParent)
    : QWidget(aParent),
    mMinimum(0), 
    mMaximum(100),
    mLowerValue(0),
    mUpperValue(100),
    mCurrentValue(50),
    mFirstHandlePressed(false),
    mSecondHandlePressed(false),
    mViewHandlePressed(false),
    mInterval(mMaximum - mMinimum),
    mBackgroudColorEnabled(QColor(0xB2, 144, 226)),
    mBackgroudColorDisabled(Qt::darkMagenta),
    mBackgroudColor(mBackgroudColorEnabled)
{
    setMouseTracking(true);
}

void RangeSlider::paintEvent(QPaintEvent* aEvent)
{
    Q_UNUSED(aEvent);
    QPainter painter(this);

    QRectF backgroundRect = QRectF(scLeftRightMargin , (height() - scSliderBarHeight) / 2, width() - scLeftRightMargin * 2 - 35 , scSliderBarHeight);
    QPen pen(Qt::transparent, 0);
    painter.setPen(pen);
    //painter.setRenderHint(QPainter::Qt4CompatiblePainting);
    QBrush backgroundBrush(QColor(0x99, 0x99, 0x99));
    painter.setBrush(backgroundBrush);
    painter.drawRoundedRect(backgroundRect, 1, 1);

    pen.setColor(Qt::transparent);
    pen.setWidth(0);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush handleBrush(QColor(0xB2, 144, 226));
    painter.setBrush(handleBrush);

    QRectF leftHandleRect = firstHandleRect();

    QRectF rightHandleRect = secondHandleRect();
    painter.drawRoundedRect(rightHandleRect, 10, 10);

    painter.setRenderHint(QPainter::Antialiasing, false);
    QRectF selectedRect(backgroundRect);
    selectedRect.setLeft(leftHandleRect.right() + 0.5);
    selectedRect.setRight(rightHandleRect.left() - 0.5);

    QBrush selectedBrush(mBackgroudColor);
    painter.setBrush(selectedBrush);
    painter.drawRect(selectedRect);

    QBrush viewBrush(QColor(0xFF, 0xFF, 0xFF));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(viewBrush);
    QRectF viewHandleRect = currentHandleRect();
    painter.drawRoundedRect(viewHandleRect, 2, 2);

    viewBrush.setColor(QColor(0xB2, 144, 226));
    painter.setBrush(viewBrush);
    painter.drawRoundedRect(leftHandleRect, 10, 10);
}

QRectF RangeSlider::firstHandleRect() const
{
    float percentage = (mLowerValue - mMinimum) * 1.0 / mInterval;
    return handleRect(percentage * validWidth() + scLeftRightMargin - 20);
}

QRectF RangeSlider::secondHandleRect() const
{
    float percentage = (mUpperValue - mMinimum) * 1.0 / mInterval;
    return handleRect(percentage * validWidth() + scLeftRightMargin + 5);
}

QRectF RangeSlider::currentHandleRect() const
{
    float percentage = (mCurrentValue) * 1.0 / mInterval;
    return QRect(percentage * validWidth() + scLeftRightMargin, (height() - scHandleSideLength) / 2, 6, scHandleSideLength);
}

QRectF RangeSlider::handleRect(int aValue) const
{
    return QRect(aValue, (height() - scHandleSideLength) / 2, scHandleSideLength, scHandleSideLength);
}

void RangeSlider::mousePressEvent(QMouseEvent* aEvent)
{
    if (aEvent->buttons() & Qt::LeftButton)
    {
        this->setCursor(Qt::PointingHandCursor);
        mViewHandlePressed = currentHandleRect().contains(aEvent->pos());
        mSecondHandlePressed = !mViewHandlePressed && secondHandleRect().contains(aEvent->pos());
        mFirstHandlePressed = !mSecondHandlePressed && firstHandleRect().contains(aEvent->pos());

        if (mFirstHandlePressed)
        {
            mDelta = aEvent->pos().x() - (firstHandleRect().x() + scHandleSideLength / 2);
        }
        else if (mSecondHandlePressed)
        {
            mDelta = aEvent->pos().x() - (secondHandleRect().x() + scHandleSideLength / 2);
        }
        else if (mViewHandlePressed)
        {
            return;
        }
        else {
            SetCurrentValue((aEvent->pos().x() - 1 / 2 - scHandleSideLength) * 1.0 / validWidth() * mInterval + mMinimum);
            mViewHandlePressed = true;
        }
    }
}

void RangeSlider::mouseMoveEvent(QMouseEvent* aEvent)
{
    this->setCursor(Qt::PointingHandCursor);
    if (aEvent->buttons() & Qt::LeftButton)
    {
        if (mFirstHandlePressed)
        {
            if (aEvent->pos().x() - mDelta + scHandleSideLength / 2 <= secondHandleRect().x())
            {
                SetLowerValue((aEvent->pos().x() - mDelta  - scHandleSideLength / 2) * 1.0 / validWidth() * mInterval + mMinimum);
            }
            else
            {
                SetLowerValue(mUpperValue);
            }
        }
        else if (mSecondHandlePressed)
        {
            if (firstHandleRect().x() + scHandleSideLength * 1.5 <= aEvent->pos().x() - mDelta)
            {
                SetUpperValue((aEvent->pos().x() - mDelta  - scHandleSideLength / 2 - scHandleSideLength) * 1.0 / validWidth() * mInterval + mMinimum);
            }
            else
            {
                SetUpperValue(mLowerValue);
            }
        }
        else if (mViewHandlePressed)
        {
            SetCurrentValue((aEvent->pos().x() - 1  / 2 - scHandleSideLength) * 1.0 / validWidth() * mInterval + mMinimum);
        }
    }
}

void RangeSlider::mouseReleaseEvent(QMouseEvent* aEvent)
{
    Q_UNUSED(aEvent);

    mFirstHandlePressed = false;
    mSecondHandlePressed = false;
    mViewHandlePressed = false;
}

void RangeSlider::changeEvent(QEvent* aEvent)
{
    if (aEvent->type() == QEvent::EnabledChange)
    {
        if (isEnabled())
        {
            mBackgroudColor = mBackgroudColorEnabled;
        }
        else
        {
            mBackgroudColor = mBackgroudColorDisabled;
        }
        update();
    }
}

QSize RangeSlider::minimumSizeHint() const
{
    return QSize(scHandleSideLength * 2 + scLeftRightMargin * 2, scHandleSideLength);
}

int RangeSlider::GetMinimun() const
{
    return mMinimum;
}

void RangeSlider::SetMinimum(int aMinimum)
{
    setMinimum(aMinimum);
}

int RangeSlider::GetMaximun() const
{
    return mMaximum;
}

void RangeSlider::SetMaximum(int aMaximum)
{
    setMaximum(aMaximum);
}

int RangeSlider::GetLowerValue() const
{
    return mLowerValue;
}

void RangeSlider::SetLowerValue(int aLowerValue)
{
    if (aLowerValue > mMaximum)
    {
        aLowerValue = mMaximum;
    }

    if (aLowerValue < mMinimum)
    {
        aLowerValue = mMinimum;
    }

    mLowerValue = aLowerValue;
    emit lowerValueChanged(mLowerValue);

    update();
}

int RangeSlider::GetUpperValue() const
{
    return mUpperValue;
}

void RangeSlider::SetUpperValue(int aUpperValue)
{
    if (aUpperValue > mMaximum)
    {
        aUpperValue = mMaximum;
    }

    if (aUpperValue < mMinimum)
    {
        aUpperValue = mMinimum;
    }

    mUpperValue = aUpperValue;
    emit upperValueChanged(mUpperValue);

    update();
}

int RangeSlider::GetCurrentValue() const
{
    return mCurrentValue;
}

void RangeSlider::SetCurrentValue(int value)
{
    if (value > mMaximum)
    {
        value = mMaximum;
    }

    if (value < mMinimum)
    {
        value = mMinimum;
    }

    mCurrentValue = value;
    emit currentValueChanged(mCurrentValue);
    update();
}

void RangeSlider::setLowerValue(int aLowerValue)
{
    if (aLowerValue > mMaximum)
    {
        aLowerValue = mMaximum;
    }

    if (aLowerValue < mMinimum)
    {
        aLowerValue = mMinimum;
    }

    mLowerValue = aLowerValue;
    update();
}

void RangeSlider::setUpperValue(int aUpperValue)
{
    if (aUpperValue > mMaximum)
    {
        aUpperValue = mMaximum;
    }

    if (aUpperValue < mMinimum)
    {
        aUpperValue = mMinimum;
    }

    mUpperValue = aUpperValue;
    update();
}

void RangeSlider::setCurrentValue(int value) 
{
    mCurrentValue = value;
    update();
}

void RangeSlider::setMinimum(int aMinimum)
{
    if (aMinimum <= mMaximum)
    {
        mMinimum = aMinimum;
    }
    else
    {
        int oldMax = mMaximum;
        mMinimum = oldMax;
        mMaximum = aMinimum;
    }
    mInterval = mMaximum - mMinimum;
    update();

    setLowerValue(mMinimum);
    setUpperValue(mMaximum);

    emit rangeChanged(mMinimum, mMaximum);
}

void RangeSlider::setMaximum(int aMaximum)
{
    if (aMaximum >= mMinimum)
    {
        mMaximum = aMaximum;
    }
    else
    {
        int oldMin = mMinimum;
        mMaximum = oldMin;
        mMinimum = aMaximum;
    }
    mInterval = mMaximum - mMinimum;
    update();

    setLowerValue(mMinimum);
    setUpperValue(mMaximum);

    emit rangeChanged(mMinimum, mMaximum);
}

int RangeSlider::validWidth() const
{
    return width() - scLeftRightMargin * 2 - scHandleSideLength * 2;
}

void RangeSlider::SetRange(int aMinimum, int mMaximum)
{
    setMinimum(aMinimum);
    setMaximum(mMaximum);
}