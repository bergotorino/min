/**
 * @file:     min_executedalldelegate.hpp
 * @version:  1.00
 * @date:     11.05.2009
 * @author:   Konrad Marek Zapalowicz
 */

#ifndef INCLUDED_MIN_EXECUTEDALLDELEGATE_HPP
#define INCLUDED_MIN_EXECUTEDALLDELEGATE_HPP

// System includes
#include <QItemDelegate>

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class ExecutedAllDelegate
     * @brief
     */
    class ExecutedAllDelegate: public QItemDelegate
    {
    Q_DISABLE_COPY(ExecutedAllDelegate)
    public:
        /** Default C++ Constructor. */
        ExecutedAllDelegate();

        /** Destructor. */
        ~ExecutedAllDelegate();
        /** @inherited from QItemDelegate */
        void paint(QPainter *painter,
                    const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;

    protected:
    private:
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_EXECUTEDALLDELEGATE_HPP (file created by generator.sh v1.08)

