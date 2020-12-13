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