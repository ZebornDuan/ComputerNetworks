/****************************************************************************
** Meta object code from reading C++ file 'gomoku.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../gomoku.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gomoku.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Gomoku_t {
    QByteArrayData data[20];
    char stringdata[199];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Gomoku_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Gomoku_t qt_meta_stringdata_Gomoku = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Gomoku"
QT_MOC_LITERAL(1, 7, 8), // "addChess"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 3), // "row"
QT_MOC_LITERAL(4, 21, 3), // "col"
QT_MOC_LITERAL(5, 25, 9), // "startGame"
QT_MOC_LITERAL(6, 35, 15), // "timeLimitExceed"
QT_MOC_LITERAL(7, 51, 4), // "undo"
QT_MOC_LITERAL(8, 56, 4), // "load"
QT_MOC_LITERAL(9, 61, 2), // "id"
QT_MOC_LITERAL(10, 64, 23), // "on_createButton_clicked"
QT_MOC_LITERAL(11, 88, 24), // "on_connectButton_clicked"
QT_MOC_LITERAL(12, 113, 8), // "readData"
QT_MOC_LITERAL(13, 122, 13), // "sendChessInfo"
QT_MOC_LITERAL(14, 136, 9), // "chessInfo"
QT_MOC_LITERAL(15, 146, 7), // "sendWin"
QT_MOC_LITERAL(16, 154, 8), // "sendSave"
QT_MOC_LITERAL(17, 163, 11), // "requestQuit"
QT_MOC_LITERAL(18, 175, 11), // "requestUndo"
QT_MOC_LITERAL(19, 187, 11) // "requestLoad"

    },
    "Gomoku\0addChess\0\0row\0col\0startGame\0"
    "timeLimitExceed\0undo\0load\0id\0"
    "on_createButton_clicked\0"
    "on_connectButton_clicked\0readData\0"
    "sendChessInfo\0chessInfo\0sendWin\0"
    "sendSave\0requestQuit\0requestUndo\0"
    "requestLoad"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Gomoku[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   84,    2, 0x06 /* Public */,
       5,    0,   89,    2, 0x06 /* Public */,
       6,    0,   90,    2, 0x06 /* Public */,
       7,    0,   91,    2, 0x06 /* Public */,
       8,    1,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   95,    2, 0x08 /* Private */,
      11,    0,   96,    2, 0x08 /* Private */,
      12,    0,   97,    2, 0x08 /* Private */,
      13,    1,   98,    2, 0x08 /* Private */,
      15,    0,  101,    2, 0x08 /* Private */,
      16,    0,  102,    2, 0x08 /* Private */,
      17,    0,  103,    2, 0x08 /* Private */,
      18,    0,  104,    2, 0x08 /* Private */,
      19,    0,  105,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Gomoku::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Gomoku *_t = static_cast<Gomoku *>(_o);
        switch (_id) {
        case 0: _t->addChess((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->startGame(); break;
        case 2: _t->timeLimitExceed(); break;
        case 3: _t->undo(); break;
        case 4: _t->load((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_createButton_clicked(); break;
        case 6: _t->on_connectButton_clicked(); break;
        case 7: _t->readData(); break;
        case 8: _t->sendChessInfo((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->sendWin(); break;
        case 10: _t->sendSave(); break;
        case 11: _t->requestQuit(); break;
        case 12: _t->requestUndo(); break;
        case 13: _t->requestLoad(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Gomoku::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Gomoku::addChess)) {
                *result = 0;
            }
        }
        {
            typedef void (Gomoku::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Gomoku::startGame)) {
                *result = 1;
            }
        }
        {
            typedef void (Gomoku::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Gomoku::timeLimitExceed)) {
                *result = 2;
            }
        }
        {
            typedef void (Gomoku::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Gomoku::undo)) {
                *result = 3;
            }
        }
        {
            typedef void (Gomoku::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Gomoku::load)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject Gomoku::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Gomoku.data,
      qt_meta_data_Gomoku,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Gomoku::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Gomoku::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Gomoku.stringdata))
        return static_cast<void*>(const_cast< Gomoku*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Gomoku::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Gomoku::addChess(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Gomoku::startGame()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Gomoku::timeLimitExceed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Gomoku::undo()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void Gomoku::load(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
