#include "statewidget.h"
#include "qdatetime.h"
#include <QDebug>
#include <QTimer>

StateWidget::StateWidget(QWidget *parent)
    : QWidget(parent)
    , m_attenuatorValues({"0", "1", "2", "3", "4", "5", "6", "9", "12", "15", "18", "21", "24", "27", "30"})
{
    CreateObjects();
    CreateUI();
    InsertWidgetsIntoLayout();
    FillUI();
    ConnectObjects();
}

StateWidget::~StateWidget()
{
    m_file->close();
    delete m_statePresenter;

    delete m_file;
    delete m_intValidator;
    delete m_gainValidator;

    delete m_speedLayout;
    delete m_rangeLineLayout;
    delete m_workPointLineLayout;
    delete m_gainLineLayout;
    delete m_attenuatorLineLayout;
    delete m_messageSendButtonsLayout;
    delete m_messageGetButtonsLayout;
    delete m_stateLayout;
    delete m_noiseLineLayout;
    delete m_mainLayout;


    delete m_DoplerFreqLabel;
    delete m_doplerFreqLineEdit;

    delete m_speedLabel;
    delete m_speedLineEdit;

    delete m_rangeLabel;
    delete m_rangeLineEdit;

    delete m_fvcoLabel;
    delete m_fvcoComboBox;

    delete m_gainTXLabel;
    delete m_gainTXLineEdit;
    delete m_gainRXLabel;
    delete m_gainRXLineEdit;

    delete m_attenuatorLabel;
    delete m_attenuatorComboBox;

    delete m_noiseComboBox;
    delete m_noiseLabel;

    qDeleteAll(m_sendStateButtonsGroup->buttons());
    delete m_sendStateButtonsGroup;
    delete m_getStateButtonGroup;

    delete m_logClearButton;

    delete m_log;
}

void StateWidget::DisconnectOldHander()
{
    m_statePresenter->DisconnectOldHandler();
}

void StateWidget::ConnectHander(DataHandler *dataHandler)
{
    m_statePresenter->ConnectHander(dataHandler);
}

void StateWidget::CreateObjects()
{
    m_statePresenter=new StatePresenter(this);
    m_file=new QFile(QDir::currentPath()+"/история запросов.txt");
    m_intValidator=new QIntValidator();
    m_gainValidator=new QIntValidator(0, 64);
    m_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    long long minWorkPointValue=2695;
    for (int i=0; i<30; i++)
    {
        workPointsValues.append(QString::number(minWorkPointValue));
        minWorkPointValue=minWorkPointValue+10;
    }
}


void StateWidget::CreateUI()
{
    m_mainLayout=new QVBoxLayout();
    m_speedLayout=new QHBoxLayout();
    m_rangeLineLayout=new QHBoxLayout();
    m_workPointLineLayout=new QHBoxLayout();
    m_gainLineLayout=new QHBoxLayout();
    m_attenuatorLineLayout=new QHBoxLayout();
    m_messageSendButtonsLayout=new QHBoxLayout();
    m_messageGetButtonsLayout=new QHBoxLayout();
    m_stateLayout=new QHBoxLayout();
    m_noiseLineLayout=new QHBoxLayout();

    m_fvcoLabel=new QLabel();
    m_fvcoComboBox=new QComboBox();

    m_DoplerFreqLabel=new QLabel();
    m_doplerFreqLineEdit=new QLineEdit();

    m_speedLabel=new QLabel();
    m_speedLineEdit=new QLineEdit();

    m_rangeLabel=new QLabel();
    m_rangeLineEdit=new QLineEdit();


    m_gainTXLabel=new QLabel();
    m_gainTXLineEdit=new QLineEdit();


    m_gainRXLabel=new QLabel();
    m_gainRXLineEdit=new QLineEdit();


    m_attenuatorLabel=new QLabel();
    m_attenuatorComboBox=new QComboBox();


    m_noiseLabel=new QLabel();
    m_noiseValueLabel=new QLabel();
    m_noiseLineEdit=new QLineEdit();
    m_noiseComboBox=new QComboBox();

    m_sendStateButtonsGroup=new QButtonGroup();
    FillButtonGroup();

    //"Адрес: адрес мохи с 1 в конце или 192.168.111.1 на РЛС ТИ. Порт: через  ЛК Operation Settings. Посмотреть http://192.168.127.254/ снизу мохи адрес Лог:Пасс admin:moxa"
    m_logClearButton=new QPushButton();

    m_log=new QPlainTextEdit();
    m_log->setStyleSheet(QStringLiteral("color: white; background-color: black;"));


}

