#include "firmwarepresenter.h"

#include <QElapsedTimer>


FirmwarePresenter::FirmwarePresenter(const QMap<QString, quint8> *actions, QObject *parent)
    : QObject(parent)
    , m_dataHandler(Q_NULLPTR)
    , m_maxMessageSize(256)
    , m_maxBufferBytesCount(16)
    , m_bufferCounts(m_maxMessageSize/m_maxBufferBytesCount)
    , m_totalCountOfPages(0)
    , m_currentPage(0)
    , m_currentAdress(0)
    , m_actions(actions)
{
    CreateObjects();
    InitObjects();
    ConnectObjects();
}

FirmwarePresenter::~FirmwarePresenter()
{
    delete m_timer;
    delete m_writingFirmwareTimer;
    delete m_erasingTimer;

    delete m_firmwareMessageMaker;
    delete m_commandsQueue;
    delete m_writinFirmwareCommandsList;
}

void FirmwarePresenter::CreateObjects()
{
    m_timer=new QTimer(this);
    m_writingFirmwareTimer=new QTimer(this);
    m_erasingTimer=new QTimer(this);
    m_firmwareMessageMaker=new FirmwareMessageMaker();
    m_commandsQueue=new QQueue<QByteArray>();
#if QT_VERSION <QT_VERSION_CHECK(6, 0, 0)
    m_writinFirmwareCommandsList=new QLinkedList<QByteArray>();
#else
    m_writinFirmwareCommandsList=new QList<QByteArray>();
#endif
    m_firmwareFromFile=Q_NULLPTR;
}

void FirmwarePresenter::InitObjects()
{
    m_timer->setInterval(3);
    m_timer->setTimerType(Qt::PreciseTimer);
    m_writingFirmwareTimer->setInterval(10);
    m_writingFirmwareTimer->setTimerType(Qt::PreciseTimer);
    m_erasingTimer->setInterval(1000);
    m_timer->start();
}

void FirmwarePresenter::ConnectObjects()
{
    connect(m_timer, &QTimer::timeout, this, &FirmwarePresenter::OnTimerTimeout);
    connect(m_writingFirmwareTimer, &QTimer::timeout, this, &FirmwarePresenter::OnWritingTimerTimeOut);
    connect(m_erasingTimer, &QTimer::timeout, this, &FirmwarePresenter::OnErasingTimerTimeOut);
}

void FirmwarePresenter::SendMessageToQueue(quint8 command, quint32 adress, quint8 lenght)
{
    switch (command) {
    case 0:
    {
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_restartHardwareCommand);
        break;
    }
    case 3:
    {
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->ReadToBufferCommand(adress, lenght));
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_readFromBufferCommand);
        break;
    }
    case 4:
    {
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_restartHardwareCommand);
        break;
    }
    case 5:
    {
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_readRegisterStatusCommand);
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_readFromBufferCommand);
        break;
    }
    case 6:
    {
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_makeWriteOnlyBufferCommand);
        break;
    }
    case 9:
    {
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_flashIdCommad);
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_readFromBufferCommand);
        break;
    }
    case 199:
    {
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_makeWriteOnlyBufferCommand);
        m_commandsQueue->enqueue(*m_firmwareMessageMaker->m_eraseCommand);
        break;
    }
    default:
    {
        qFatal("Нет команды");
    }

    }
}



void FirmwarePresenter::FillFullPageIntoBuffer(QByteArray *partOfFirmware)
{
    QByteArray by=partOfFirmware->mid(0, m_maxBufferBytesCount);
    m_writinFirmwareCommandsList->push_back(m_firmwareMessageMaker->WriteDataToBufferCommand(true, by));
    int currentSubByteArrayIndex=m_maxBufferBytesCount;
    for (int i=1; i<m_bufferCounts; i++)
    {
        by=partOfFirmware->mid(currentSubByteArrayIndex, m_maxBufferBytesCount);
        m_writinFirmwareCommandsList->push_back(m_firmwareMessageMaker->WriteDataToBufferCommand(false, by));
        currentSubByteArrayIndex+=m_maxBufferBytesCount;
    }
}

