#include "MainWindow.h"
#include "PongWidget.h"
#include <DTitlebar>
#include <QAction>
#include <QFont>
#include <QLabel>
#include <QMenu>
#include <QSettings>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    setWindowTitle("Pong");

    QMenu *menu = new QMenu(this);
    QAction *newGameAction = menu->addAction(tr("Nuevo juego"));
    newGameAction->setShortcut(QKeySequence("Ctrl+N"));
    QAction *cpuAction = menu->addAction(tr("1 jugador (contra la CPU)"));
    cpuAction->setCheckable(true);
    // La tecla P la gestiona el propio campo de juego
    QAction *pauseAction = menu->addAction(tr("Pausa (P)"));
    menu->addSeparator();
    QAction *quitAction = menu->addAction(tr("Salir"));
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    titlebar()->setMenu(menu);
    // Los atajos del menu de la barra de titulo solo funcionan con el menu
    // abierto; anadirlas tambien a la ventana los activa siempre.
    addAction(newGameAction);
    addAction(quitAction);

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

    // El tamano de la ventana incluye la barra de titulo de DTK
    setFixedSize(m_pongWidget->width(),
                 m_scoreLabel->height() + m_pongWidget->height() + titlebar()->sizeHint().height());

    const bool vsCpu = QSettings().value("vsCpu", false).toBool();
    cpuAction->setChecked(vsCpu);
    m_pongWidget->setVsCpu(vsCpu);

    // Sin esto el teclado no responde hasta hacer clic en el campo
    m_pongWidget->setFocus();

    connect(newGameAction, &QAction::triggered, m_pongWidget, &PongWidget::resetGame);
    connect(cpuAction, &QAction::toggled, this, [this](bool on) {
        QSettings().setValue("vsCpu", on);
        m_pongWidget->setVsCpu(on);
    });
    connect(pauseAction, &QAction::triggered, m_pongWidget, &PongWidget::togglePause);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
}