void StateWidget::FillButtonGroup()
{

    m_sendStateButtonsGroup->addButton(new QPushButton(QStringLiteral("Установить RX")), 1);
    m_sendStateButtonsGroup->addButton(new QPushButton(QStringLiteral("Установить RX и ТХ")), 21);
    m_sendStateButtonsGroup->addButton(new QPushButton(QStringLiteral("Установить TX")), 2);
    m_sendStateButtonsGroup->addButton(new QPushButton(QStringLiteral("Установить")), 3);

    m_sendStateButtonsGroup->addButton(new QPushButton(QStringLiteral("Установить")), 4);
    m_sendStateButtonsGroup->addButton(new QPushButton(QStringLiteral("Установить")), 5);
    m_sendStateButtonsGroup->addButton(new QPushButton(QStringLiteral("Пинг")), 0);

    m_getStateButtonGroup=new QButtonGroup();
    m_getStateButtonGroup->addButton(new QPushButton(QStringLiteral("Запросить")), 1);
    m_getStateButtonGroup->addButton(new QPushButton(QStringLiteral("Запросить")), 21);
    m_getStateButtonGroup->addButton(new QPushButton(QStringLiteral("Запросить")), 3);
    m_getStateButtonGroup->addButton(new QPushButton(QStringLiteral("Запросить")), 4);
    m_getStateButtonGroup->addButton(new QPushButton(QStringLiteral("Запросить")), 5);
    m_getStateButtonGroup->addButton(new QPushButton(QStringLiteral("Запросить")), 6);
}

void StateWidget::InsertWidgetsIntoLayout()
{
    QList<QAbstractButton*> sendStateButtonList=m_sendStateButtonsGroup->buttons();
    QList<QAbstractButton*> getStateButtonList=m_getStateButtonGroup->buttons();
    QList<QAbstractButton*>::const_iterator setStateButtonListInterator=sendStateButtonList.cbegin();
    QList<QAbstractButton*>::const_iterator getStateButtonLIstIterator=getStateButtonList.cbegin();

    m_workPointLineLayout->addWidget(m_fvcoLabel);
    m_workPointLineLayout->addWidget(m_fvcoComboBox);
    AddButtonFromGroupToLayout(&setStateButtonListInterator, &setStateButtonListInterator, m_workPointLineLayout);
    AddButtonFromGroupToLayout(&getStateButtonLIstIterator, nullptr, m_workPointLineLayout);

    m_speedLayout->addWidget(m_DoplerFreqLabel);
    m_speedLayout->addWidget(m_doplerFreqLineEdit);
    m_speedLayout->addWidget(m_speedLabel);
    m_speedLayout->addWidget(m_speedLineEdit);
    AddButtonFromGroupToLayout(&setStateButtonListInterator, &getStateButtonLIstIterator, m_speedLayout);

    m_rangeLineLayout->addWidget(m_rangeLabel);
    m_rangeLineLayout->addWidget(m_rangeLineEdit);
    AddButtonFromGroupToLayout(&setStateButtonListInterator, &getStateButtonLIstIterator, m_rangeLineLayout);

    m_gainLineLayout->addWidget(m_gainTXLabel);
    m_gainLineLayout->addWidget(m_gainTXLineEdit);
    m_gainLineLayout->addWidget(m_gainRXLabel);
    m_gainLineLayout->addWidget(m_gainRXLineEdit);
    AddButtonFromGroupToLayout(&setStateButtonListInterator, &getStateButtonLIstIterator, m_gainLineLayout);

    m_attenuatorLineLayout->addWidget(m_attenuatorLabel);
    m_attenuatorLineLayout->addWidget(m_attenuatorComboBox);
    AddButtonFromGroupToLayout(&setStateButtonListInterator, &getStateButtonLIstIterator, m_attenuatorLineLayout);

    m_noiseLineLayout->addWidget(m_noiseValueLabel);
    m_noiseLineLayout->addWidget(m_noiseLineEdit);
    m_noiseLineLayout->addWidget(m_noiseLabel);
    m_noiseLineLayout->addWidget(m_noiseComboBox);
    AddButtonFromGroupToLayout(&getStateButtonLIstIterator, nullptr, m_noiseLineLayout);

    m_mainLayout->addLayout(m_workPointLineLayout);
    m_mainLayout->addLayout(m_speedLayout);
    m_mainLayout->addLayout(m_rangeLineLayout);
    m_mainLayout->addLayout(m_gainLineLayout);
    m_mainLayout->addLayout(m_attenuatorLineLayout);
    m_mainLayout->addLayout(m_noiseLineLayout);
    m_mainLayout->addLayout(m_messageSendButtonsLayout);
    m_mainLayout->addLayout(m_messageGetButtonsLayout);

    AddButtonFromGroupToLayout(&setStateButtonListInterator, nullptr, m_mainLayout);
    m_mainLayout->addWidget(m_logClearButton);
    m_mainLayout->addWidget(m_log);
    setLayout(m_mainLayout);
}

