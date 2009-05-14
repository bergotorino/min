/**
 * @file:     min_dbusconnectiondialog.hpp
 * @version:  1.00
 * @date:     13.05.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_DBUSCONNECTIONDIALOG_HPP
#define INCLUDED_MIN_DBUSCONNECTIONDIALOG_HPP

// System include
#include <QDialog>

// Forward declaratons
class QComboBox;
class QGridLayout;
class QPushButton;
class QLabel;

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class DBusConnectionDialog
     * @brief
     */
    class DBusConnectionDialog: public QDialog                                
    {
    Q_DISABLE_COPY(DBusConnectionDialog)
    Q_OBJECT
    public:
        /** Default C++ Constructor. */
        DBusConnectionDialog(QWidget *parent);

        /** Destructor. */
        ~DBusConnectionDialog();

    public slots:
        /** Dialog has been accepted */
        void accept();

    signals:
        /** Fired when user selects accept. It transmits the host and port
         *  in the form host:port
         */
        void selectedDBusConnection(const QString &address);

    protected:
    private:
        /** Layout for the dialog */
        QGridLayout *layout_;

        /** Combo box to select host */
        QComboBox *comboBox_;

        /** Displays text */
        QLabel *selectText_;

        /** Accept dialog */
        QPushButton *okButton_;

        /** Cancel dialog */
        QPushButton *cancelButton_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_DBUSCONNECTIONDIALOG_HPP (file created by generator.sh v1.08)

