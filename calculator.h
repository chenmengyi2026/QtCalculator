#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QString>
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

// 带键盘事件的计算器
class Calculator : public QWidget
{
    Q_OBJECT
public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator();

protected:
    // 键盘事件：把按键翻译成语义动作，复用与鼠标相同的处理函数
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::Calculator *ui;

    // —— 计算状态 ——
    QString m_display = "0";     // 当前操作数/结果
    QString m_expr;              // 上方灰色表达式
    double  m_first = 0.0;       // 第一个操作数
    char    m_op = '\0';         // 挂起的二元运算符
    bool    m_waitingForOperand = false; // 刚按运算符/等号，下一数字开始新操作数
    bool    m_justEvaluated = false;     // 刚按等号
    bool    m_error = false;            // 出错（如除零）

    void inputDigit(int digit);
    void inputDecimal();
    void inputBackspace();
    void inputClear();
    void inputClearEntry();
    void inputOperator(char op);
    void inputEquals();
    void inputNegate();
    void inputPercent();
    void inputReciprocal();
    void inputSquare();
    void inputSqrt();

    double compute(double a, double b, char op, bool &ok) const;
    double applyUnary(std::function<double(double)> fn, const QString &exprText);
    QString formatDouble(double v) const;
    void setDisplay(const QString &text);
    void showError(const QString &msg);
    void connectButtons();
};

#endif // CALCULATOR_H