void StateWidget::FillUI()
{
    m_fvcoLabel->setText(QStringLiteral("Рабочая точка Fvco для частот Tx и Rx: МЕГАгерцы"));
    m_DoplerFreqLabel->setText(QStringLiteral("Частота Доплера (по желанию) для частоты Tx: ГЕРЦЫ"));
    m_speedLabel->setText(QStringLiteral("Скорость может быть <0: метры/секунду "));
    m_rangeLabel->setText(QStringLiteral("Дальность ответного сигнала d: метры"));
    m_gainTXLabel->setText(QStringLiteral("Усиление TX GAIN_TX: децибелы"));
    m_gainRXLabel->setText(QStringLiteral("Усиление RX GAIN_RX: децибелы"));
    m_attenuatorLabel->setText(QStringLiteral("Установка ослабления Attenuator_RX: децибелы"));
    m_noiseValueLabel->setText(QStringLiteral("Значения для синуса и м сигнала"));
    m_noiseLabel->setText(QStringLiteral("Режимы работы:"));

    m_speedLineEdit->setValidator(m_intValidator);
    m_doplerFreqLineEdit->setValidator(m_intValidator);
    m_rangeLineEdit->setValidator(m_intValidator);
    m_gainTXLineEdit->setValidator(m_gainValidator);
    m_gainRXLineEdit->setValidator(m_gainValidator);

    m_doplerFreqLineEdit->setText(QStringLiteral("0"));
    m_speedLineEdit->setText(QStringLiteral("0"));
    m_rangeLineEdit->setText(QStringLiteral("0"));
    m_noiseLineEdit->setText(QStringLiteral("0"));
    m_gainTXLineEdit->setText(QStringLiteral("32"));
    m_gainRXLineEdit->setText(QStringLiteral("0"));


    m_fvcoComboBox->setEditable(true);
    m_fvcoComboBox->addItems(workPointsValues);
    m_attenuatorComboBox->addItems(m_attenuatorValues);
    m_attenuatorComboBox->setCurrentIndex(8);
    m_noiseComboBox->addItems(noiseValues);
    m_noiseComboBox->setEditable(false);

    m_log->setReadOnly(true);
    m_log->appendPlainText(QStringLiteral("Не подключено к ответчику"));
    m_logClearButton->setText(QStringLiteral("Консоль отчистить"));

        OnSetButtonEnabled(false);

}

