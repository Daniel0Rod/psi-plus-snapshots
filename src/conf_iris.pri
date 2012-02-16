include(../conf.pri)
windows:include(../conf_windows.pri)

# don't build iris apps
CONFIG += no_tests

# use qca from psi if necessary
qca-static {
	DEFINES += QCA_STATIC
	INCLUDEPATH += $$PWD/../third-party/qca/qca/include/QtCrypto
}
else {
	CONFIG += crypto
}

psi-winzlib {
    INCLUDEPATH += $$PWD/libpsi/tools/zip/minizip/win32
    DEPENDPATH  += $$PWD/libpsi/tools/zip/minizip/win32
    LIBS += $$PWD/libpsi/tools/zip/minizip/win32/libz.a
}

mac {
	# Universal binaries
	qc_universal:contains(QT_CONFIG,x86):contains(QT_CONFIG,ppc) {
		CONFIG += x86 ppc
		QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.5.sdk
		QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
	}
}