#include "firmwarecommansmaker.h"
#include <QDebug>
#include <QDataStream>
#include <QIODevice>
#include <iterator>



FirmwareMessageMaker::FirmwareMessageMaker()
    : m_readRegisterStatusCommand(new QByteArray("\x0b\x05\x0a\x00", 4))
    , m_readFromBufferCommand(new QByteArray(std::begin<char>({0x0d, 0x00}), 2))
    , m_flashIdCommad(new QByteArray("\x0b\x09\x0a\x02", 4))
    , m_eraseCommand(new QByteArray("\x0b\xc7\x0a\x00", 4))
    , m_makeWriteOnlyBufferCommand(new QByteArray("\x0b\x06\x0a\x00", 4))
    , m_makeReadOnlyBufferCommand(new QByteArray("\x0b\x04\x0a\x00", 4))
    , m_restartHardwareCommand(new QByteArray("\x0d\x5a", 2))
    , m_readToBufferCommand(new QByteArray("\x0b\x03\x0c\x00\x00\x00\x0a\x00", 8))
    , m_commands(new QVector<quint8>({10, 11, 12, 13, 14}))
{

}

FirmwareMessageMaker::~FirmwareMessageMaker()
{
    delete m_readRegisterStatusCommand;
    delete m_readFromBufferCommand;
    delete m_flashIdCommad;
    delete m_commands;
}

QByteArray* FirmwareMessageMaker::ReadToBufferCommand(quint32 adress, quint8 lenght)
{
    m_readToBufferCommand->operator[](3)=(adress >> (8*0)) & 0xff;
    m_readToBufferCommand->operator[](4)=(adress >> (8*1)) & 0xff;
    m_readToBufferCommand->operator[](5)=(adress >> (8*2)) & 0xff;
    m_readToBufferCommand->operator[](7)=lenght;
    return m_readToBufferCommand;
}

QByteArray FirmwareMessageMaker::WriteBufferToFlashCommand(quint32 adress, quint16 lenght)
{
    QByteArray result;
    result.append(CreateTypeCommand((quint8)2));
    result.append(CreateAddressCommand(adress));
    result.append(CreateNumberOfBytesLenghtCommand(lenght));
    return result;
}

QByteArray FirmwareMessageMaker::WriteDataToBufferCommand(bool isNewPage, const QByteArray &data)
{
    if(data.length()>16)
    {
        qFatal("Длина сообщения в буфере не может быть больше 16 байт");
    }
    else
    {
        quint8 messageLenght=data.length()-1;
        if(isNewPage)
        {
            messageLenght=messageLenght|m_significantBit;
        }
        QByteArray result;
        result.append(0x0e);
        result.append((quint8)(messageLenght));
        result.append(data);
        return result;
    }
}

QByteArray FirmwareMessageMaker::CreateTypeCommand(quint8 commandId)
{
    QByteArray result;
    result.append(0x0b);
    result.append(commandId);
    return result;
}

QByteArray FirmwareMessageMaker::CreateAddressCommand(quint32 adress)
{
    quint8 first=(adress >> (8*0)) & 0xff;
    quint8 second=(adress >> (8*1)) & 0xff;
    quint8 third=(adress >> (8*2)) & 0xff;
    QByteArray result;
    QDataStream streamMain(&result, QIODevice::WriteOnly);
    streamMain << (quint8)m_commands->at(2);
    streamMain << first;
    streamMain << second;
    streamMain << third;
    return result;
}

QByteArray FirmwareMessageMaker::CreateNumberOfBytesLenghtCommand(quint16 numberOfBytes)
{
    QByteArray result;
    result.append(0x0a);
    result.append(quint8(numberOfBytes-1));
    return result;
}
