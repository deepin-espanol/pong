#ifndef PONGWIDGET_H
#define PONGWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>

class QLabel;

class PongWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PongWidget(QWidget *parent = nullptr, QLabel *scoreLabel = nullptr);

    void resetGame();
    void togglePause();
    void setVsCpu(bool enabled);
    bool vsCpu() const { return m_vsCpu; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void gameLoop();
    void resetBall(int towards);
    void updateCpu();
    void bounceOnPaddle(const QPointF &paddlePos, int newDirX);
    void scorePoint(int player);
    void updateLabel();

    QLabel *m_scoreLabel;

    // Game dimensions
    static const int WIDTH = 600;
    static const int HEIGHT = 400;
    static const int WIN_SCORE = 11;

    // Paddle
    static const int PADDLE_WIDTH = 8;
    static const int PADDLE_HEIGHT = 60;
    static const int PADDLE_MARGIN = 20;
    static const int PADDLE_SPEED = 6;
    static const int CPU_SPEED = 5;
    QPointF m_paddle1Pos; // left paddle
    QPointF m_paddle2Pos; // right paddle
    int m_paddle1Dir; // -1, 0, or 1
    int m_paddle2Dir; // -1, 0, or 1

    // Ball
    static const int BALL_SIZE = 10;
    static constexpr double BALL_SPEED = 5.0;
    static constexpr double BALL_SPEED_MAX = 11.0;
    QPointF m_ballPos;   // esquina superior izquierda de la pelota
    QPointF m_ballDir;
    double m_ballSpeed;

    // Score
    int m_score1;
    int m_score2;
    int m_winner;  // 0 = nadie, 1 o 2
    bool m_paused;
    bool m_vsCpu;

    // Timer
    QTimer *m_gameTimer;
    QTimer *m_resetTimer;
    bool m_resetting;
    int m_serveTowards;
};

#endif // PONGWIDGET_H
