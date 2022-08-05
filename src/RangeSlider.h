#ifndef RANGESLIDER_H
#define RANGESLIDER_H


#include "stdafx.h"

class RangeSlider : public QWidget
{
    Q_OBJECT

public:
    RangeSlider(QWidget* aParent = Q_NULLPTR);

    QSize minimumSizeHint() const override;

    int GetMinimun() const;
    void SetMinimum(int aMinimum);

    int GetMaximun() const;
    void SetMaximum(int aMaximum);

    int GetLowerValue() const;
    void SetLowerValue(int aLowerValue);

    int GetUpperValue() const;
    void SetUpperValue(int aUpperValue);

    int GetCurrentValue() const;
    void SetCurrentValue(int value);

    void SetRange(int aMinimum, int aMaximum);

protected:
    void paintEvent(QPaintEvent* aEvent) override;
    void mousePressEvent(QMouseEvent* aEvent) override;
    void mouseMoveEvent(QMouseEvent* aEvent) override;
    void mouseReleaseEvent(QMouseEvent* aEvent) override;
    void changeEvent(QEvent* aEvent) override;

    QRectF firstHandleRect() const;
    QRectF secondHandleRect() const;
    QRectF currentHandleRect() const;
    QRectF handleRect(int aValue) const;

signals:
    void lowerValueChanged(int aLowerValue);
    void upperValueChanged(int aUpperValue);
    void currentValueChanged(int value);
    void rangeChanged(int aMin, int aMax);

public slots:
    void setLowerValue(int aLowerValue);
    void setUpperValue(int aUpperValue);
    void setCurrentValue(int value);
    void setMinimum(int aMinimum);
    void setMaximum(int aMaximum);

private:
    Q_DISABLE_COPY(RangeSlider)
        float currentPercentage();
    int validWidth() const;

    int mMinimum;
    int mMaximum;
    int mLowerValue;
    int mUpperValue;
    int mCurrentValue;
    bool mFirstHandlePressed;
    bool mSecondHandlePressed;
    bool mViewHandlePressed;
    int mInterval;
    int mDelta;
    QColor mBackgroudColorEnabled;
    QColor mBackgroudColorDisabled;
    QColor mBackgroudColor;
};

#endif