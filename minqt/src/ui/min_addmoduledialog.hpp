/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
 * Contact e-mail: min-support@lists.sourceforge.net
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 2 of the License. 
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
 * more details. You should have received a copy of the GNU General Public 
 * License along with this program. If not,  see 
 * <http://www.gnu.org/licenses/>.
 */

/**
 * @file:     min_addmoduledialog.hpp
 * @version:  1.00
 * @date:     27.04.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_ADDMODULEDIALOG_HPP
#define INCLUDED_MIN_ADDMODULEDIALOG_HPP

// System includes
#include <QDialog>
#include <QStringListModel>

// Forward declarations
class QGridLayout;
class QLabel;
class QListView;
class QLineEdit;
class QPushButton;

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class AddModuleDialog
     * @brief
     */
    class AddModuleDialog: public QDialog
    {
    Q_OBJECT
    Q_DISABLE_COPY(AddModuleDialog)
    public:
        /** Default C++ Constructor. */
        AddModuleDialog(QWidget *parent);

        /** Destructor. */
        ~AddModuleDialog();

    public slots:
        /** Accepts dialog - user pressed ok */
        void accept();

    signals:
        /** Transmits module name and test case files
         *  Format:
         *  [module] [tc file 1] [tc file 2] ... [tc file N]
         */
        void moduleAndTestCaseFiles(const QStringList data);
    private slots:
        /** Displays select file dialog */
        void selectModule();

        /** Displays add test case file dialog */
        void addTestCaseFile();

        /** Removes selected test case files from list */
        void removeTestCaseFile();

    private:
        /** Displays "select file" dialog.
         *  @param title is the title of the dialog
         *  @param filter is the filter of files to be displayed
         */
        QString displayFileOpenDialog(const QString &title,
				      const QString &filter);

        /** Lays out stuff on the dialog box */
        QGridLayout *layout_;

        /** "Module" label */
        QLabel *moduleLabel_;

        /** Displays selected module */
        QLineEdit *moduleEdit_;

        /** "Test Case files" label */
        QLabel *tcFilesLabel_;

        /** Displays list of added test case files */
        QListView *tcView_;

        /** Model which provides list of test case files */
        QStringListModel tcModel_;

        /** List of test cases files */
        QStringList testCaseFiles_;

        /** */
        QPushButton *pickModuleButton_;

        /** */
        QPushButton *addTcFileButton_;

        /** */
        QPushButton *removeTcFileButton_;

        /** */
        QPushButton *okButton_;

        /** */
        QPushButton *koButton_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_ADDMODULEDIALOG_HPP (file created by generator.sh v1.08)

