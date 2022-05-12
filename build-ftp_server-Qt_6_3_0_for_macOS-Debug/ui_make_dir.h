/********************************************************************************
** Form generated from reading UI file 'make_dir.ui'
**
** Created by: Qt User Interface Compiler version 6.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAKE_DIR_H
#define UI_MAKE_DIR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_make_dir
{
public:
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLineEdit *name;
    QPushButton *ok;

    void setupUi(QDialog *make_dir)
    {
        if (make_dir->objectName().isEmpty())
            make_dir->setObjectName(QString::fromUtf8("make_dir"));
        make_dir->resize(460, 63);
        widget = new QWidget(make_dir);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(30, 10, 401, 41));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        name = new QLineEdit(widget);
        name->setObjectName(QString::fromUtf8("name"));

        horizontalLayout->addWidget(name);

        ok = new QPushButton(widget);
        ok->setObjectName(QString::fromUtf8("ok"));

        horizontalLayout->addWidget(ok);


        retranslateUi(make_dir);

        QMetaObject::connectSlotsByName(make_dir);
    } // setupUi

    void retranslateUi(QDialog *make_dir)
    {
        make_dir->setWindowTitle(QCoreApplication::translate("make_dir", "Dialog", nullptr));
        ok->setText(QCoreApplication::translate("make_dir", "Create", nullptr));
    } // retranslateUi

};

namespace Ui {
    class make_dir: public Ui_make_dir {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAKE_DIR_H