void StateWidget::AddButtonFromGroupToLayout(QList<QAbstractButton*>::const_iterator *setButtonIterator, QList<QAbstractButton*>::const_iterator *getButtonIterator, QBoxLayout *layout)
{
    layout->addWidget(**setButtonIterator);
    setButtonIterator->operator++();
    if (getButtonIterator!=nullptr)
    {
        layout->addWidget(**getButtonIterator);
        getButtonIterator->operator++();
    }
}

void StateWidget::ConnectObjects()
{
    connect(m_speedLineEdit, &QLineEdit::textEdited, this, &StateWidget::ChangeDoplerLineEdit);
    connect(m_doplerFreqLineEdit, &QLineEdit::textEdited, this, &StateWidget::ChangeSpeedLineEdit);
    connect(m_logClearButton, &QPushButton::clicked, m_log, &QPlainTextEdit::clear);
    connect(m_statePresenter, &StatePresenter::ToConsoleLog, this, &StateWidget::OnConsoleLog);
    connect(m_statePresenter, &StatePresenter::ToSetButtonsEnabled, this, &StateWidget::OnSetButtonEnabled);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_sendStateButtonsGroup, &QButtonGroup::idClicked, this, &StateWidget::OnSetStateButtonIdClicked);
    connect(m_getStateButtonGroup, &QButtonGroup::idClicked, this, &StateWidget::OnGetStateButtonIdClicked);
#else
    connect(m_sendStateButtonsGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &StateWidget::OnSetStateButtonIdClicked);
    connect(m_getStateButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &StateWidget::OnGetStateButtonIdClicked);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(m_noiseComboBox, &QComboBox::currentIndexChanged, [&](int index){this->OnSetStateButtonIdClicked(6);});
#else
    connect(m_noiseComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index){this->OnSetStateButtonIdClicked(6);});
#endif
}

void StateWidget::UpdateHistoryFile()
{
    QString time="\n"+QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
    int space=5;
    QString fvco=m_fvcoComboBox->currentText().leftJustified(space, ' ');
    QString dopler =m_doplerFreqLineEdit->text().leftJustified(space, ' ');
    QString speed = m_speedLineEdit->text().leftJustified(space, ' ');
    QString range= m_rangeLineEdit->text().leftJustified(space, ' ');
    QString gaintTx = m_gainTXLineEdit->text().leftJustified(space, ' ');
    QString gaintRx = m_gainRXLineEdit->text().leftJustified(space, ' ');
    QString attenuator =m_attenuatorComboBox->currentText();
    QString noise=m_noiseComboBox->currentText();
    QString message=time + "|Fvco(МПЦ)= "+fvco+" |Доплер(ГЦ)= "+dopler+ " |Скорость(М/С)= "+speed+ " |Дистанция(М)= "+range+ " |gainTx= "+gaintTx+ " |gainRx= "+gaintRx+ " |Ослабление= "+attenuator + " |"+ noise;
    m_file->write(message.toUtf8());
    m_file->flush();
}


void StateWidget::OnConsoleLog(const QString &message)
{
    QString time=QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
    m_log->appendPlainText(time+ " "+ message);
}