void FirmwarePresenter::FillLastPageIntoBuffer(QByteArray *partOfFirmware)
{
    if (partOfFirmware->count()<m_maxBufferBytesCount)
    {
        m_writinFirmwareCommandsList->push_back(m_firmwareMessageMaker->WriteDataToBufferCommand(true, *partOfFirmware));
    }
    else
    {
        QByteArray by=partOfFirmware->mid(0, m_maxBufferBytesCount);
        m_writinFirmwareCommandsList->push_back(m_firmwareMessageMaker->WriteDataToBufferCommand(true, by));
        int currentSubByteArrayIndex=m_maxBufferBytesCount;
        while (true) {
            if (currentSubByteArrayIndex+m_maxBufferBytesCount<partOfFirmware->count())
            {
                by=partOfFirmware->mid(currentSubByteArrayIndex, m_maxBufferBytesCount);
                m_writinFirmwareCommandsList->push_back(m_firmwareMessageMaker->WriteDataToBufferCommand(false, by));
                currentSubByteArrayIndex=currentSubByteArrayIndex+m_maxBufferBytesCount;
            }
            else
            {
                by= partOfFirmware->mid(currentSubByteArrayIndex);
                m_writinFirmwareCommandsList->push_back(m_firmwareMessageMaker->WriteDataToBufferCommand(false, by));
                break;
            }
        }
    }
}

void FirmwarePresenter::OnFlash(QByteArray *firmwareFromFile)
{
    m_currentPage=0;
    m_currentAdress=0;
    m_isPcbBisy=true;
    m_firmwareFromFile=firmwareFromFile;
    m_timer->stop();
    Q_EMIT ToWidgetsEnable(false);
    m_dataHandler->SendMessageToDevice(*m_firmwareMessageMaker->m_makeWriteOnlyBufferCommand);
    m_writinFirmwareCommandsList->clear();
    m_dataHandler->SetHandlerState(HandlerState::Flash);
    m_dataHandler->SendMessageToDevice(*m_firmwareMessageMaker->m_eraseCommand);
    Q_EMIT ToConsoleLog(QStringLiteral("Послали запрос на форматирование"));

    Q_EMIT ToConsoleLog(QStringLiteral("Начали формировать страницы"));
    m_pagesOfFirmware=GenerateFirmwarePages(firmwareFromFile);
    Q_EMIT ToConsoleLog(QStringLiteral("Сформировали страницы. Формируем команды"));
    PrepareCommandsToFlash(m_pagesOfFirmware);
    Q_EMIT ToConsoleLog(QStringLiteral("Формирование команды готово. Форматируемся..."));
    m_erasingTimer->start();

}

void FirmwarePresenter::OnStartReadingFirmWareFromDevice()
{
    //    m_elapsedTimer.start();
    Q_EMIT ToConsoleLog(QStringLiteral("Начали читать страницы с устройства"));
    m_currentPage=0;
    m_currentAdress=0;
    m_dataHandler->SetHandlerState(HandlerState::ReadFirmware);
    OnReadFirmwareAgain();
}

void FirmwarePresenter::OnFirmwareFromDeviceLoaded(QByteArray *firmwareFromDevice)
{
    if ((*m_firmwareFromFile)==(*firmwareFromDevice))
    {
        Q_EMIT ToConsoleLog(QStringLiteral("Верификация файла прошла успешно, прошивки совпадают"));
    }
    else
    {
        Q_EMIT ToConsoleLog(QStringLiteral("Верификация файла прошла не успешно. Надо перешится."));
    }
}

void FirmwarePresenter::OnTimerTimeout()
{
    if(!m_commandsQueue->isEmpty())
    {
        QByteArray commandToExecute(m_commandsQueue->dequeue());
        Q_EMIT ToConsoleLog("Выслали команду  "+QString::fromLatin1(commandToExecute.toHex()));
        m_dataHandler->SendMessageToDevice(commandToExecute);
    }
}

void FirmwarePresenter::OnReadFirmwareAgain()
{
    Q_EMIT ToConsoleLog("Считываем страницу: " + QString::number(m_currentPage));
    qDebug()<< m_currentPage;
    m_currentPage++;
    m_dataHandler->SendMessageToDevice(*m_firmwareMessageMaker->ReadToBufferCommand(m_currentAdress, m_maxMessageSize-1));
    SleepMiliseconds(3);
    m_dataHandler->SendMessageToDevice(*m_firmwareMessageMaker->m_readFromBufferCommand);
    m_currentAdress=m_currentAdress+m_maxMessageSize;
    Q_EMIT ToConsoleLog("новый адрес: " + QString::number(m_currentAdress));
}

