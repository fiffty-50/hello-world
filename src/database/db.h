/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#ifndef DB_H
#define DB_H

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QDebug>

/*!
 * \brief The Db class provides a basic API for accessing the database programatically.
 * It is used to set up the initial connection and various basic queries can be
 * executed using a set of static functions. When interfacing with the database
 * for the purpose of adding, deleting or updating entries, the use of the entry class
 * and its subclasses is recommended.
 */
class Db
{
    public:
        static Db& get()
        {
            static Db instance;

            return instance;
        }
        /*!
         * \brief The editRole enum {createNew, editExisting} is used to differentiate
         * between creating a new entry in the database vs editing an existing one
         */
        enum editRole {createNew, editExisting};
        /*!
         * \brief The matchType enum {exactMatch, partialMatch} is used to determine the
         * matching when using a WHERE sql statement. exactMatch results in a "=" operator,
         * whereas partiasMatch results in a "LIKE" operator
         */
        enum matchType {exactMatch, partialMatch};
        
        static void             connect(){get().iconnect();}
        static bool             exists(QString column, QString table, QString checkColumn,
                                       QString value, Db::matchType match){
            return get().iexists(column, table, checkColumn, value, match);
        }
        static bool             singleUpdate(QString table, QString column, QString value,
                                             QString checkColumn, QString checkvalue, Db::matchType match){
            return get().isingleUpdate(table,column,value,checkColumn,checkvalue,match);
        }
        static QString          singleSelect(QString column, QString table, QString checkColumn,
                                             QString value, Db::matchType match){
            return get().isingleSelect(column,table,checkColumn,value,match);
        }
        static QVector<QString> multiSelect(QVector<QString> columns, QString table,
                                            QString checkColumn, QString value, Db::matchType match){
            return get().imultiSelect(columns,table,checkColumn,value,match);
        }
        static QVector<QString> multiSelect(QVector<QString> columns, QString table){
            return get().imultiSelect(columns, table);
        }
        static QVector<QString> customQuery(QString query, int returnValues){
            return get().icustomQuery(query, returnValues);
        }
    private:
        Db() {}
        void             iconnect();
        bool             iexists(QString column, QString table, QString checkColumn,
                                       QString value, Db::matchType match);
        bool             isingleUpdate(QString table, QString column, QString value,
                                             QString checkColumn, QString checkvalue, Db::matchType match);
        QString          isingleSelect(QString column, QString table, QString checkColumn,
                                             QString value, Db::matchType match);
        QVector<QString> imultiSelect(QVector<QString> columns, QString table,
                                            QString checkColumn, QString value, Db::matchType match);
        QVector<QString> imultiSelect(QVector<QString> columns, QString table);
        QVector<QString> icustomQuery(QString query, int returnValues);

    public:
        Db(Db const&)              = delete;
        void operator=(Db const&)  = delete;
};

#endif // DB_H
