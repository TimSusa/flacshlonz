/****************************************************************************
** Meta object code from reading C++ file 'AlbumDataModel.hpp'
**
** Created: Sun Jul 20 12:08:35 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "AlbumDataModel.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AlbumDataModel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AlbumDataModel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   16,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AlbumDataModel[] = {
    "AlbumDataModel\0\0enabled\0"
    "slotDeepScanEnabled(bool)\0"
};

void AlbumDataModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AlbumDataModel *_t = static_cast<AlbumDataModel *>(_o);
        switch (_id) {
        case 0: _t->slotDeepScanEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData AlbumDataModel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AlbumDataModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AlbumDataModel,
      qt_meta_data_AlbumDataModel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AlbumDataModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AlbumDataModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AlbumDataModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AlbumDataModel))
        return static_cast<void*>(const_cast< AlbumDataModel*>(this));
    if (!strcmp(_clname, "FlacMetadataManager"))
        return static_cast< FlacMetadataManager*>(const_cast< AlbumDataModel*>(this));
    return QObject::qt_metacast(_clname);
}

int AlbumDataModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