void StateWidget::OnSetStateButtonIdClicked(int id)
{
    double firstValue=0.0, secondValue=0.0;
    switch (id) {
    case 0:
    {
        OnConsoleLog(QStringLiteral("Высылаем сообщение пинга"));
        break;
    }
    case 1:
    {
        if (!m_fvcoComboBox->currentText().isEmpty())
        {
            bool isOk;
            firstValue=m_fvcoComboBox->currentText().toDouble(&isOk);
            if (isOk)
            {
                firstValue=firstValue*1000000;
                OnConsoleLog("Высылаем первое сообщение: установка частоты RX fvco= " +QString::number(firstValue)+ " ГЦ");
                break;
            }
            else
            {
                OnConsoleLog("Не смогли перевесли в число " + m_fvcoComboBox->currentText());
                return;
            }
        }
        else
        {
            OnConsoleLog(QStringLiteral("Заполните пожалуйста поля: Рабочая точка Fvco: для частот Tx и Rx"));
            return;
        }
    }
    case 2:
    {
        if (!m_fvcoComboBox->currentText().isEmpty()&&!m_doplerFreqLineEdit->text().isEmpty())
        {
            bool isOk1, isOk2;
            firstValue=m_fvcoComboBox->currentText().toDouble(&isOk1);
            secondValue=m_doplerFreqLineEdit->text().toDouble(&isOk2);
            if (isOk1)
            {
                firstValue=firstValue*1000000;
                if (isOk2)
                {

                    OnConsoleLog("Высылаем второе сообщение: установка частоты Tx fvco= " +QString::number(firstValue) + " ГЦ и доплер= "+ QString::number(secondValue, 'f')+ " ГЦ");
                    break;
                }
                else
                {
                    OnConsoleLog("Не смогли перевесли в число " + m_doplerFreqLineEdit->text());
                    OnConsoleLog("Высылаем сообщение fvco= "+QString::number(firstValue) + " МГЦ доплер= 0 ГЦ");
                    break;
                }
            }
            else
            {
                OnConsoleLog("Не смогли перевесли в число " + m_fvcoComboBox->currentText());
                return;
            }
        }
        else
        {
            OnConsoleLog(QStringLiteral("Заполните пожалуйста поля: Рабочая точка Fvco: для частот Tx и Rx"));
            return;
        }
    }
    case 3:
    {
        if (!m_rangeLineEdit->text().isEmpty())
        {
            bool isOk;
            firstValue=m_rangeLineEdit->text().toDouble(&isOk);
            if (isOk)
            {
                OnConsoleLog("Высылаем третье сообщение: установка дальности ответного сигнала. Дистанция= " + QString::number(firstValue, 'f') );
                break;
            }
            else
            {
                OnConsoleLog("Не смогли перевесли в число " + m_rangeLineEdit->text());
                return;
            }
        }
        else
        {
            OnConsoleLog(QStringLiteral("Заполните пожалуйста поля: Дальность ответного сигнала: d"));
            return;
        }
    }
    case 4:
    {
        if (!m_gainTXLineEdit->text().isEmpty() &&!m_gainRXLineEdit->text().isEmpty())
        {
            bool isOk1, isOk2;
            firstValue=m_gainTXLineEdit->text().toDouble(&isOk1);
            secondValue=m_gainRXLineEdit->text().toDouble(&isOk2);
            if (isOk1&&isOk2)
            {
                OnConsoleLog("Высылаем четвертое сообщение: установка усиления. Tx= " + QString::number(firstValue, 'f') + " Rx= " + QString::number(secondValue, 'f'));
                break;
            }
            else
            {
                OnConsoleLog("Не смогли перевесли в число " + m_gainRXLineEdit->text() + " или  "+ m_gainTXLineEdit->text());
                return;
            }
        }
        else
        {
            OnConsoleLog(QStringLiteral("Заполните пожалуйста поля: Усиление RX а так же Усиление TX"));
            return;
        }
    }
    case 5:
    {
        bool isOk;
        firstValue=m_attenuatorComboBox->currentText().toDouble(&isOk);
        if (isOk)
        {
            OnConsoleLog(QStringLiteral("Высылаем пятое сообщение: установка ослабления"));
        }
        else
        {
            OnConsoleLog("Не смогли перевесли в число " + m_attenuatorComboBox->currentText());
        }
        break;
    }
    case 6:
    {
        firstValue=m_noiseComboBox->currentIndex();
        if (firstValue==4 || firstValue==3)
        {
            bool isOk;
            secondValue=m_noiseLineEdit->text().toDouble(&isOk);
            if (isOk)
            {

                OnConsoleLog("Шестое сообщение: параметр равен: "+ m_noiseLineEdit->text());
            }
            else
            {
                OnConsoleLog("Не смогли перевесли в число: " + m_noiseLineEdit->text() + ".Берем значение 0");
                secondValue=0.0;
                break;
            }
        }
        OnConsoleLog(QStringLiteral("Высылаем шестое сообщение: установка шума"));
        break;
    }
    case 21:
    {
        if (!m_fvcoComboBox->currentText().isEmpty()&&!m_doplerFreqLineEdit->text().isEmpty())
        {
            bool isOk1, isOk2;
            firstValue=m_fvcoComboBox->currentText().toDouble(&isOk1);
            secondValue=m_doplerFreqLineEdit->text().toDouble(&isOk2);
            if (isOk1)
            {
                firstValue=firstValue*1000000;
                if (isOk2)
                {

                    OnConsoleLog("Высылаем второе и первое сообщения: установка частоты Tx и Rх fvco= " +QString::number(firstValue) + " ГЦ и доплер= "+ QString::number(secondValue, 'f')+ " ГЦ");
                }
                else
                {
                    OnConsoleLog("Не смогли перевесли в число " + m_doplerFreqLineEdit->text());
                    OnConsoleLog("Высылаем второе и первое сообщения: fvco= "+QString::number(firstValue) + " МГЦ доплер= 0 ГЦ");
                }
                id=1;
                QTimer::singleShot(7, [&](){    m_statePresenter->SetStateToDevice(2, firstValue, secondValue);});
            }
            else
            {
                OnConsoleLog("Не смогли перевесли в число " + m_fvcoComboBox->currentText());
                return;
            }
        }
        else
        {
            OnConsoleLog(QStringLiteral("Заполните пожалуйста поля: Рабочая точка Fvco: для частот Tx и Rx"));
            return;
        }
        break;
    }
    default:
    {
        OnConsoleLog(QStringLiteral("Обработка не написана void CommandWidget::buttonIdClicked"));
        return;
    }
    }
    m_statePresenter->SetStateToDevice(id, firstValue, secondValue);
    UpdateHistoryFile();
}

