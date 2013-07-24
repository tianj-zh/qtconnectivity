TARGET = QtNfc
QT = core

load(qt_module)

QMAKE_DOCS = $$PWD/doc/qtnfc.qdocconf
OTHER_FILES += doc/src/*.qdoc   # show .qdoc files in Qt Creator

PUBLIC_HEADERS += \
    qnfcglobal.h \
    qnearfieldmanager.h \
    qnearfieldtarget.h \
    qndefrecord.h \
    qndefnfctextrecord.h \
    qndefmessage.h \
    qndeffilter.h \
    qndefnfcurirecord.h \
    qllcpsocket.h \
    qllcpserver.h \
    qdeclarativendefrecord.h \
    qndefnfcsmartposterrecord.h

PRIVATE_HEADERS += \
    qndefrecord_p.h \
    qnearfieldtarget_p.h \
    qnearfieldmanager_p.h \
    qnearfieldtagtype1_p.h \
    qnearfieldtagtype2_p.h \
    qnearfieldtagtype3_p.h \
    qnearfieldtagtype4_p.h \
    qtlv_p.h \
    checksum_p.h \
    qndefnfcsmartposterrecord_p.h

SOURCES += \
    qnearfieldmanager.cpp \
    qnearfieldtarget.cpp \
    qndefrecord.cpp \
    qndefnfctextrecord.cpp \
    qndefmessage.cpp \
    qndeffilter.cpp \
    qndefnfcurirecord.cpp \
    qnearfieldtagtype1.cpp \
    qnearfieldtagtype2.cpp \
    qnearfieldtagtype3.cpp \
    qllcpsocket.cpp \
    qnearfieldtagtype4.cpp \
    qtlv.cpp \
    qllcpserver.cpp \
    qdeclarativendefrecord.cpp \
    qndefnfcsmartposterrecord.cpp

qnx {
    NFC_BACKEND_AVAILABLE = yes
    DEFINES += QNX_NFC #QQNXNFC_DEBUG

    LIBS += -lnfc

    PRIVATE_HEADERS += \
        qllcpserver_qnx_p.h \
        qllcpsocket_qnx_p.h \
        qnearfieldmanager_qnx_p.h \
        qnx/qnxnfcmanager_p.h \
        qnearfieldtarget_qnx_p.h \
        qnx/qnxnfceventfilter_p.h

    SOURCES += \
        qllcpserver_qnx_p.cpp \
        qllcpsocket_qnx_p.cpp \
        qnearfieldmanager_qnx.cpp \
        qnx/qnxnfcmanager.cpp \
        qnx/qnxnfceventfilter.cpp
}

simulator {
    NFC_BACKEND_AVAILABLE = yes

    QT *= gui

    PRIVATE_HEADERS += \
        qnearfieldmanagervirtualbase_p.h \
        qnearfieldmanager_simulator_p.h \
        qllcpsocket_simulator_p.h \
        qllcpserver_simulator_p.h

    SOURCES += \
        qnearfieldmanagervirtualbase.cpp \
        qnearfieldmanager_simulator.cpp \
        qllcpsocket_simulator_p.cpp \
        qllcpserver_simulator_p.cpp
}

isEmpty(NFC_BACKEND_AVAILABLE) {
    message("Unsupported NFC platform, will not build a working QtNfc library.")

    PRIVATE_HEADERS += \
        qllcpsocket_p.h \
        qllcpserver_p.h \
        qnearfieldmanagerimpl_p.h

    SOURCES += \
        qllcpsocket_p.cpp \
        qllcpserver_p.cpp \
        qnearfieldmanagerimpl_p.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
