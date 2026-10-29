#include "calculator.h"
#include "ui_calculator.h"

#include <QPushButton>
#include <QtMath>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);
    setDisplay(m_display);
    connectButtons();
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::connectButtons()
{
    QPushButton *digits[10] = {
        ui->btn_0, ui->btn_1, ui->btn_2, ui->btn_3, ui->btn_4,
        ui->btn_5, ui->btn_6, ui->btn_7, ui->btn_8, ui->btn_9
    };
    for (int i = 0; i <= 9; ++i)
        connect(digits[i], &QPushButton::clicked, this, [this, i]() { inputDigit(i); });

    connect(ui->btn_dot,       &QPushButton::clicked, this, [this]() { inputDecimal(); });
    connect(ui->btn_backspace, &QPushButton::clicked, this, [this]() { inputBackspace(); });
    connect(ui->btn_clear,     &QPushButton::clicked, this, [this]() { inputClear(); });
    connect(ui->btn_ce,        &QPushButton::clicked, this, [this]() { inputClearEntry(); });

    connect(ui->btn_plus,     &QPushButton::clicked, this, [this]() { inputOperator('+'); });
    connect(ui->btn_minus,    &QPushButton::clicked, this, [this]() { inputOperator('-'); });
    connect(ui->btn_multiply, &QPushButton::clicked, this, [this]() { inputOperator('*'); });
    connect(ui->btn_divide,   &QPushButton::clicked, this, [this]() { inputOperator('/'); });
    connect(ui->btn_equals,   &QPushButton::clicked, this, [this]() { inputEquals(); });
}

double Calculator::compute(double a, double b, char op, bool &ok) const
{
    ok = true;
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (qFuzzyIsNull(b)) { ok = false; return 0.0; }  // 修复：除零不再得到 inf
        return a / b;
    default:  return b;
    }
}

QString Calculator::formatDouble(double v) const
{
    if (qIsNaN(v) || qIsInf(v)) return "错误";
    return QString::number(v, 'g', 10);   // 去掉 0.1+0.2 的浮点长尾
}

void Calculator::setDisplay(const QString &text)
{
    m_display = text;
    ui->displayLabel->setText(m_display);
    ui->exprLabel->setText(m_expr);
}

void Calculator::showError(const QString &msg)
{
    m_error = true;
    m_op = '\0';
    m_waitingForOperand = true;
    m_justEvaluated = false;
    m_display = msg;
    ui->displayLabel->setText(m_display);
}

void Calculator::inputDigit(int digit)
{
    if (m_error) inputClear();

    if (m_justEvaluated) {          // 算完结果后输入数字 -> 全新计算
        m_expr.clear();
        m_first = 0.0;
        m_justEvaluated = false;
        m_waitingForOperand = false;
        m_display = QString::number(digit);
        setDisplay(m_display);
        return;
    }

    if (m_waitingForOperand) {      // 运算符之后 -> 开始第二个操作数
        m_waitingForOperand = false;
        m_display = QString::number(digit);
    } else {
        if (m_display.length() >= 12) return;
        if (m_display == "0") m_display = QString::number(digit);
        else m_display.append(QString::number(digit));
    }
    setDisplay(m_display);
}

void Calculator::inputDecimal()
{
    if (m_error) inputClear();
    if (m_justEvaluated) { m_expr.clear(); m_justEvaluated = false; m_waitingForOperand = false; m_display = "0."; setDisplay(m_display); return; }
    if (m_waitingForOperand) { m_waitingForOperand = false; m_display = "0."; }
    else if (!m_display.contains('.')) m_display.append('.');
    setDisplay(m_display);
}

static QString opSymbol(char op)
{
    switch (op) {
    case '+': return " + "; case '-': return " - ";
    case '*': return " × "; case '/': return " ÷ ";
    default:  return QString();
    }
}

void Calculator::inputOperator(char op)
{
    if (m_error) return;
    double newValue = m_display.toDouble();

    if (m_justEvaluated) { m_first = newValue; m_justEvaluated = false; }

    if (m_waitingForOperand) {
        m_op = op;                    // 连续按运算符：只替换，不重复算
    } else if (m_op != '\0') {
        bool ok = true;
        double result = compute(m_first, newValue, m_op, ok);
        if (!ok) { showError("除数不能为0"); return; }
        m_first = result;
        setDisplay(formatDouble(result));
    } else {
        m_first = newValue;
    }

    m_op = op;
    m_waitingForOperand = true;
    m_expr = formatDouble(m_first) + opSymbol(op);
    ui->exprLabel->setText(m_expr);
}

void Calculator::inputEquals()
{
    if (m_error || m_op == '\0') return;
    double second = m_display.toDouble();
    bool ok = true;
    double result = compute(m_first, second, m_op, ok);

    m_expr = formatDouble(m_first) + opSymbol(m_op) + formatDouble(second) + " =";
    m_op = '\0';
    m_waitingForOperand = true;
    m_justEvaluated = true;

    if (!ok) { showError("除数不能为0"); ui->exprLabel->setText(m_expr); return; }
    m_first = result;
    setDisplay(formatDouble(result));
    ui->exprLabel->setText(m_expr);
}

void Calculator::inputBackspace()
{
    if (m_error) { inputClear(); return; }
    if (m_waitingForOperand || m_justEvaluated) return;
    m_display.chop(1);
    if (m_display.isEmpty() || m_display == "-") m_display = "0";
    setDisplay(m_display);
}

void Calculator::inputClear()
{
    m_display = "0"; m_expr.clear(); m_first = 0.0;
    m_op = '\0'; m_waitingForOperand = false; m_justEvaluated = false; m_error = false;
    setDisplay(m_display);
}

void Calculator::inputClearEntry()
{
    if (m_error) { inputClear(); return; }
    m_display = "0"; m_waitingForOperand = false;
    setDisplay(m_display);
}
