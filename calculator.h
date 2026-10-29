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

    void inputDigit(int digit);
    void inputDecimal();        // 小数点
    void inputBackspace();      // 退格
    void inputClear();          // C 全部清除
    void inputClearEntry();     // CE 清除当前项
    void connectButtons();
};

#endif // CALCULATOR_H
