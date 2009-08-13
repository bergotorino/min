/**
 * @file:     min_statusbarprovider.hpp
 * @version:  1.00
 * @date:     11.05.2009
 * @author:   Konrad Marek Zapalowicz
 */

#ifndef INCLUDED_MIN_STATUSBARPROVIDER_HPP
#define INCLUDED_MIN_STATUSBARPROVIDER_HPP

// System includes
#include <QStatusBar>

// Min includes
#include "min_singleton.hpp"

// Forward declarations

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class StatusBar
     * @brief status bar for GUI
     */
    class StatusBar
    {
    public:
        static void update(const QString &message,int timeout=0);
    };
    // -------------------------------------------------------------------------
    /**
     * @class StatusBarProvider
     * @brief
     */
    class StatusBarProvider: public Min::Singleton<Min::StatusBarProvider>
    {
    friend class Min::Singleton<Min::StatusBarProvider>;
    public:

        /** Destructor. */
        ~StatusBarProvider();

    public:
        /** Returns true if statusBar_ has been set. */
        bool isValid() const;

        /** Sets statusBar_ internal variable */
        void setStatusBar(QStatusBar *statusBar);

        /** Returns pointer to the statusBar_ or NULL */
        QStatusBar* statusBar() const;

    protected:
    private:
        /** Default C++ Constructor. */
        StatusBarProvider();
        /** Copy Constructor. */
        StatusBarProvider( const Min::StatusBarProvider& c );
        /** Assignment operator. */
        Min::StatusBarProvider& operator =( const Min::StatusBarProvider& c );

        /** Status bar which will be shared across the application */
        QStatusBar *statusBar_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_STATUSBARPROVIDER_HPP (file created by generator.sh v1.08)

