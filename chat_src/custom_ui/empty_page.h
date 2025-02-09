#ifndef EMPTY_PAGE_H
#define EMPTY_PAGE_H

#include <QWidget>

namespace Ui {
class EmptyPage;
}

class EmptyPage : public QWidget
{
    Q_OBJECT

public:
    explicit EmptyPage(QWidget *parent = nullptr);
    ~EmptyPage();

private:
    Ui::EmptyPage *ui;
};

#endif // EMPTY_PAGE_H
