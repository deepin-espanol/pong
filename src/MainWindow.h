#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

class PongWidget;
class QLabel;

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    PongWidget *m_pongWidget;
    QLabel *m_scoreLabel;
};

#endif // MAINWINDOW_H
