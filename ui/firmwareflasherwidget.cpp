#include "firmwareflasherwidget.h"

FirmwareFlasherWidget::FirmwareFlasherWidget(QWidget *parent)
    : QWidget(parent)
{
    CreateUI();
    InsertWidgetsIntoLayout();
    FillUI();
    ConnectObjects();
}

FirmwareFlasherWidget::~FirmwareFlasherWidget()
{
    delete m_mainLayout;

    delete m_fileNameLabel;
    delete m_fileNameValue;
    delete m_addNewFileButton;
    delete m_flashButton;
    delete m_readFirmwareButton;
    delete m_firmWareProcessLabel;
    delete m_checkFirmwareCheckBox;
    delete m_restartFirmwareCheckBox;
}

void FirmwareFlasherWidget::CreateUI()
{
    m_mainLayout=new QVBoxLayout();
    m_fileNameLabel=new QLabel();
    m_fileNameValue=new QLabel();
    m_addNewFileButton=new QPushButton();
    m_flashButton=new QPushButton();
    m_readFirmwareButton=new QPushButton();
    m_firmWareProcessLabel=new QLabel();
    m_checkFirmwareCheckBox=new QCheckBox();
    m_restartFirmwareCheckBox=new QCheckBox();
}

void FirmwareFlasherWidget::InsertWidgetsIntoLayout()
{
    m_mainLayout->addWidget(m_fileNameLabel);
    m_mainLayout->addWidget(m_fileNameValue);
    m_mainLayout->addWidget(m_addNewFileButton);
    m_mainLayout->addWidget(m_flashButton);
    m_mainLayout->addWidget(m_readFirmwareButton);
    m_mainLayout->addWidget(m_firmWareProcessLabel);
    m_mainLayout->addWidget(m_checkFirmwareCheckBox);
    m_mainLayout->addWidget(m_restartFirmwareCheckBox);
    setLayout(m_mainLayout);
}

void FirmwareFlasherWidget::FillUI()
{
    m_fileNameLabel->setText(QStringLiteral("ПРОШИВКА С ПК \n Имя файла:"));
    m_fileNameValue->setText(QStringLiteral("Файл не выбран"));
    m_addNewFileButton->setText(QStringLiteral("Добавить файл"));
    m_flashButton->setText(QStringLiteral("Прошить"));
    m_readFirmwareButton->setText(QStringLiteral("Верифицировать прошивку"));
    QFont font=m_flashButton->font();
    font.setPixelSize(30);
    m_flashButton->setFont(font);
    m_flashButton->setFixedHeight(70);
    m_flashButton->setDisabled(true);
    m_readFirmwareButton->setFont(font);
    m_readFirmwareButton->setFixedHeight(70);
    m_readFirmwareButton->setDisabled(true);
    m_checkFirmwareCheckBox->setText(QStringLiteral("Проверить прошивку после загрузки"));
    m_checkFirmwareCheckBox->setChecked(false);
    m_restartFirmwareCheckBox->setText(QStringLiteral("Перезагрузить после прошивки"));
    m_restartFirmwareCheckBox->setChecked(false);
    m_firmWareProcessLabel->setText(QStringLiteral("Прошивка не загружается"));
}

void FirmwareFlasherWidget::ConnectObjects()
{
    connect(m_addNewFileButton, &QPushButton::clicked, this, &FirmwareFlasherWidget::OnAddNewFirmwareButtonClicked);
    connect(m_flashButton, &QPushButton::clicked,[&](bool checked){Q_UNUSED(checked) Q_EMIT ToFlash(&m_firmwareFromFile);});
    connect(m_readFirmwareButton, &QPushButton::clicked, this, &FirmwareFlasherWidget::StartReadingFirmWareFromDevice);
}

void FirmwareFlasherWidget::OnAddNewFirmwareButtonClicked()
{
    QString strDesktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString pathToFirmware=QFileDialog::getOpenFileName(this, QStringLiteral("Выберите файл прошивки"), strDesktop,  QStringLiteral("Файл настроек (*.bit)"));
    QFile file(pathToFirmware);
    if (file.open(QIODevice::ReadOnly))
    {
        m_firmwareFromFile=file.readAll();//107
        m_firmwareFromFile.remove(0, 107);
        m_flashButton->setEnabled(true);
        m_readFirmwareButton->setEnabled(true);
        m_firmWareProcessLabel->setText(QStringLiteral("Прошивка загружена"));
        m_fileNameValue->setText(file.fileName().section(QStringLiteral("/"),-1,-1));
    }
    else
    {
        Q_EMIT ToConsoleLog("Ошибка открытия файла " +file.errorString());
        m_fileNameValue->setText(QStringLiteral("Файл не выбран"));
        m_firmWareProcessLabel->setText(QStringLiteral("Прошивка не загружена"));
        m_flashButton->setDisabled(true);
        m_readFirmwareButton->setDisabled(true);
    }
}
