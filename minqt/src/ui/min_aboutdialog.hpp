/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Maciej Jablonski
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


#ifndef INCLUDED_MIN_ABOUTDIALOG_HPP
#define INCLUDED_MIN_ABOUTDIALOG_HPP

// System includes
#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>


// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class AboutDialog
     * @brief shows About dialog box
     */
    class AboutDialog: public QDialog
    {
	    Q_OBJECT
	    Q_DISABLE_COPY(AboutDialog)
    public:
        /** Default C++ Constructor. */
        explicit AboutDialog(QWidget *parent = 0);

        /** Destructor. */
        virtual ~AboutDialog();
    protected:
        /** Handles event that comes from the system. */
        /* virtual void changeEvent (QEvent *event); */
    private:
	QDialogButtonBox *buttonBox;
	QLabel *label_1;
	QLabel *label_2;

    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_ABOUTDIALOG_HPP

