#include "calculator.h"
#include "ui_calculator.h"

#include <QPushButton>
#include <QtMath>
#include <QKeyEvent>

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

    connect(ui->btn_negate,     &QPushButton::clicked, this, [this]() { inputNegate(); });
    connect(ui->btn_percent,    &QPushButton::clicked, this, [this]() { inputPercent(); });
    connect(ui->btn_reciprocal, &QPushButton::clicked, this, [this]() { inputReciprocal(); });
    connect(ui->btn_square,     &QPushButton::clicked, this, [this]() { inputSquare(); });
    connect(ui->btn_sqrt,       &QPushButton::clicked, this, [this]() { inputSqrt(); });
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

// —— 单目 / 其他 ——
void Calculator::inputNegate()
{
    if (m_error || m_display == "0") return;
    if (m_display.startsWith('-')) m_display = m_display.mid(1);
    else m_display = "-" + m_display;
    setDisplay(m_display);
}

void Calculator::inputPercent()
{
    if (m_error) return;
    m_display = formatDouble(m_display.toDouble() / 100.0);
    setDisplay(m_display);
}

double Calculator::applyUnary(std::function<double(double)> fn, const QString &exprText)
{
    if (m_error) return 0;
    double v = m_display.toDouble();
    double r = fn(v);
    if (qIsNaN(r) || qIsInf(r)) { showError("错误"); return 0; }
    m_expr = exprText.arg(formatDouble(v));
    m_display = formatDouble(r);
    m_justEvaluated = true;
    setDisplay(m_display);
    ui->exprLabel->setText(m_expr);
    return r;
}

void Calculator::inputReciprocal()
{
    if (qFuzzyIsNull(m_display.toDouble())) { showError("除数不能为0"); return; }
    applyUnary([](double v) { return 1.0 / v; }, "1 / (%1)");
}

void Calculator::inputSquare()
{
    applyUnary([](double v) { return v * v; }, "(%1)^2");
}

void Calculator::inputSqrt()
{
    if (m_display.toDouble() < 0) { showError("负数不能开平方"); return; }
    applyUnary([](double v) { return qSqrt(v); }, "√(%1)");
}

// —— 键盘事件：只做"按键 -> 语义动作"的翻译，逻辑全部复用上面的函数 ——
void Calculator::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_0: case Qt::Key_1: case Qt::Key_2: case Qt::Key_3:
    case Qt::Key_4: case Qt::Key_5: case Qt::Key_6: case Qt::Key_7:
    case Qt::Key_8: case Qt::Key_9:
        inputDigit(event->key() - Qt::Key_0);
        break;
    case Qt::Key_Period:
    case Qt::Key_Comma:
        inputDecimal();
        break;
    case Qt::Key_Plus:   inputOperator('+'); break;
    case Qt::Key_Minus:  inputOperator('-'); break;
    case Qt::Key_Asterisk:
    case Qt::Key_X:      inputOperator('*'); break;
    case Qt::Key_Slash:
        event->accept();
        inputOperator('/');
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Equal:  inputEquals(); break;
    case Qt::Key_Backspace: inputBackspace(); break;
    case Qt::Key_Escape:    inputClear(); break;
    case Qt::Key_Percent:   inputPercent(); break;
    default: QWidget::keyPressEvent(event); break;
    }
}
