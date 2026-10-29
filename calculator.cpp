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
    QPushButton *digits[10] = {
        ui->btn_0, ui->btn_1, ui->btn_2, ui->btn_3, ui->btn_4,
        ui->btn_5, ui->btn_6, ui->btn_7, ui->btn_8, ui->btn_9
    };
    for (int i = 0; i <= 9; ++i) {
        connect(digits[i], &QPushButton::clicked, this, [this, i]() { inputDigit(i); });
    }

    connect(ui->btn_dot,       &QPushButton::clicked, this, [this]() { inputDecimal(); });
    connect(ui->btn_backspace, &QPushButton::clicked, this, [this]() { inputBackspace(); });
    connect(ui->btn_clear,     &QPushButton::clicked, this, [this]() { inputClear(); });
    connect(ui->btn_ce,        &QPushButton::clicked, this, [this]() { inputClearEntry(); });
}

void Calculator::inputDigit(int digit)
{
    if (m_display == "0")
        m_display = QString::number(digit);
    else
        m_display.append(QString::number(digit));
    ui->displayLabel->setText(m_display);
}

void Calculator::inputDecimal()
{
    // 同一个操作数里只允许出现一个 '.'，避免 "1.2.3"
    if (!m_display.contains('.'))
        m_display.append('.');
    ui->displayLabel->setText(m_display);
}

void Calculator::inputBackspace()
{
    m_display.chop(1);
    if (m_display.isEmpty())
        m_display = "0";
    ui->displayLabel->setText(m_display);
}

void Calculator::inputClear()
{
    m_display = "0";
    ui->displayLabel->setText(m_display);
}

void Calculator::inputClearEntry()
{
    m_display = "0";
    ui->displayLabel->setText(m_display);
}
