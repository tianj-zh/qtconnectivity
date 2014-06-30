/***************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
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

#ifndef QLOWENERGYDESCRIPTOR_H
#define QLOWENERGYDESCRIPTOR_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVariantMap>
#include <QtBluetooth/qbluetooth.h>
#include <QtBluetooth/QBluetoothUuid>

QT_BEGIN_NAMESPACE

class QLowEnergyDescriptorPrivate;
class QLowEnergyServicePrivate;

class Q_BLUETOOTH_EXPORT QLowEnergyDescriptor
{
    friend class QLowEnergyControllerPrivate;
public:
    QLowEnergyDescriptor();
    QLowEnergyDescriptor(const QLowEnergyDescriptor &other);
    ~QLowEnergyDescriptor();

    QLowEnergyDescriptor &operator=(const QLowEnergyDescriptor &other);
    bool operator==(const QLowEnergyDescriptor &other) const;
    bool operator!=(const QLowEnergyDescriptor &other) const;

    bool isValid() const;

    QByteArray value() const;
    void setValue(const QByteArray &value); //TODO shift to QLowEnergyControllerNew

    QBluetoothUuid uuid() const;
    QLowEnergyHandle handle() const;
    QString name() const;

    QBluetoothUuid::DescriptorType type() const;

protected:
    QSharedPointer<QLowEnergyServicePrivate> d_ptr;

    friend class QLowEnergyCharacteristic;
    QLowEnergyDescriptorPrivate *data;

    QLowEnergyDescriptor(QSharedPointer<QLowEnergyServicePrivate> p,
                             QLowEnergyHandle charHandle,
                             QLowEnergyHandle descHandle);
};

QT_END_NAMESPACE

#endif // QLOWENERGYDESCRIPTOR_H
