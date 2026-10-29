#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QString>

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

private:
    Ui::Calculator *ui;

    QString m_display = "0";   // 主显示区当前字符串
    double  m_first = 0.0;     // 第一个操作数
    char    m_op = '\0';       // 待执行的二元运算符
    bool    m_waiting = false;  // 刚按过运算符，下一个数字开始新操作数

    void inputDigit(int digit);
    void inputDecimal();
    void inputBackspace();
    void inputClear();
    void inputClearEntry();
    void inputOperator(char op);
    void inputEquals();
    void connectButtons();
};

#endif // CALCULATOR_H
