#include "trimdialog.h"

Dialog::Dialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Dialog())
{
	ui->setupUi(this);

	connect(ui->firstBtn, SIGNAL(pressed()), this, SLOT(firstBtn()));
	connect(ui->secondBtn, SIGNAL(pressed()), this, SLOT(secondBtn()));

}

void Dialog::SetMessage(QString message)
{
	ui->message->setText(message);
}

void Dialog::SetFirstBtn(QString text)
{
	ui->firstBtn->setText(text);
}

void Dialog::SetSecondBtn(QString text)
{
	ui->secondBtn->setText(text);
}

void Dialog::SetCheckbox(QString text)
{
	ui->checkBox->setText(text);
}

void Dialog::SetCheckBoxEnabled(bool enabled)
{
	ui->checkBox->setVisible(enabled);
}

void Dialog::firstBtn()
{
	emit first_btn_pressed(ui->checkBox->isChecked());
	accept();
}

void Dialog::secondBtn()
{
	emit second_btn_pressed(ui->checkBox->isChecked());
	accept();
}



Dialog::~Dialog()
{
	delete ui;
}