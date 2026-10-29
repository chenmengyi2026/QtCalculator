#include "calculator.h"
#include "ui_calculator.h"

#include <QPushButton>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);
    ui->displayLabel->setText(m_display);
    connectButtons();
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::connectButtons()
{
    // 数字按钮用循环统一连接，避免 0~9 写十段重复代码
    QPushButton *digits[10] = {
        ui->btn_0, ui->btn_1, ui->btn_2, ui->btn_3, ui->btn_4,
        ui->btn_5, ui->btn_6, ui->btn_7, ui->btn_8, ui->btn_9
    };
    for (int i = 0; i <= 9; ++i) {
        connect(digits[i], &QPushButton::clicked, this, [this, i]() { inputDigit(i); });
    }
}

void Calculator::inputDigit(int digit)
{
    if (m_display == "0")
        m_display = QString::number(digit);   // 初始的 0 不前置
    else
        m_display.append(QString::number(digit));
    ui->displayLabel->setText(m_display);
}
