/***************************************************************************
**
** Copyright (C) 2013 BlackBerry Limited all rights reserved
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QUuid>

#include <QDebug>

#include <QBluetoothServiceDiscoveryAgent>
#include <QLowEnergyDescriptor>
#include <QLowEnergyControllerNew>
#include <QBluetoothLocalDevice>

QT_USE_NAMESPACE

class tst_QLowEnergyDescriptor : public QObject
{
    Q_OBJECT

public:
    tst_QLowEnergyDescriptor();
    ~tst_QLowEnergyDescriptor();

protected slots:
    void serviceDiscovered(const QLowEnergyServiceInfo &info);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void tst_constructionDefault();
    void tst_assignCompare();

private:
    QBluetoothServiceDiscoveryAgent *agent;
    QSet<QString> remoteLeDevices;
    QList<QLowEnergyServiceInfo> foundServices;
    QLowEnergyControllerNew *globalControl;
    QLowEnergyService *globalService;
};

tst_QLowEnergyDescriptor::tst_QLowEnergyDescriptor() :
    globalControl(0), globalService(0)
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
}

tst_QLowEnergyDescriptor::~tst_QLowEnergyDescriptor()
{
}

void tst_QLowEnergyDescriptor::initTestCase()
{
    if (QBluetoothLocalDevice::allDevices().isEmpty()) {
        qWarning("No remote device discovered.");

        return;
    }

    // start Bluetooth if not started
    QBluetoothLocalDevice device;
    device.powerOn();

    // find an arbitrary low energy device in vincinity
    // find an arbitrary service with descriptor

    QBluetoothServiceDiscoveryAgent * agent = new QBluetoothServiceDiscoveryAgent(this);
    connect(agent, SIGNAL(serviceDiscovered(QLowEnergyServiceInfo)),
            SLOT(serviceDiscovered(QLowEnergyServiceInfo)));

    QSignalSpy spy(agent, SIGNAL(finished()));
    // there should be no changes yet
    QVERIFY(spy.isValid());
    QVERIFY(spy.isEmpty());

    agent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 50000);

    // find first service with descriptor
    QLowEnergyControllerNew *controller = 0;
    foreach (const QString &remoteDevice, remoteLeDevices.toList()) {
        controller = new QLowEnergyControllerNew(QBluetoothAddress(remoteDevice), this);
        qDebug() << "Connecting to" << remoteDevice;
        controller->connectToDevice();
        QTRY_IMPL(controller->state() != QLowEnergyControllerNew::ConnectingState,
                  10000);
        if (controller->state() != QLowEnergyControllerNew::ConnectedState) {
            // any error and we skip
            delete controller;
            qDebug() << "Skipping device";
            continue;
        }

        QSignalSpy discoveryFinishedSpy(controller, SIGNAL(discoveryFinished()));
        controller->discoverServices();
        QTRY_VERIFY_WITH_TIMEOUT(discoveryFinishedSpy.count() == 1, 10000);
        foreach (const QBluetoothUuid &leServiceUuid, controller->services()) {
            QLowEnergyService *leService = controller->createServiceObject(leServiceUuid, this);
            if (!leService)
                continue;

            leService->discoverDetails();
            QTRY_VERIFY_WITH_TIMEOUT(
                        leService->state() == QLowEnergyService::ServiceDiscovered, 10000);

            QList<QLowEnergyCharacteristic> chars = leService->characteristics();
            foreach (const QLowEnergyCharacteristic &ch, chars) {
                if (!ch.descriptors().isEmpty()) {
                    globalService = leService;
                    globalControl = controller;
                    qWarning() << "Found service with descriptor" << remoteDevice
                               << globalService->serviceName() << globalService->serviceUuid();
                    break;
                }
            }

            if (globalControl)
                break;
            else
                delete leService;
        }

        if (globalControl)
            break;

        delete controller;
    }

    if (!globalControl) {
        qWarning() << "Test limited due to missing remote QLowEnergyDescriptor."
                   << "Please ensure the Bluetooth Low Energy device is advertising its services.";
    }
}

void tst_QLowEnergyDescriptor::cleanupTestCase()
{
    if (globalControl)
        globalControl->disconnectFromDevice();
}

void tst_QLowEnergyDescriptor::serviceDiscovered(const QLowEnergyServiceInfo &info)
{
    remoteLeDevices.insert(info.device().address().toString());
}

void tst_QLowEnergyDescriptor::tst_constructionDefault()
{
    QLowEnergyDescriptor descriptor;
    QVERIFY(!descriptor.isValid());
    QCOMPARE(descriptor.value(), QByteArray());
    QVERIFY(descriptor.uuid().isNull());
    QVERIFY(descriptor.handle() == 0);
    QCOMPARE(descriptor.name(), QString());
    QCOMPARE(descriptor.type(), QBluetoothUuid::UnknownDescriptorType);

    QLowEnergyDescriptor copyConstructed(descriptor);
    QVERIFY(!copyConstructed.isValid());
    QCOMPARE(copyConstructed.value(), QByteArray());
    QVERIFY(copyConstructed.uuid().isNull());
    QVERIFY(copyConstructed.handle() == 0);
    QCOMPARE(copyConstructed.name(), QString());
    QCOMPARE(copyConstructed.type(), QBluetoothUuid::UnknownDescriptorType);

    QLowEnergyDescriptor assigned;
    assigned = descriptor;
    QVERIFY(!assigned.isValid());
    QCOMPARE(assigned.value(), QByteArray());
    QVERIFY(assigned.uuid().isNull());
    QVERIFY(assigned.handle() == 0);
    QCOMPARE(assigned.name(), QString());
    QCOMPARE(assigned.type(), QBluetoothUuid::UnknownDescriptorType);
}


void tst_QLowEnergyDescriptor::tst_assignCompare()
{
    //find the descriptor
    if (!globalService)
        QSKIP("No descriptor found.");

    QLowEnergyDescriptor target;
    QVERIFY(!target.isValid());
    QCOMPARE(target.type(), QBluetoothUuid::UnknownDescriptorType);
    QCOMPARE(target.name(), QString());
    QCOMPARE(target.handle(), 0u);
    QCOMPARE(target.uuid(), QBluetoothUuid());
    QCOMPARE(target.value(), QByteArray());

    QLowEnergyCharacteristic targetChar;
    QList<QLowEnergyDescriptor> targets;
    const QList<QLowEnergyCharacteristic> chars = globalService->characteristics();
    foreach (const QLowEnergyCharacteristic &ch, chars) {
        if (!ch.descriptors().isEmpty()) {
           targets = ch.descriptors();
           break;
        }
    }

    if (targets.isEmpty())
        QSKIP("No descriptor found despite prior indication.");

    // test assignment operator
    target = targets.first();
    QVERIFY(target.isValid());
    QVERIFY(target.type() != QBluetoothUuid::UnknownDescriptorType);
    QVERIFY(!target.name().isEmpty());
    QVERIFY(target.handle() > 0);
    QVERIFY(!target.uuid().isNull());
    QVERIFY(!target.value().isEmpty());

    QCOMPARE(target.isValid(), targets.first().isValid());
    QCOMPARE(target.type(), targets.first().type());
    QCOMPARE(target.name(), targets.first().name());
    QCOMPARE(target.handle(), targets.first().handle());
    QCOMPARE(target.uuid(), targets.first().uuid());
    QCOMPARE(target.value(), targets.first().value());
}

QTEST_MAIN(tst_QLowEnergyDescriptor)

#include "tst_qlowenergydescriptor.moc"
