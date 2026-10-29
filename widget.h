#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onDigitClicked();
    void onOperatorClicked();
    void onEqualClicked();
    void onClearClicked();
    void onBackspaceClicked();
    void onPointClicked();
    void onSignClicked();
    void onPercentClicked();

private:
    Ui::Widget *ui;

    double m_leftOperand = 0.0;
    QString m_pendingOperator;
    bool m_waitingForOperand = true;
    bool m_hasDecimal = false;
    bool m_clearDisplay = false;

    bool calculate(double rightOperand);
    void resetState();
};

#endif // WIDGET_H