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
#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QRegExp>
#include <QValidator>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QSettings>
#include "src/database/db.h"

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

private slots:
    void on_flightNumberPrefixLineEdit_textEdited(const QString &arg1);

    void themeGroup_toggled(int id);

    void on_aboutPushButton_clicked();

    void on_acSortComboBox_currentIndexChanged(int index);

    void on_acAllowIncompleteComboBox_currentIndexChanged(int index);

private:
    Ui::SettingsWidget *ui;

};

#endif // SETTINGSWIDGET_H
