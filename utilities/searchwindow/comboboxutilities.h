/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-03-14
 * Description : User interface for searches
 * 
 * Copyright (C) 2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef COMBOBOXUTILITIES_H
#define COMBOBOXUTILITIES_H

// Qt includes

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

// KDE includes

// Local includes


class QVBoxLayout;

namespace Digikam
{


class ProxyLineEdit : public QLineEdit
{
    Q_OBJECT

public:

    /**
     * This class will not act as a QLineEdit at all,
     * but present another widget (any kind of widget)
     * instead in the space assigned to the QLineEdit.
     * Use this class if you need to pass a QLineEdit but
     * want actually to use a different widget.
     */

    ProxyLineEdit(QWidget *parent = 0);
    /// After contructing, set the actual widget here
    void setWidget(QWidget *widget);

protected:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dropEvent(QDropEvent *event);
    void changeEvent(QEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void inputMethodEvent(QInputMethodEvent *event);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    QWidget     *m_widget;
    QVBoxLayout *m_layout;
};

class ModelIndexBasedComboBox : public QComboBox
{
public:

    /**
     * QComboBox has a current index based on a single integer.
     * This is not sufficient for more complex models.
     * This class is a combo box that stores a current index
     * based on QModelIndex.
     */

    ModelIndexBasedComboBox(QWidget *parent = 0);

    virtual void hidePopup();
    virtual void showPopup();

    QModelIndex currentIndex() const;
    void setCurrentIndex(const QModelIndex &index);

protected:

    QModelIndex m_currentIndex;
};

class StayPoppedUpComboBox : public ModelIndexBasedComboBox
{
    Q_OBJECT

public:

    /** This class provides an abstract QComboBox with a custom view
     *  (which is created by implementing subclasses)
     *  instead of the usual QListView.
     *  The Pop-up of the combo box will stay open after selecting an item;
     *  it will be closed by clicking outside, but not inside the widget.
     *  You need three steps:
     *  Contruct the object, call setModel() with an appropriate
     *  QAbstractItemModel, then call installView() to replace
     *  the standard combo box view with a view.
     */
    StayPoppedUpComboBox(QWidget *parent = 0);

protected:

    /** Implement in subclass:
     *  Send the given event to the viewportEvent() method of m_view.
     *  This method is protected for a usual QAbstractItemView.
     */
    virtual void sendViewportEventToView(QEvent *e) = 0;

    /** Replace the standard combo box list view with the given view.
     *  The view will be set as the view of the combo box
     *  (including reparenting) and be stored in the m_view variable */
    void installView(QAbstractItemView *view);

    virtual bool eventFilter(QObject *watched, QEvent *event);

    QAbstractItemView   *m_view;
};

}

#endif

