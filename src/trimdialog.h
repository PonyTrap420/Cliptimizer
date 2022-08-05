#pragma once

#include <stdafx.h>
#include "ui_trimdialog.h"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget* parent = nullptr);
    ~Dialog();

    void SetMessage(QString messege);
    void SetFirstBtn(QString text);
    void SetSecondBtn(QString text);
    void SetCheckbox(QString text);

    void SetCheckBoxEnabled(bool enabled);

signals:
    void first_btn_pressed(bool checkbox);
    void second_btn_pressed(bool checkbox);

private slots:
    void firstBtn();
    void secondBtn();

private:
    Ui::Dialog* ui;
};
