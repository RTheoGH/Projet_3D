/****************************************************************************
** Meta object code from reading C++ file 'MyViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../src/MyViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MyViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MyViewer_t {
    QByteArrayData data[13];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MyViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MyViewer_t qt_meta_stringdata_MyViewer = {
    {
QT_MOC_LITERAL(0, 0, 8), // "MyViewer"
QT_MOC_LITERAL(1, 9, 18), // "windowTitleUpdated"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 12), // "create_plane"
QT_MOC_LITERAL(4, 42, 9), // "open_mesh"
QT_MOC_LITERAL(5, 52, 9), // "save_mesh"
QT_MOC_LITERAL(6, 62, 12), // "showControls"
QT_MOC_LITERAL(7, 75, 16), // "saveCameraInFile"
QT_MOC_LITERAL(8, 92, 8), // "filename"
QT_MOC_LITERAL(9, 101, 18), // "openCameraFromFile"
QT_MOC_LITERAL(10, 120, 10), // "openCamera"
QT_MOC_LITERAL(11, 131, 10), // "saveCamera"
QT_MOC_LITERAL(12, 142, 20) // "saveSnapShotPlusPlus"

    },
    "MyViewer\0windowTitleUpdated\0\0create_plane\0"
    "open_mesh\0save_mesh\0showControls\0"
    "saveCameraInFile\0filename\0openCameraFromFile\0"
    "openCamera\0saveCamera\0saveSnapShotPlusPlus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   67,    2, 0x0a /* Public */,
       4,    0,   68,    2, 0x0a /* Public */,
       5,    0,   69,    2, 0x0a /* Public */,
       6,    0,   70,    2, 0x0a /* Public */,
       7,    1,   71,    2, 0x0a /* Public */,
       9,    1,   74,    2, 0x0a /* Public */,
      10,    0,   77,    2, 0x0a /* Public */,
      11,    0,   78,    2, 0x0a /* Public */,
      12,    0,   79,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MyViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyViewer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->windowTitleUpdated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->create_plane(); break;
        case 2: _t->open_mesh(); break;
        case 3: _t->save_mesh(); break;
        case 4: _t->showControls(); break;
        case 5: _t->saveCameraInFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->openCameraFromFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->openCamera(); break;
        case 8: _t->saveCamera(); break;
        case 9: _t->saveSnapShotPlusPlus(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MyViewer::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyViewer::windowTitleUpdated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MyViewer::staticMetaObject = { {
    QMetaObject::SuperData::link<QGLViewer::staticMetaObject>(),
    qt_meta_stringdata_MyViewer.data,
    qt_meta_data_MyViewer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MyViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyViewer.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions_4_3_Core"))
        return static_cast< QOpenGLFunctions_4_3_Core*>(this);
    return QGLViewer::qt_metacast(_clname);
}

int MyViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLViewer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void MyViewer::windowTitleUpdated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
