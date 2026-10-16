#include "PongWidget.h"
#include <QApplication>
#include <QPainter>
#include <QLabel>
#include <cmath>
#include <cstdlib>

PongWidget::PongWidget(QWidget *parent, QLabel *scoreLabel)
    : QWidget(parent)
    , m_scoreLabel(scoreLabel)
    , m_paddle1Pos(PADDLE_MARGIN, HEIGHT / 2 - PADDLE_HEIGHT / 2)
    , m_paddle2Pos(WIDTH - PADDLE_MARGIN - PADDLE_WIDTH, HEIGHT / 2 - PADDLE_HEIGHT / 2)
    , m_paddle1Dir(0)
    , m_paddle2Dir(0)
    , m_score1(0)
    , m_score2(0)
    , m_resetting(false)
    , m_resetCounter(0)
{
    setFixedSize(WIDTH, HEIGHT);
    setFocusPolicy(Qt::StrongFocus);

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &PongWidget::gameLoop);

    m_resetTimer = new QTimer(this);
    m_resetTimer->setSingleShot(true);
    connect(m_resetTimer, &QTimer::timeout, this, [this]() {
        m_resetting = false;
        resetBall();
    });

    resetGame();
}

void PongWidget::resetGame()
{
    m_score1 = 0;
    m_score2 = 0;
    m_scoreLabel->setText(tr("0 : 0"));
    m_paddle1Pos = QPointF(PADDLE_MARGIN, HEIGHT / 2 - PADDLE_HEIGHT / 2);
    m_paddle2Pos = QPointF(WIDTH - PADDLE_MARGIN - PADDLE_WIDTH, HEIGHT / 2 - PADDLE_HEIGHT / 2);
    resetBall();
    m_gameTimer->start(16); // ~60 FPS
}

void PongWidget::resetBall()
{
    m_ballPos = QPointF(WIDTH / 2 - BALL_SIZE / 2, HEIGHT / 2 - BALL_SIZE / 2);

    // Random angle between -45 and 45 degrees
    double angle = (std::rand() % 9001 - 4500) / 10000.0; // -0.45 to 0.45 rad (~-26 to 26 degrees)
    // Randomly choose direction
    int dir = (std::rand() % 2) * 2 - 1; // -1 or 1
    m_ballDir = QPointF(BALL_SPEED * dir * std::cos(angle), BALL_SPEED * std::sin(angle));
}

void PongWidget::gameLoop()
{
    // Move paddles
    m_paddle1Pos.setY(m_paddle1Pos.y() + m_paddle1Dir * PADDLE_SPEED);
    m_paddle2Pos.setY(m_paddle2Pos.y() + m_paddle2Dir * PADDLE_SPEED);

    // Clamp paddles
    if (m_paddle1Pos.y() < 0) m_paddle1Pos.setY(0);
    if (m_paddle1Pos.y() > HEIGHT - PADDLE_HEIGHT) m_paddle1Pos.setY(HEIGHT - PADDLE_HEIGHT);
    if (m_paddle2Pos.y() < 0) m_paddle2Pos.setY(0);
    if (m_paddle2Pos.y() > HEIGHT - PADDLE_HEIGHT) m_paddle2Pos.setY(HEIGHT - PADDLE_HEIGHT);

    if (m_resetting) return;

    // Move ball
    m_ballPos += m_ballDir;

    // Ball collision with top/bottom walls
    if (m_ballPos.y() <= 0 || m_ballPos.y() >= HEIGHT - BALL_SIZE) {
        m_ballDir.setY(-m_ballDir.y());
        m_ballPos.setY(m_ballPos.y() + m_ballDir.y());
    }

    // Ball collision with left paddle
    QRectF paddle1Rect(m_paddle1Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT));
    QRectF ballRect(m_ballPos, QSizeF(BALL_SIZE, BALL_SIZE));
    if (ballRect.intersects(paddle1Rect) && m_ballDir.x() < 0) {
        m_ballDir.setX(-m_ballDir.x());
        // Adjust angle based on where ball hit the paddle
        double hitPos = (m_ballPos.y() + BALL_SIZE / 2) - (m_paddle1Pos.y() + PADDLE_HEIGHT / 2);
        hitPos /= (PADDLE_HEIGHT / 2); // normalize to -1..1
        m_ballDir.setY(BALL_SPEED * hitPos);
    }

    // Ball collision with right paddle
    QRectF paddle2Rect(m_paddle2Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT));
    if (ballRect.intersects(paddle2Rect) && m_ballDir.x() > 0) {
        m_ballDir.setX(-m_ballDir.x());
        double hitPos = (m_ballPos.y() + BALL_SIZE / 2) - (m_paddle2Pos.y() + PADDLE_HEIGHT / 2);
        hitPos /= (PADDLE_HEIGHT / 2);
        m_ballDir.setY(BALL_SPEED * hitPos);
    }

    // Scoring
    if (m_ballPos.x() < -BALL_SIZE) {
        // Player 2 scores
        m_score2++;
        m_scoreLabel->setText(tr("%1 : %2").arg(m_score1).arg(m_score2));
        m_resetting = true;
        m_resetTimer->start(500);
    } else if (m_ballPos.x() > WIDTH) {
        // Player 1 scores
        m_score1++;
        m_scoreLabel->setText(tr("%1 : %2").arg(m_score1).arg(m_score2));
        m_resetting = true;
        m_resetTimer->start(500);
    }

    update();
}

void PongWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPalette pal = QApplication::palette();
    QColor field = pal.color(QPalette::Base);
    QColor fg = pal.color(QPalette::WindowText);
    QColor mid = pal.color(QPalette::Mid);

    // Background
    painter.fillRect(rect(), field);

    // Center line
    painter.setPen(QPen(mid, 2, Qt::DashLine));
    painter.drawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT);

    // Paddles
    painter.setBrush(fg);
    painter.setPen(Qt::NoPen);
    painter.drawRect(QRectF(m_paddle1Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT)));
    painter.drawRect(QRectF(m_paddle2Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT)));

    // Ball
    painter.setBrush(fg);
    painter.drawEllipse(m_ballPos, BALL_SIZE / 2, BALL_SIZE / 2);
}

void PongWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:
        m_paddle1Dir = -1;
        break;
    case Qt::Key_S:
        m_paddle1Dir = 1;
        break;
    case Qt::Key_Up:
        m_paddle2Dir = -1;
        break;
    case Qt::Key_Down:
        m_paddle2Dir = 1;
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void PongWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_S:
        m_paddle1Dir = 0;
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
        m_paddle2Dir = 0;
        break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}