void FirmwarePresenter::OnWritingTimerTimeOut()
{
    if(m_currentPage==m_totalCountOfPages)//list
    {
        m_writingFirmwareTimer->stop();
        m_erasingTimer->stop();
        Q_EMIT ToWidgetsEnable(true);
        m_timer->start();
    }
    else
    {
        if (m_currentPage<m_totalCountOfPages-1)
        {
            for (int i=0; i<18; ++i)
            {
                m_dataHandler->WriteMessageToBuffer(*m_writingFirmwareCommandsListIterator);
                ++m_writingFirmwareCommandsListIterator;
            }
            m_dataHandler->FlushBuffer();
            SleepMiliseconds(35);
        }
        else
        {
            while( m_writingFirmwareCommandsListIterator!=m_writinFirmwareCommandsList->end())
            {
                m_dataHandler->WriteMessageToBuffer(*m_writingFirmwareCommandsListIterator);
                ++m_writingFirmwareCommandsListIterator;
            }
            m_dataHandler->FlushBuffer();
        }
        Q_EMIT ToConsoleLog("Записана страница "+ QString::number(m_currentPage));
        ++m_currentPage;
        Q_EMIT ToProgressBarUpdate(m_currentPage);
    }
}

void FirmwarePresenter::OnErasingTimerTimeOut()
{
    Q_EMIT ToConsoleLog(QStringLiteral("Проверяем состояние регистров после форматирования:"));
    if (m_isPcbBisy)
    {
        //        m_isPcbBisy=false;
        Q_EMIT ToConsoleLog(QStringLiteral("Занят. Форматируется еще..."));
        Q_EMIT ToConsoleLog(QStringLiteral("Высылаем еще раз запрос на проверку состояния"));
        m_dataHandler->SendMessageToDevice(*m_firmwareMessageMaker->m_readRegisterStatusCommand);
        m_dataHandler->SendMessageToDevice(*m_firmwareMessageMaker->m_readFromBufferCommand);
    }
    else
    {
        Q_EMIT ToConsoleLog(QStringLiteral("Свободен. Шьемся"));
        m_erasingTimer->stop();
        m_writingFirmwareTimer->start();
    }
}

void FirmwarePresenter::OnTakedHardwareState(quint8 state)
{
    Q_EMIT ToConsoleLog("Состояние= "+ QString::number(state));
    if(state==0)
    {
        m_isPcbBisy=false;
    }
    else
    {
        m_isPcbBisy=true;
    }

}

void FirmwarePresenter::DisconnectOldHandler()
{
    m_commandsQueue->clear();
    disconnect(m_dataHandler, &DataHandler::ToFirmwareWidgetConsoleLog, this, &FirmwarePresenter::ToConsoleLog);
    disconnect(m_dataHandler, &DataHandler::ToConsoleLog, this, &FirmwarePresenter::ToConsoleLog);
    disconnect(m_dataHandler, &DataHandler::ToReadFirmwareAgain, this, &FirmwarePresenter::OnReadFirmwareAgain);
    disconnect(m_dataHandler, &DataHandler::ToButtonsEnabledChanging, this, &FirmwarePresenter::ToSetButtonsEnabled);
    disconnect(m_dataHandler, &DataHandler::ToRegisterStateChanging, this, &FirmwarePresenter::OnTakedHardwareState);
    disconnect(m_dataHandler, &DataHandler::ToFirmWareFormDeviceLoaded, this, &FirmwarePresenter::OnFirmwareFromDeviceLoaded);
}

void FirmwarePresenter::ConnectHander(DataHandler *dataHandler)
{
    m_dataHandler=dataHandler;
    connect(m_dataHandler, &DataHandler::ToFirmwareWidgetConsoleLog, this, &FirmwarePresenter::ToConsoleLog);
    connect(m_dataHandler, &DataHandler::ToConsoleLog, this, &FirmwarePresenter::ToConsoleLog);
    connect(m_dataHandler, &DataHandler::ToReadFirmwareAgain, this, &FirmwarePresenter::OnReadFirmwareAgain);
    connect(m_dataHandler, &DataHandler::ToButtonsEnabledChanging, this, &FirmwarePresenter::ToSetButtonsEnabled);
    connect(m_dataHandler, &DataHandler::ToRegisterStateChanging, this, &FirmwarePresenter::OnTakedHardwareState);
    connect(m_dataHandler, &DataHandler::ToFirmWareFormDeviceLoaded, this, &FirmwarePresenter::OnFirmwareFromDeviceLoaded);
}

void FirmwarePresenter::GetDataFromWidget(const QString &id, const QString &adress, const QString &lenght)
{
    m_dataHandler->SetHandlerState(HandlerState::ReadFirmware);
    quint8 idVal=m_actions->value(id);
    quint32 adressVal=adress.toUInt();
    quint8 lenghtVal=lenght.toUInt();
    SendMessageToQueue(idVal, adressVal, lenghtVal);
}

