#include "PongWidget.h"
#include <QApplication>
#include <QPainter>
#include <QLabel>
#include <QRandomGenerator>
#include <cmath>

PongWidget::PongWidget(QWidget *parent, QLabel *scoreLabel)
    : QWidget(parent)
    , m_scoreLabel(scoreLabel)
    , m_paddle1Pos(PADDLE_MARGIN, HEIGHT / 2 - PADDLE_HEIGHT / 2)
    , m_paddle2Pos(WIDTH - PADDLE_MARGIN - PADDLE_WIDTH, HEIGHT / 2 - PADDLE_HEIGHT / 2)
    , m_paddle1Dir(0)
    , m_paddle2Dir(0)
    , m_ballSpeed(BALL_SPEED)
    , m_score1(0)
    , m_score2(0)
    , m_winner(0)
    , m_paused(false)
    , m_vsCpu(false)
    , m_resetting(false)
    , m_serveTowards(1)
{
    setFixedSize(WIDTH, HEIGHT);
    setFocusPolicy(Qt::StrongFocus);

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &PongWidget::gameLoop);

    m_resetTimer = new QTimer(this);
    m_resetTimer->setSingleShot(true);
    connect(m_resetTimer, &QTimer::timeout, this, [this]() {
        m_resetting = false;
        resetBall(m_serveTowards);
    });

    resetGame();
}

void PongWidget::setVsCpu(bool enabled)
{
    m_vsCpu = enabled;
    m_paddle2Dir = 0;
    resetGame();
}

void PongWidget::updateLabel()
{
    if (m_vsCpu)
        m_scoreLabel->setText(tr("Tú %1 : %2 CPU").arg(m_score1).arg(m_score2));
    else
        m_scoreLabel->setText(tr("%1 : %2").arg(m_score1).arg(m_score2));
}

void PongWidget::resetGame()
{
    m_score1 = 0;
    m_score2 = 0;
    m_winner = 0;
    m_paused = false;
    m_resetting = false;
    m_resetTimer->stop();
    updateLabel();
    m_paddle1Pos = QPointF(PADDLE_MARGIN, HEIGHT / 2 - PADDLE_HEIGHT / 2);
    m_paddle2Pos = QPointF(WIDTH - PADDLE_MARGIN - PADDLE_WIDTH, HEIGHT / 2 - PADDLE_HEIGHT / 2);
    // Pausa breve antes del primer saque
    m_serveTowards = QRandomGenerator::global()->bounded(2) ? 1 : -1;
    resetBall(m_serveTowards);
    m_resetting = true;
    m_resetTimer->start(1000);
    m_gameTimer->start(16); // ~60 FPS
    update();
}

void PongWidget::togglePause()
{
    if (m_winner) return;
    m_paused = !m_paused;
    update();
}

