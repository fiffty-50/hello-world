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
#include "logbookwidget.h"
#include "ui_logbookwidget.h"

#include "src/classes/aflightentry.h"
#include "src/database/adatabase.h"
#include "src/classes/asettings.h"
#include "src/gui/dialogues/newflightdialog.h"
#include "src/functions/alog.h"
#include "src/functions/alog.h"

const QMap<int, QString> FILTER_MAP = {
    {0, QStringLiteral("Date LIKE \"%")},
    {1, QStringLiteral("Dept LIKE \"%")},
    {2, QStringLiteral("Dest LIKE \"%")},
    {3, QStringLiteral("Registration LIKE \"%")},
    {4, QStringLiteral("\"Name PIC\" LIKE \"%")}
};
const auto NON_WORD_CHAR = QRegularExpression("\\W");

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);
    ui->newFlightButton->setFocus();

    //customContextMenu for tablewidget
    menu  = new QMenu(this);
    menu->addAction(ui->actionEdit_Flight);
    menu->addAction(ui->actionDelete_Flight);

    //Initialise message Box
    messageBox = new QMessageBox(this);
    // Initalise the display Model and view
    displayModel = new QSqlTableModel(this);
    view = ui->tableView;

    setupModelAndView(ASettings::read(ASettings::Main::LogbookView).toInt());
    connectSignalsAndSlots();

}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}

/*
 * Functions
 */

void LogbookWidget::setupModelAndView(int view_id)
{
    switch (view_id) {
    case 0:
        LOG << "Loading Default View...";
        displayModel->setTable(QStringLiteral("viewDefault"));
        displayModel->select();
        break;
    case 1:
        LOG << "Loading EASA View...";
        displayModel->setTable(QStringLiteral("viewEASA"));
        displayModel->select();
        break;
    default:
        LOG << "Loading Default View...";
        displayModel->setTable(QStringLiteral("viewDefault"));
        displayModel->select();
    }

    view->setModel(displayModel);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);
    view->resizeColumnsToContents();
    view->show();
}

void LogbookWidget::connectSignalsAndSlots()
{
    selectionModel = view->selectionModel();
    QObject::connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &LogbookWidget::flightsTableView_selectionChanged);
}
/*
 * Slots
 */

void LogbookWidget::flightsTableView_selectionChanged()
{
    selectedFlights.clear();
    for (const auto& row : selectionModel->selectedRows()) {
        selectedFlights.append(row.data().toInt());
        DEB << "Selected Flight(s) with ID: " << selectedFlights;
    }
}

void LogbookWidget::on_newFlightButton_clicked()
{
    auto nf = new NewFlightDialog(this);
    nf->setAttribute(Qt::WA_DeleteOnClose);
    nf->exec();
    displayModel->select();
}

void LogbookWidget::on_editFlightButton_clicked()
{
    if(selectedFlights.length() == 1){
        auto ef = new NewFlightDialog(selectedFlights.first(), this);
        ef->setAttribute(Qt::WA_DeleteOnClose);
        ef->exec();
        displayModel->select();
    } else if (selectedFlights.isEmpty()) {
        messageBox->setText(tr("<br>No flight selected.<br>"));
        messageBox->exec();
    } else {
        messageBox->setText(tr("<br>More than one flight selected."
                               "<br><br>Editing multiple entries is not yet supported."));
        messageBox->exec();
    }
}

