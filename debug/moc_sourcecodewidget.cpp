/****************************************************************************
** Meta object code from reading C++ file 'sourcecodewidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../ui/widgets/sourcecodewidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sourcecodewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SourceCodeWidget_t {
    QByteArrayData data[9];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SourceCodeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SourceCodeWidget_t qt_meta_stringdata_SourceCodeWidget = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SourceCodeWidget"
QT_MOC_LITERAL(1, 17, 5), // "Flash"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 30), // "StartReadingFirmWareFromDevice"
QT_MOC_LITERAL(4, 55, 32), // "SendFirmwareSourcePartFromDevice"
QT_MOC_LITERAL(5, 88, 11), // "QByteArray&"
QT_MOC_LITERAL(6, 100, 8), // "firmware"
QT_MOC_LITERAL(7, 109, 20), // "LoadFirmWareFromFile"
QT_MOC_LITERAL(8, 130, 19) // "ClearFirmWareSource"

    },
    "SourceCodeWidget\0Flash\0\0"
    "StartReadingFirmWareFromDevice\0"
    "SendFirmwareSourcePartFromDevice\0"
    "QByteArray&\0firmware\0LoadFirmWareFromFile\0"
    "ClearFirmWareSource"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SourceCodeWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   41,    2, 0x0a /* Public */,
       7,    0,   44,    2, 0x08 /* Private */,
       8,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SourceCodeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SourceCodeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Flash(); break;
        case 1: _t->StartReadingFirmWareFromDevice(); break;
        case 2: _t->SendFirmwareSourcePartFromDevice((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 3: _t->LoadFirmWareFromFile(); break;
        case 4: _t->ClearFirmWareSource(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SourceCodeWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SourceCodeWidget::Flash)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SourceCodeWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SourceCodeWidget::StartReadingFirmWareFromDevice)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SourceCodeWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SourceCodeWidget.data,
    qt_meta_data_SourceCodeWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SourceCodeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SourceCodeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SourceCodeWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SourceCodeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SourceCodeWidget::Flash()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SourceCodeWidget::StartReadingFirmWareFromDevice()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE