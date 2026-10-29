#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <qmath.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    for (int i = 0; i <= 9; ++i) {
        QPushButton *btn = findChild<QPushButton*>(QString("btn%1").arg(i));
        if (btn) connect(btn, &QPushButton::clicked, this, &Widget::onDigitClicked);
    }

    connect(ui->btnAdd, &QPushButton::clicked, this, &Widget::onOperatorClicked);
    connect(ui->btnSub, &QPushButton::clicked, this, &Widget::onOperatorClicked);
    connect(ui->btnMul, &QPushButton::clicked, this, &Widget::onOperatorClicked);
    connect(ui->btnDiv, &QPushButton::clicked, this, &Widget::onOperatorClicked);

    connect(ui->btnEqual, &QPushButton::clicked, this, &Widget::onEqualClicked);
    connect(ui->btnClear, &QPushButton::clicked, this, &Widget::onClearClicked);
    connect(ui->btnBackspace, &QPushButton::clicked, this, &Widget::onBackspaceClicked);
    connect(ui->btnPoint, &QPushButton::clicked, this, &Widget::onPointClicked);
    connect(ui->btnSign, &QPushButton::clicked, this, &Widget::onSignClicked);
    connect(ui->btnPercent, &QPushButton::clicked, this, &Widget::onPercentClicked);
}

Widget::~Widget() { delete ui; }

void Widget::onDigitClicked() {
    if (m_clearDisplay || m_waitingForOperand) {
        ui->display->clear();
        m_clearDisplay = false;
        m_waitingForOperand = false;
        m_hasDecimal = false;
    }
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    ui->display->setText(ui->display->text() + btn->text());
}

void Widget::onPointClicked() {
    if (m_waitingForOperand || m_clearDisplay) {
        ui->display->setText("0.");
        m_waitingForOperand = false;
        m_clearDisplay = false;
        m_hasDecimal = true;
        return;
    }
    if (m_hasDecimal) return;
    ui->display->setText(ui->display->text() + ".");
    m_hasDecimal = true;
}

void Widget::onOperatorClicked() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    QString op = btn->text();
    double current = ui->display->text().toDouble();

    if (!m_pendingOperator.isEmpty() && !m_waitingForOperand) {
        if (!calculate(current)) return;
    } else {
        m_leftOperand = current;
    }
    m_pendingOperator = op;
    m_waitingForOperand = true;
    m_hasDecimal = false;
    m_clearDisplay = false;
}

void Widget::onEqualClicked() {
    if (m_pendingOperator.isEmpty() || m_waitingForOperand) return;
    double right = ui->display->text().toDouble();
    if (calculate(right)) {
        m_pendingOperator.clear();
        m_waitingForOperand = true;
        m_clearDisplay = true;
    }
}

void Widget::onClearClicked() {
    ui->display->clear();
    resetState();
}

void Widget::onBackspaceClicked() {
    if (m_clearDisplay || m_waitingForOperand) return;
    QString text = ui->display->text();
    if (text.isEmpty()) return;

    QChar removed = text.back();
    text.chop(1);
    if (text.isEmpty() || text == "-") text = "0";

    if (removed == '.') m_hasDecimal = false;
    ui->display->setText(text);
}

void Widget::onSignClicked() {
    QString text = ui->display->text();
    if (text == "0" || text.isEmpty() || text == "除数不能为0") return;
    if (text.startsWith("-")) {
        text.remove(0, 1);
    } else {
        text.prepend("-");
    }
    ui->display->setText(text);
}

void Widget::onPercentClicked() {
    double val = ui->display->text().toDouble();
    val /= 100.0;
    ui->display->setText(QString::number(val, 'g', 12));
    m_waitingForOperand = true;
}

bool Widget::calculate(double rightOperand) {
    double result = 0.0;
    if (m_pendingOperator == "+") result = m_leftOperand + rightOperand;
    else if (m_pendingOperator == "-") result = m_leftOperand - rightOperand;
    else if (m_pendingOperator == "×" || m_pendingOperator == "*")
        result = m_leftOperand * rightOperand;
    else if (m_pendingOperator == "÷" || m_pendingOperator == "/") {
        if (qFuzzyIsNull(rightOperand)) {
            ui->display->setText("除数不能为0");
            resetState();
            return false;
        }
        result = m_leftOperand / rightOperand;
    }
    m_leftOperand = result;
    ui->display->setText(QString::number(result, 'g', 12));
    return true;
}

void Widget::resetState() {
    m_leftOperand = 0.0;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasDecimal = false;
    m_clearDisplay = true;
}

void Widget::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        QPushButton *btn = findChild<QPushButton*>(QString("btn%1").arg(key - Qt::Key_0));
        if (btn) btn->click();
        return;
    }
    switch (key) {
    case Qt::Key_Plus: ui->btnAdd->click(); break;
    case Qt::Key_Minus: ui->btnSub->click(); break;
    case Qt::Key_Asterisk: ui->btnMul->click(); break;
    case Qt::Key_Slash: ui->btnDiv->click(); break;
    case Qt::Key_Equal:
    case Qt::Key_Return:
    case Qt::Key_Enter: ui->btnEqual->click(); break;
    case Qt::Key_Period: ui->btnPoint->click(); break;
    case Qt::Key_Backspace: ui->btnBackspace->click(); break;
    case Qt::Key_Escape: ui->btnClear->click(); break;
    default: QWidget::keyPressEvent(event); return;
    }
}