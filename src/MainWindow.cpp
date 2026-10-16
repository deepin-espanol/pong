#include "MainWindow.h"
#include "PongWidget.h"
#include <DTitlebar>
#include <QAction>
#include <QFont>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    setWindowTitle("Pong");
    resize(600, 440);
    setMinimumSize(600, 440);
    setMaximumSize(600, 440);

    QMenu *menu = new QMenu(this);
    QAction *newGameAction = menu->addAction(tr("Nuevo juego"));
    newGameAction->setShortcut(QKeySequence("Ctrl+N"));
    menu->addSeparator();
    QAction *quitAction = menu->addAction(tr("Salir"));
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    titlebar()->setMenu(menu);

    auto *centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_scoreLabel = new QLabel("0 : 0", this);
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    QFont scoreFont = m_scoreLabel->font();
    scoreFont.setPointSize(14);
    scoreFont.setBold(true);
    m_scoreLabel->setFont(scoreFont);
    m_scoreLabel->setFixedHeight(40);

    m_pongWidget = new PongWidget(this, m_scoreLabel);

    layout->addWidget(m_scoreLabel);
    layout->addWidget(m_pongWidget, 1);

    setCentralWidget(centralWidget);

    connect(newGameAction, &QAction::triggered, m_pongWidget, &PongWidget::resetGame);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
}
