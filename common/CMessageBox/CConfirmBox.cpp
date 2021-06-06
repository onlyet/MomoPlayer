#include "CConfirmBox.h"
#include "ui_CConfirmBox.h"

CConfirmBox::CConfirmBox(QWidget *parent) :
	IMessageBox(parent),
	//QDialog(parent),
    ui(new Ui::CConfirmBox)
{
	setWindowFlag(Qt::WindowType::FramelessWindowHint, true);
	setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
}

CConfirmBox::CConfirmBox(const QString &text, QWidget *parent) :
	IMessageBox(parent),
	//QDialog(parent),
    ui(new Ui::CConfirmBox)
{
	setWindowFlag(Qt::WindowType::FramelessWindowHint, true);
	setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    setText(text);
}

CConfirmBox::~CConfirmBox()
{
    delete ui;
}

void CConfirmBox::setText(const QString &text)
{
    ui->content->setText(text);
}
