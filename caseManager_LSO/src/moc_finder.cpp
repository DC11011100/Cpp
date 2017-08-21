/****************************************************************************
** Meta object code from reading C++ file 'finder.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../DataMan/finder.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'finder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CaseFinder_t {
    QByteArrayData data[8];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CaseFinder_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CaseFinder_t qt_meta_stringdata_CaseFinder = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CaseFinder"
QT_MOC_LITERAL(1, 11, 14), // "clientSearched"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 7), // "results"
QT_MOC_LITERAL(4, 35, 12), // "searchClient"
QT_MOC_LITERAL(5, 48, 16), // "updateClientList"
QT_MOC_LITERAL(6, 65, 14), // "updateCaseList"
QT_MOC_LITERAL(7, 80, 6) // "client"

    },
    "CaseFinder\0clientSearched\0\0results\0"
    "searchClient\0updateClientList\0"
    "updateCaseList\0client"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CaseFinder[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   37,    2, 0x0a /* Public */,
       5,    1,   38,    2, 0x0a /* Public */,
       6,    1,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void CaseFinder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CaseFinder *_t = static_cast<CaseFinder *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clientSearched((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 1: _t->searchClient(); break;
        case 2: _t->updateClientList((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 3: _t->updateCaseList((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CaseFinder::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CaseFinder::clientSearched)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject CaseFinder::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CaseFinder.data,
      qt_meta_data_CaseFinder,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CaseFinder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CaseFinder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CaseFinder.stringdata0))
        return static_cast<void*>(const_cast< CaseFinder*>(this));
    return QDialog::qt_metacast(_clname);
}

int CaseFinder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CaseFinder::clientSearched(QStringList _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