void StateWidget::OnGetStateButtonIdClicked(int id)
{
    switch (id) {
    case 1:
    {
        OnConsoleLog(QStringLiteral("Получаем рабочую точку"));
        break;
    }
    case 21:
    {
        OnConsoleLog(QStringLiteral("Получаем доплера точку"));
        id=1;
         QTimer::singleShot(7, [&](){    m_statePresenter->GetStateFromDevice(2);});
        break;
    }
    case 3:
    {
        OnConsoleLog(QStringLiteral("Получаем дальность"));
        break;
    }
    case 4:
    {
        OnConsoleLog(QStringLiteral("Получаем усиление"));
        break;
    }
    case 5:
    {
        OnConsoleLog(QStringLiteral("Получаем ослабление"));
        break;
    }
    case 6:
    {
        OnConsoleLog(QStringLiteral("Получаем режим работы"));
        break;
    }
    default:
    {
        OnConsoleLog(QStringLiteral("Обработка команды не реализована"));
        return;
    }
    }
    m_statePresenter->GetStateFromDevice(id);
}

void StateWidget::OnSetButtonEnabled(bool state)
{
    m_attenuatorComboBox->setEnabled(state);
    m_noiseComboBox->setEnabled(state);
    QList<QAbstractButton*> buttonsSetState=m_sendStateButtonsGroup->buttons();
    for (auto iter=buttonsSetState.begin(); iter!=buttonsSetState.end();++iter ) {
        (*iter)->setEnabled(state);
    }
    QList<QAbstractButton*> buttonsGetState=m_getStateButtonGroup->buttons();
    for (auto iter=buttonsGetState.begin(); iter!=buttonsGetState.end();++iter ) {
        (*iter)->setEnabled(state);
    }
}

void StateWidget::ChangeSpeedLineEdit(const QString &doplerText)
{
    double dopler=doplerText.toDouble();
    double speed=dopler*c/(2.0*m_fvcoComboBox->currentText().toDouble()*1000000.0);
    m_speedLineEdit->setText(QString::number(speed, 'f'));
}

void StateWidget::ChangeDoplerLineEdit(const QString &speedText)
{
    double speed=speedText.toDouble();
    double dopler1=speed*m_fvcoComboBox->currentText().toDouble()*1000000.0*2.0;
    double dopler=dopler1/c;
    m_doplerFreqLineEdit->setText(QString::number(dopler, 'f'));
}
