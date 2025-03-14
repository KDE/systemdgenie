// This file was generated by a modified version of qdbusxml2cpp
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef SYSTEMD_TIMER_INTERFACE_H
#define SYSTEMD_TIMER_INTERFACE_H

#include "systemd_unit_interface.h"

/*
 * Proxy class for interface org.freedesktop.systemd1.Timer
 */
class OrgFreedesktopSystemd1TimerInterface : public OrgFreedesktopSystemd1UnitInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "org.freedesktop.systemd1.Timer";
    }

public:
    OrgFreedesktopSystemd1TimerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~OrgFreedesktopSystemd1TimerInterface();

    Q_PROPERTY(qulonglong AccuracyUSec READ accuracyUSec NOTIFY accuracyUSecChanged)
    inline qulonglong accuracyUSec() const
    {
        return qvariant_cast<qulonglong>(property("AccuracyUSec"));
    }
    Q_SIGNAL void accuracyUSecChanged();

    Q_PROPERTY(bool FixedRandomDelay READ fixedRandomDelay NOTIFY fixedRandomDelayChanged)
    inline bool fixedRandomDelay() const
    {
        return qvariant_cast<bool>(property("FixedRandomDelay"));
    }
    Q_SIGNAL void fixedRandomDelayChanged();

    Q_PROPERTY(qulonglong LastTriggerUSec READ lastTriggerUSec NOTIFY lastTriggerUSecChanged)
    inline qulonglong lastTriggerUSec() const
    {
        return qvariant_cast<qulonglong>(property("LastTriggerUSec"));
    }
    Q_SIGNAL void lastTriggerUSecChanged();

    Q_PROPERTY(qulonglong LastTriggerUSecMonotonic READ lastTriggerUSecMonotonic NOTIFY lastTriggerUSecMonotonicChanged)
    inline qulonglong lastTriggerUSecMonotonic() const
    {
        return qvariant_cast<qulonglong>(property("LastTriggerUSecMonotonic"));
    }
    Q_SIGNAL void lastTriggerUSecMonotonicChanged();

    Q_PROPERTY(qulonglong NextElapseUSecMonotonic READ nextElapseUSecMonotonic NOTIFY nextElapseUSecMonotonicChanged)
    inline qulonglong nextElapseUSecMonotonic() const
    {
        return qvariant_cast<qulonglong>(property("NextElapseUSecMonotonic"));
    }
    Q_SIGNAL void nextElapseUSecMonotonicChanged();

    Q_PROPERTY(qulonglong NextElapseUSecRealtime READ nextElapseUSecRealtime NOTIFY nextElapseUSecRealtimeChanged)
    inline qulonglong nextElapseUSecRealtime() const
    {
        return qvariant_cast<qulonglong>(property("NextElapseUSecRealtime"));
    }
    Q_SIGNAL void nextElapseUSecRealtimeChanged();

    Q_PROPERTY(bool OnClockChange READ onClockChange NOTIFY onClockChangeChanged)
    inline bool onClockChange() const
    {
        return qvariant_cast<bool>(property("OnClockChange"));
    }
    Q_SIGNAL void onClockChangeChanged();

    Q_PROPERTY(bool OnTimezoneChange READ onTimezoneChange NOTIFY onTimezoneChangeChanged)
    inline bool onTimezoneChange() const
    {
        return qvariant_cast<bool>(property("OnTimezoneChange"));
    }
    Q_SIGNAL void onTimezoneChangeChanged();

    Q_PROPERTY(bool Persistent READ persistent NOTIFY persistentChanged)
    inline bool persistent() const
    {
        return qvariant_cast<bool>(property("Persistent"));
    }
    Q_SIGNAL void persistentChanged();

    Q_PROPERTY(qulonglong RandomizedDelayUSec READ randomizedDelayUSec NOTIFY randomizedDelayUSecChanged)
    inline qulonglong randomizedDelayUSec() const
    {
        return qvariant_cast<qulonglong>(property("RandomizedDelayUSec"));
    }
    Q_SIGNAL void randomizedDelayUSecChanged();

    Q_PROPERTY(bool RemainAfterElapse READ remainAfterElapse NOTIFY remainAfterElapseChanged)
    inline bool remainAfterElapse() const
    {
        return qvariant_cast<bool>(property("RemainAfterElapse"));
    }
    Q_SIGNAL void remainAfterElapseChanged();

    Q_PROPERTY(QString Result READ result NOTIFY resultChanged)
    inline QString result() const
    {
        return qvariant_cast<QString>(property("Result"));
    }
    Q_SIGNAL void resultChanged();

    Q_PROPERTY(QVariant TimersCalendar READ timersCalendar NOTIFY timersCalendarChanged)
    inline QVariant timersCalendar() const
    {
        return qvariant_cast<QVariant>(property("TimersCalendar"));
    }
    Q_SIGNAL void timersCalendarChanged();

    Q_PROPERTY(QVariant TimersMonotonic READ timersMonotonic NOTIFY timersMonotonicChanged)
    inline QVariant timersMonotonic() const
    {
        return qvariant_cast<QVariant>(property("TimersMonotonic"));
    }
    Q_SIGNAL void timersMonotonicChanged();

    Q_PROPERTY(QString Unit READ unit NOTIFY unitChanged)
    inline QString unit() const
    {
        return qvariant_cast<QString>(property("Unit"));
    }
    Q_SIGNAL void unitChanged();

    Q_PROPERTY(bool WakeSystem READ wakeSystem NOTIFY wakeSystemChanged)
    inline bool wakeSystem() const
    {
        return qvariant_cast<bool>(property("WakeSystem"));
    }
    Q_SIGNAL void wakeSystemChanged();
};

#endif
