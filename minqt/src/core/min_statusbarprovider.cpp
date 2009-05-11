/**
 * @file:     min_statusbarprovider.cpp
 * @version:  1.00
 * @date:     11.05.2009
 * @author:   Konrad Marek Zapalowicz
 */

// Module include
#include "min_statusbarprovider.hpp"

// -----------------------------------------------------------------------------
/*static*/ void Min::StatusBar::update(const QString &message, int timeout)
{
    Min::StatusBarProvider &sbp = Min::StatusBarProvider::getInstance();
    if (!sbp.isValid()) return;

    sbp.statusBar()->showMessage(message,timeout);
}
// -----------------------------------------------------------------------------
Min::StatusBarProvider::StatusBarProvider()
    : statusBar_(0)
{ ; }
// -----------------------------------------------------------------------------
Min::StatusBarProvider::~StatusBarProvider()
{ ; }
// -----------------------------------------------------------------------------
bool Min::StatusBarProvider::isValid() const
{ return statusBar_; }
// -----------------------------------------------------------------------------
void Min::StatusBarProvider::setStatusBar(QStatusBar *statusBar)
{ statusBar_ = statusBar; }
// -----------------------------------------------------------------------------
QStatusBar* Min::StatusBarProvider::statusBar() const
{ return statusBar_;}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
