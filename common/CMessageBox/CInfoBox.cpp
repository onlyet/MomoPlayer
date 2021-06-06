#include "CInfoBox.h"
#include "ui_CInfoBox.h"

CInfoBox::CInfoBox(QWidget *parent) :
    IMessageBox(parent),
    ui(new Ui::CInfoBox)
{
	setWindowFlag(Qt::WindowType::FramelessWindowHint, true);
	setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
}

CInfoBox::CInfoBox(const QString &text, QWidget *parent) :
    IMessageBox(parent),
    ui(new Ui::CInfoBox)
{
	setWindowFlag(Qt::WindowType::FramelessWindowHint, true);
	setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    setText(text);
}

CInfoBox::~CInfoBox()
{
    delete ui;
}

void CInfoBox::setText(const QString &text)
{
    ui->content->setText(text);
}
