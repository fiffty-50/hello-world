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
#include "alog.h"
#include <QMessageBox>

namespace ALog {

static bool logDebug = false; // Debug doesn't log to file by default

/*!
 * \brief setLogFileName sets a log file name ("Log_<Date>_<Time>.txt")
 */
void setLogFileName()
{
    logFileName = QString(logFolder.absolutePath() + QLatin1String("/Log_%1_%2.txt")
                          ).arg(QDate::currentDate().toString(QStringLiteral("yyyy_MM_dd")),
                                QTime::currentTime().toString(QStringLiteral("hh_mm_ss")));
}

/*!
 * \brief Cleans up old logs and initializes logging by preparing and installing a QMessageHandler
 *
 */
void deleteOldLogs()
{  
    logFolder.setSorting(QDir::Time | QDir::Reversed);

    QFileInfoList logs_list = logFolder.entryInfoList();
    if (logs_list.size() <= numberOfLogs) {
        return;
    } else {
        for (int i = 0; i < (logs_list.size() - numberOfLogs); i++) {
            const QString path = logs_list.at(i).absoluteFilePath();
            QFile file(path);
            file.remove();
        }
    }
}

/*!
 * \brief initialise logging, clean up logfiles and install a QMessageHandler. To enable
 * logging of debug messages, pass parameter as true.
 */
bool init(bool log_debug)
{
    logDebug = log_debug;
    logFolder = AStandardPaths::directory(AStandardPaths::Log);
    deleteOldLogs();
    setLogFileName();

    QFile log_file((logFileName));
    if(log_file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qInstallMessageHandler(ALog::aMessageHandler);
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief aMessageHandler Intercepts Messages and prints to console and log file
 *
 * \abstract The message handler is responsible for intercepting the output from
 * qDebug(), qInfo(), qWarning() and qCritical(), formatting them and printing them
 * to the standard console out and to a logfile using QTextStream. Debug messages are
 * not written to the log file.
 *
 */
void aMessageHandler(QtMsgType type, const QMessageLogContext &context,
                      const QString& msg)
{
    const char *function = context.function ? context.function : "";
    //check file size and if needed create new log!
    {
        QFile outFileCheck(logFileName);
        if (outFileCheck.size() > sizeOfLogs) {
            deleteOldLogs();
            setLogFileName();
        }
    }
    // open the log file and prepare a textstream to write to it
    QFile log_file(logFileName);
    log_file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream log_stream(&log_file);

    switch (type) {
        case QtDebugMsg:
            QTextStream(stdout) << DEB_HEADER_CONSOLE << msg << endl << D_SPACER << function << "\033[m" << endl;
            if(logDebug)
                log_stream << timeNow() << DEB_HEADER << msg << D_SPACER << function << endl;
            break;
        case QtInfoMsg:
            log_stream << timeNow() << INFO_HEADER << msg << SPACER << function << endl;
            QTextStream(stdout) << INFO_HEADER_CONSOLE << msg << endl;
            break;
        case QtWarningMsg:
            log_stream << timeNow() << WARN_HEADER << msg << SPACER << endl;
            QTextStream(stdout) << WARN_HEADER_CONSOLE << msg << endl;
            break;
        case QtCriticalMsg:
            log_stream << timeNow() << CRIT_HEADER << msg << SPACER << endl;
            QTextStream(stdout) << CRIT_HEADER_CONSOLE << msg << endl;
            break;
    default:
            log_stream << timeNow() << INFO_HEADER << msg << function << endl;
            QTextStream(stdout) << INFO_HEADER_CONSOLE << msg << endl;
            break;
    }
}

} // namespace ALog
