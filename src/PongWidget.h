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

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void gameLoop();
    void resetBall();

    QLabel *m_scoreLabel;

    // Game dimensions
    static const int WIDTH = 600;
    static const int HEIGHT = 400;

    // Paddle
    static const int PADDLE_WIDTH = 8;
    static const int PADDLE_HEIGHT = 60;
    static const int PADDLE_MARGIN = 20;
    static const int PADDLE_SPEED = 6;
    QPointF m_paddle1Pos; // left paddle
    QPointF m_paddle2Pos; // right paddle
    int m_paddle1Dir; // -1, 0, or 1
    int m_paddle2Dir; // -1, 0, or 1

    // Ball
    static const int BALL_SIZE = 10;
    static const int BALL_SPEED = 5;
    QPointF m_ballPos;
    QPointF m_ballDir;

    // Score
    int m_score1;
    int m_score2;

    // Timer
    QTimer *m_gameTimer;
    QTimer *m_resetTimer;
    bool m_resetting;
    int m_resetCounter;
};

#endif // PONGWIDGET_H
