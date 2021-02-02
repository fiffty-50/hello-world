/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "arunguard.h"

#include <QCryptographicHash>


namespace
{

QString generateKeyHash(const QString &key, const QString &salt)
{
    QByteArray data;

    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();

    return data;
}

}


ARunGuard::ARunGuard(const QString &key )
    : key(key )
    , memLockKey(generateKeyHash(key, "_memLockKey" ))
    , sharedmemKey(generateKeyHash(key, "_sharedmemKey" ))
    , sharedMem(sharedmemKey )
    , memLock(memLockKey, 1 )
{
    memLock.acquire();
    {
        QSharedMemory fix(sharedmemKey );    // Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
    }
    memLock.release();
}

ARunGuard::~ARunGuard()
{
    release();
}

bool ARunGuard::isAnotherRunning()
{
    if (sharedMem.isAttached())
        return false;

    memLock.acquire();
    const bool isRunning = sharedMem.attach();
    if (isRunning )
        sharedMem.detach();
    memLock.release();

    return isRunning;
}

bool ARunGuard::tryToRun()
{
    if (isAnotherRunning())  // Extra check
        return false;

    memLock.acquire();
    const bool result = sharedMem.create(sizeof(quint64 ));
    memLock.release();
    if (!result )
    {
        release();
        return false;
    }

    return true;
}

void ARunGuard::release()
{
    memLock.acquire();
    if (sharedMem.isAttached())
        sharedMem.detach();
    memLock.release();
}
