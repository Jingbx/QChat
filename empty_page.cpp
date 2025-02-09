#include "empty_page.h"
#include "ui_emptypage.h"

EmptyPage::EmptyPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmptyPage)
{
    ui->setupUi(this);
}

EmptyPage::~EmptyPage()
{
    delete ui;
}
