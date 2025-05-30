/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <KAuth/ActionReply>
using namespace KAuth;

class Helper : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    ActionReply saveunitfile(const QVariantMap &args);
    ActionReply dbusaction(const QVariantMap &args);
};
