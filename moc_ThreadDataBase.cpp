/****************************************************************************
** Meta object code from reading C++ file 'ThreadDataBase.hpp'
**
** Created: Sun Aug 3 17:53:11 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ThreadDataBase.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ThreadDataBase.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ThreadDataBase[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   15,   15,   15, 0x0a,
      62,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ThreadDataBase[] = {
    "ThreadDataBase\0\0wasSuccessful\0"
    "sigRunFinished(bool)\0slotStop()\0"
    "slotResetProgressCount()\0"
};

void ThreadDataBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ThreadDataBase *_t = static_cast<ThreadDataBase *>(_o);
        switch (_id) {
        case 0: _t->sigRunFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->slotStop(); break;
        case 2: _t->slotResetProgressCount(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ThreadDataBase::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ThreadDataBase::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ThreadDataBase,
      qt_meta_data_ThreadDataBase, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ThreadDataBase::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ThreadDataBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ThreadDataBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ThreadDataBase))
        return static_cast<void*>(const_cast< ThreadDataBase*>(this));
    return QThread::qt_metacast(_clname);
}

int ThreadDataBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ThreadDataBase::sigRunFinished(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