void LogbookWidget::on_deleteFlightPushButton_clicked()
{
    DEB << "Flights selected: " << selectedFlights.length();
    if (selectedFlights.length() == 0) {
        messageBox->setIcon(QMessageBox::Information);
        messageBox->setText(tr("<br>No flight selected.<br>"));
        messageBox->exec();
        return;
    } else if (selectedFlights.length() > 0 && selectedFlights.length() <= 10) {
        QList<AFlightEntry> flights_list;

        for (const auto &flight_id : selectedFlights) {
            flights_list.append(aDB->getFlightEntry(flight_id));
        }

        QString flights_list_string;

        for (auto &flight : flights_list) {
            flights_list_string.append(flight.summary());
            flights_list_string.append(QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
        }

        QMessageBox confirm(this);
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Question);
        confirm.setWindowTitle("Delete Flight");
        confirm.setText(tr("The following flight(s) will be deleted:<br><br><b><tt>"
                           "%1<br></b></tt>"
                           "Deleting flights is irreversible.<br>Do you want to proceed?"
                           ).arg(flights_list_string));
        if (confirm.exec() == QMessageBox::Yes) {
            for (auto& flight : flights_list) {
                DEB << "Deleting flight: " << flight.summary();
                if(!aDB->remove(flight)) {
                    confirm.setText(tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
                                       ).arg(aDB->lastError.text()));
                    messageBox->exec();
                    return;
                }
            }
            messageBox->setText(tr("%1 flights have been deleted successfully."
                                   ).arg(QString::number(selectedFlights.length())));
            messageBox->exec();
            displayModel->select();
        }
    } else if (selectedFlights.length() > 10) {
        QMessageBox confirm;
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Warning);
        confirm.setWindowTitle("Delete Flight");
        confirm.setText(tr("You have selected %1 flights.<br><br>"
                           "Deleting flights is irreversible.<br><br>"
                           "Are you sure you want to proceed?"
                           ).arg(QString::number(selectedFlights.length())));
        if(confirm.exec() == QMessageBox::Yes) {
            QList<DataPosition> selected_flights;
            for (const auto& flight_id : selectedFlights) {
                selected_flights.append({QStringLiteral("flights"), flight_id});
            }
            if (!aDB->removeMany(selected_flights)) {
                messageBox->setText(aDB->lastError.text()); // [F]: To Do: error info
                messageBox->exec();
                return;
            }
            messageBox->setText(tr("%1 flights have been deleted successfully."
                                   ).arg(QString::number(selectedFlights.length())));
            messageBox->exec();
            displayModel->select();
        }
        displayModel->select();
    }
}


void LogbookWidget::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void LogbookWidget::on_actionDelete_Flight_triggered()
{
    emit ui->deleteFlightPushButton->clicked();
}

void LogbookWidget::on_actionEdit_Flight_triggered()
{
    emit ui->editFlightButton->clicked();
}

void LogbookWidget::on_tableView_doubleClicked()
{
    emit ui->editFlightButton->clicked();
}

void LogbookWidget::on_flightSearchComboBox_currentIndexChanged(int)
{
    emit ui->showAllButton->clicked();
}

void LogbookWidget::refresh()
{
    //refresh view to reflect changes the user has made via a dialog.
    displayModel->select();
    view->resizeColumnsToContents();
}

void LogbookWidget::onLogbookWidget_viewSelectionChanged(SettingsWidget::SettingSignal signal)
{
    if (signal == SettingsWidget::SettingSignal::LogbookWidget)
        setupModelAndView(ASettings::read(ASettings::Main::LogbookView).toInt());
}

void LogbookWidget::on_showAllButton_clicked()
{
    ui->flightSearchLlineEdit->setText(QString());
    displayModel->setFilter(QString());
    displayModel->select();
}

void LogbookWidget::on_flightSearchLlineEdit_textChanged(const QString &arg1)
{
    if(arg1.length() == 0) {
        displayModel->setFilter("");
        displayModel->select();
        return;
    }

    if (ui->flightSearchComboBox->currentIndex() < 3) {
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + QStringLiteral("%\""));
        return;
    } else if (ui->flightSearchComboBox->currentIndex() == 3) { // registration
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + QStringLiteral("%\""));
        return;
    } else if (ui->flightSearchComboBox->currentIndex() == 4) { // Name Pic
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + QStringLiteral("%\""));
        return;
    }
}

void LogbookWidget::repopulateModel()
{
    // unset the current model and delete it to avoid leak
    view->setModel(nullptr);
    delete displayModel;
    // create a new model and populate it
    displayModel = new QSqlTableModel(this);
    setupModelAndView(ASettings::read(ASettings::Main::LogbookView).toInt());
    connectSignalsAndSlots();
}