const QByteArray FirmwarePresenter::GetPartOfFirmwareFromArray(quint32 currentIndex, QByteArray *firmwareFromFile)
{
    if (currentIndex+m_maxMessageSize<m_firmwareSize)
    {
        return firmwareFromFile->mid(currentIndex, m_maxMessageSize);
    }
    else
    {
        return firmwareFromFile->mid(currentIndex);
    }
}


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QLinkedList<QByteArray> FirmwarePresenter::GenerateFirmwarePages(QByteArray *firmwareFromFile)
{
    m_firmwareSize=firmwareFromFile->size();
    QLinkedList<QByteArray> pagesOfFirmware;
    quint32 currentAdress=0;
    while(true)
    {
        const QByteArray partOfPage=GetPartOfFirmwareFromArray(currentAdress, firmwareFromFile);
        if(partOfPage.isEmpty())
        {
            break;
        }
        else
        {
            currentAdress+=partOfPage.length();
            pagesOfFirmware.append(partOfPage);
        }
    }
    return pagesOfFirmware;
}

void FirmwarePresenter::PrepareCommandsToFlash(QLinkedList<QByteArray> &pagesOfFirmware)
{
    Q_EMIT ToConsoleLog(QStringLiteral("Делаем команды"));
    quint32 currentAdress=0;
    for(QLinkedList<QByteArray>::iterator it=pagesOfFirmware.begin(); it!=pagesOfFirmware.end(); ++it)
    {
        m_writinFirmwareCommandsList->push_back(*m_firmwareMessageMaker->m_makeWriteOnlyBufferCommand);
        if(it->count()==m_maxMessageSize)
        {
            FillFullPageIntoBuffer(&*it);
        }
        else
        {
            FillLastPageIntoBuffer(&*it);
        }
        m_writinFirmwareCommandsList->push_back(m_firmwareMessageMaker->WriteBufferToFlashCommand(currentAdress, it->length()));
        currentAdress+=m_maxMessageSize;
    }
    m_writingFirmwareCommandsListIterator=m_writinFirmwareCommandsList->begin();
    Q_EMIT ToConsoleLog(QStringLiteral("Вычисляем колличество страниц в прошивке"));
    m_totalCountOfPages=pagesOfFirmware.count();
    Q_EMIT ToSetMaximumCountOfPages(m_totalCountOfPages);
    Q_EMIT ToConsoleLog("Страниц в прошивке: "+ QString::number(m_totalCountOfPages));
}

#else

QList<QByteArray> FirmwarePresenter::GenerateFirmwarePages(QByteArray *firmwareFromFile)
{
    m_firmwareSize=firmwareFromFile->size();
    QList<QByteArray> pagesOfFirmware;
    quint32 currentAdress=0;
    while(true)
    {
        const QByteArray partOfPage=GetPartOfFirmwareFromArray(currentAdress, firmwareFromFile);
        if(partOfPage.isEmpty())
        {
            break;
        }
        else
        {
            currentAdress+=partOfPage.length();
            pagesOfFirmware.append(partOfPage);
        }
    }
    return pagesOfFirmware;
}

void FirmwarePresenter::PrepareCommandsToFlash(QList<QByteArray> &pagesOfFirmware)
{
    Q_EMIT ToConsoleLog(QStringLiteral("Делаем команды"));
    quint32 currentAdress=0;
    for(QList<QByteArray>::iterator it=pagesOfFirmware.begin(); it!=pagesOfFirmware.end(); ++it)
    {
        m_writinFirmwareCommandsList->append(*m_firmwareMessageMaker->m_makeWriteOnlyBufferCommand);
        if(it->count()==m_maxMessageSize)
        {
            FillFullPageIntoBuffer(&*it);
        }
        else
        {
            FillLastPageIntoBuffer(&*it);
        }
        m_writinFirmwareCommandsList->append(m_firmwareMessageMaker->WriteBufferToFlashCommand(currentAdress, it->length()));
        currentAdress+=m_maxMessageSize;
    }
    m_writingFirmwareCommandsListIterator=m_writinFirmwareCommandsList->begin();
    Q_EMIT ToConsoleLog(QStringLiteral("Вычисляем колличество страниц в прошивке"));
    m_totalCountOfPages=pagesOfFirmware.count();
    Q_EMIT ToSetMaximumCountOfPages(m_totalCountOfPages);
    Q_EMIT ToConsoleLog("Страниц в прошивке: "+ QString::number(m_totalCountOfPages));
}

#endif

void FirmwarePresenter::SleepMiliseconds(int ms)
{
#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}