// Saca hacia el jugador indicado (-1 izquierda, 1 derecha) con un angulo aleatorio
void PongWidget::resetBall(int towards)
{
    m_ballSpeed = BALL_SPEED;
    m_ballPos = QPointF(WIDTH / 2 - BALL_SIZE / 2, HEIGHT / 2 - BALL_SIZE / 2);
    double angle = (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * 0.45; // +-26 grados
    m_ballDir = QPointF(m_ballSpeed * towards * std::cos(angle), m_ballSpeed * std::sin(angle));
}

// La CPU sigue la pelota con velocidad limitada y solo cuando viene hacia ella
void PongWidget::updateCpu()
{
    double target = HEIGHT / 2.0;
    if (m_ballDir.x() > 0 && !m_resetting)
        target = m_ballPos.y() + BALL_SIZE / 2.0;
    double center = m_paddle2Pos.y() + PADDLE_HEIGHT / 2.0;
    double diff = target - center;
    if (std::abs(diff) > 4)
        m_paddle2Pos.setY(m_paddle2Pos.y() + qBound(-double(CPU_SPEED), diff, double(CPU_SPEED)));
}

// Rebote: el angulo depende de donde golpea la pelota y cada golpe acelera
void PongWidget::bounceOnPaddle(const QPointF &paddlePos, int newDirX)
{
    double hitPos = (m_ballPos.y() + BALL_SIZE / 2.0) - (paddlePos.y() + PADDLE_HEIGHT / 2.0);
    hitPos = qBound(-1.0, hitPos / (PADDLE_HEIGHT / 2.0), 1.0);
    m_ballSpeed = qMin(m_ballSpeed * 1.06, BALL_SPEED_MAX);
    double angle = hitPos * M_PI / 3.6; // hasta 50 grados
    m_ballDir = QPointF(newDirX * m_ballSpeed * std::cos(angle), m_ballSpeed * std::sin(angle));
}

void PongWidget::scorePoint(int player)
{
    if (player == 1) m_score1++; else m_score2++;
    updateLabel();
    if (m_score1 >= WIN_SCORE || m_score2 >= WIN_SCORE) {
        m_winner = player;
        update();
        return;
    }
    // Saca hacia quien acaba de perder el punto
    m_serveTowards = (player == 1) ? 1 : -1;
    m_resetting = true;
    m_resetTimer->start(700);
}

void PongWidget::gameLoop()
{
    if (m_paused || m_winner) return;

    // Move paddles
    m_paddle1Pos.setY(m_paddle1Pos.y() + m_paddle1Dir * PADDLE_SPEED);
    if (m_vsCpu)
        updateCpu();
    else
        m_paddle2Pos.setY(m_paddle2Pos.y() + m_paddle2Dir * PADDLE_SPEED);

    // Clamp paddles
    m_paddle1Pos.setY(qBound(0.0, m_paddle1Pos.y(), double(HEIGHT - PADDLE_HEIGHT)));
    m_paddle2Pos.setY(qBound(0.0, m_paddle2Pos.y(), double(HEIGHT - PADDLE_HEIGHT)));

    if (m_resetting) {
        update();
        return;
    }

    // Move ball
    m_ballPos += m_ballDir;

    // Ball collision with top/bottom walls
    if (m_ballPos.y() <= 0) {
        m_ballPos.setY(0);
        m_ballDir.setY(std::abs(m_ballDir.y()));
    } else if (m_ballPos.y() >= HEIGHT - BALL_SIZE) {
        m_ballPos.setY(HEIGHT - BALL_SIZE);
        m_ballDir.setY(-std::abs(m_ballDir.y()));
    }

    QRectF ballRect(m_ballPos, QSizeF(BALL_SIZE, BALL_SIZE));

    // Ball collision with left paddle
    QRectF paddle1Rect(m_paddle1Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT));
    if (ballRect.intersects(paddle1Rect) && m_ballDir.x() < 0) {
        bounceOnPaddle(m_paddle1Pos, 1);
        m_ballPos.setX(paddle1Rect.right() + 1);
    }

    // Ball collision with right paddle
    QRectF paddle2Rect(m_paddle2Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT));
    if (ballRect.intersects(paddle2Rect) && m_ballDir.x() > 0) {
        bounceOnPaddle(m_paddle2Pos, -1);
        m_ballPos.setX(paddle2Rect.left() - BALL_SIZE - 1);
    }

    // Scoring
    if (m_ballPos.x() < -BALL_SIZE)
        scorePoint(2);
    else if (m_ballPos.x() > WIDTH)
        scorePoint(1);

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
    painter.drawRoundedRect(QRectF(m_paddle1Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT)), 3, 3);
    painter.drawRoundedRect(QRectF(m_paddle2Pos, QSizeF(PADDLE_WIDTH, PADDLE_HEIGHT)), 3, 3);

    // Ball: se dibuja centrada en su rectangulo de colision
    if (!m_resetting) {
        painter.setBrush(fg);
        painter.drawEllipse(QPointF(m_ballPos.x() + BALL_SIZE / 2.0, m_ballPos.y() + BALL_SIZE / 2.0),
                            BALL_SIZE / 2.0, BALL_SIZE / 2.0);
    }

    // Controles
    painter.setPen(mid);
    QFont small = painter.font();
    small.setPixelSize(12);
    painter.setFont(small);
    painter.drawText(QRectF(8, HEIGHT - 20, WIDTH / 2 - 16, 18), Qt::AlignLeft | Qt::AlignVCenter, tr("W / S"));
    painter.drawText(QRectF(WIDTH / 2 + 8, HEIGHT - 20, WIDTH / 2 - 16, 18), Qt::AlignRight | Qt::AlignVCenter,
                     m_vsCpu ? tr("CPU") : tr("↑ / ↓"));

    if (m_paused || m_winner) {
        painter.fillRect(rect(), QColor(0, 0, 0, 140));
        painter.setPen(fg);
        QFont font = painter.font();
        font.setPixelSize(30);
        font.setBold(true);
        painter.setFont(font);
        QString title;
        if (m_winner)
            title = m_vsCpu ? (m_winner == 1 ? tr("¡GANASTE!") : tr("GANA LA CPU"))
                            : tr("¡GANA EL JUGADOR %1!").arg(m_winner);
        else
            title = tr("PAUSA");
        painter.drawText(rect().adjusted(0, -30, 0, -30), Qt::AlignCenter, title);
        font.setPixelSize(15);
        font.setBold(false);
        painter.setFont(font);
        painter.drawText(rect().adjusted(0, 25, 0, 25), Qt::AlignCenter,
                         m_winner ? tr("%1 : %2\nPresiona Enter para jugar otra vez").arg(m_score1).arg(m_score2)
                                  : tr("Presiona P para continuar"));
    }
}

void PongWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (m_winner) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            resetGame();
        return;
    }

    switch (event->key()) {
    case Qt::Key_W:
        m_paddle1Dir = -1;
        break;
    case Qt::Key_S:
        m_paddle1Dir = 1;
        break;
    case Qt::Key_Up:
        if (!m_vsCpu) m_paddle2Dir = -1;
        else m_paddle1Dir = -1; // contra la CPU tambien se puede usar flechas
        break;
    case Qt::Key_Down:
        if (!m_vsCpu) m_paddle2Dir = 1;
        else m_paddle1Dir = 1;
        break;
    case Qt::Key_P:
    case Qt::Key_Escape:
        togglePause();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void PongWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        QWidget::keyReleaseEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_W:
        if (m_paddle1Dir < 0) m_paddle1Dir = 0;
        break;
    case Qt::Key_S:
        if (m_paddle1Dir > 0) m_paddle1Dir = 0;
        break;
    case Qt::Key_Up:
        if (m_vsCpu) { if (m_paddle1Dir < 0) m_paddle1Dir = 0; }
        else if (m_paddle2Dir < 0) m_paddle2Dir = 0;
        break;
    case Qt::Key_Down:
        if (m_vsCpu) { if (m_paddle1Dir > 0) m_paddle1Dir = 0; }
        else if (m_paddle2Dir > 0) m_paddle2Dir = 0;
        break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}

void PongWidget::focusOutEvent(QFocusEvent *event)
{
    m_paddle1Dir = 0;
    m_paddle2Dir = 0;
    if (!m_paused && !m_winner) {
        m_paused = true;
        update();
    }
    QWidget::focusOutEvent(event);
}
