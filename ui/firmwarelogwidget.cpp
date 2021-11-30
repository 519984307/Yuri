#include "firmwarelogwidget.h"



FirmwareLogWidget::FirmwareLogWidget(QWidget *parent)
    : QWidget(parent)
{
    CreateUI();
    InsertWidgetsIntoLayout();
    FillUI();
    ConnectObjects();
}

FirmwareLogWidget::~FirmwareLogWidget()
{
    delete m_mainLayout;

    delete m_restarPushButton;
    delete m_logClearButton;
    delete m_log;
    delete m_resultLabel;
}

void FirmwareLogWidget::CreateUI()
{
    m_mainLayout=new QVBoxLayout();
    m_restarPushButton=new QPushButton();
    m_logClearButton=new QPushButton();
    m_log=new QPlainTextEdit();
    m_resultLabel=new QLabel();
}

void FirmwareLogWidget::InsertWidgetsIntoLayout()
{
    m_mainLayout->addWidget(m_restarPushButton);
    m_mainLayout->addWidget(m_logClearButton);
    m_mainLayout->addWidget(m_log);
    m_mainLayout->addWidget(m_resultLabel);
    setLayout(m_mainLayout);
}

void FirmwareLogWidget::FillUI()
{
    m_restarPushButton->setText(QStringLiteral("Перезагрузить устройство"));
    m_logClearButton->setText(QStringLiteral("Отчистить консоль"));
    m_log->setReadOnly(true);
}

void FirmwareLogWidget::ConnectObjects()
{
    connect(m_logClearButton, &QPushButton::clicked, m_log, &QPlainTextEdit::clear);
    connect(m_restarPushButton, &QPushButton::clicked, this, &FirmwareLogWidget::ToRestartDevice);
}

void FirmwareLogWidget::OnConsoleLog(const QString &message)
{
    QString time=QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
    m_log->appendPlainText(time+ " "+ message);
}

void FirmwareLogWidget::OnResultLabelSetText(const QString &message)
{
    m_resultLabel->setText(message);
}
