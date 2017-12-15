/* -*- c++ -*- */
/*
 * Gqrx SDR: Software defined radio receiver powered by GNU Radio and Qt
 *           http://gqrx.dk/
 *
 * Copyright 2017 Alexander Fasching OE5TKM.
 *
 * Gqrx is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gqrx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gqrx; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <QtGlobal>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QLibrary>
#include <QPluginLoader>

#include "dockplugins.h"
#include "ui_dockplugins.h"



DockPlugins::DockPlugins(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockPlugins)
{
    ui->setupUi(this);

    /* The following steps create a grid layout in a scroll area */
    pluginLayout = new QGridLayout();
    pluginLayout->setObjectName(QString::fromUtf8("pluginLayout"));

    QWidget *viewport = new QWidget;
    viewport->setLayout(pluginLayout);
    ui->scrollArea->setWidget(viewport);

    /* TODO: Change to seperate plugin directory */
    auto xdgdir = qgetenv("XDG_CONFIG_HOME");
    if (xdgdir.isEmpty())
        searchDir = QString("%1/.config/gqrx").arg(QDir::homePath());
    else
        searchDir = QString("%1/gqrx").arg(xdgdir.data());

    // Trigger initial refresh
    on_refreshButton_clicked();

}

DockPlugins::~DockPlugins()
{
    unloadAll();

    delete ui;
    delete pluginLayout;
}

void DockPlugins::on_refreshButton_clicked(void)
{
    /* Clean up the layout */
    while (not pluginWidgets.isEmpty()) {
        auto widgets = pluginWidgets.takeFirst();
        if (widgets->label != nullptr) {
            pluginLayout->removeWidget(widgets->label);
            delete widgets->label;
        }
        if (widgets->showButton != nullptr) {
            pluginLayout->removeWidget(widgets->showButton);
            delete widgets->showButton;
        }
        if (widgets->loadButton != nullptr) {
            pluginLayout->removeWidget(widgets->loadButton);
            delete widgets->loadButton;
        }
        delete widgets;
    }


    /* get files in searchpath */
    auto files = QDir(searchDir).entryList(QDir::Files);

    int i = 0;
    for (auto &file : files) {
        if (!QLibrary::isLibrary(file))
            continue;

        auto path = QDir::cleanPath(searchDir + QDir::separator() + file);

        auto widgets = new PluginWidgets;

        widgets->loader = new QPluginLoader(path, this);
        widgets->instance = nullptr;
        widgets->label = new QLabel(file, this);
        widgets->showButton = nullptr;
        widgets->loadButton = new QPushButton("Load", this);
        widgets->loadButton->setMaximumWidth(60);
        widgets->loadButton->setMinimumWidth(60);
        connect(widgets->loadButton, SIGNAL(clicked(bool)), this, SLOT(loadButtonClicked(void)));

        pluginWidgets.append(widgets);
        pluginLayout->addWidget(widgets->label, i, 0);
        pluginLayout->addWidget(widgets->loadButton, i, 2);

        i++;
    }
}

/* Load of plugin requested */
void DockPlugins::loadButtonClicked(void)
{
    /* Determine sender */
    QObject *obj = sender();
    PluginWidgets *widgets = nullptr;

    int i;
    for (i=0; i<pluginWidgets.count(); ++i) {
        if (pluginWidgets.value(i)->loadButton == obj) {
            widgets = pluginWidgets.value(i);
            break;
        }
    }

    /* This should not happen */
    if (widgets == nullptr) {
        qDebug() << "Sender of load event not found";
        return;
    }

    /* Load the plugin if it's not loaded */
    if (not widgets->loader->isLoaded()) {
        qDebug() << "Loading plugin" << widgets->label->text();
        widgets->loadButton->setText("Unload");

        /* Try to get an instance */
        if (auto instance = widgets->loader->instance()) {
            /* Check if plugin implements our interface */
            if (auto plugin = qobject_cast<PluginInterface*>(instance)) {
                qDebug() << "PluginInterface detected";
                widgets->instance = instance;

                loadedPlugins.append(plugin);

                /* Enable visibility button if it's a GUI widget */
                if (qobject_cast<QWidget*>(instance)) {
                    qDebug() << "Plugin is a widget";
                    widgets->showButton = new QPushButton("Show", this);
                    widgets->showButton->setMinimumWidth(60);
                    widgets->showButton->setMaximumWidth(60);

                    pluginLayout->addWidget(widgets->showButton, i, 1);
                    connect(widgets->showButton, SIGNAL(clicked(bool)), this, SLOT(showButtonClicked(void)));
                }
                else {
                    qDebug() << "Plugin is not a widget";
                }
            }
            else {
                qDebug() << "Cast to PluginInterface failed";
            }
        }
        else {
            qDebug() << "Couldn't get instance";
        }
    }
    /* Unload the plugin */
    else {
        qDebug() << "Unloading plugin" << widgets->label->text();

        if (auto plugin = qobject_cast<PluginInterface*>(widgets->instance)) {
            if (not loadedPlugins.removeOne(plugin)) {
                qDebug() << "Couln't remove instance from loadedPlugins list";
            }
        }

        widgets->loader->unload();
        widgets->loadButton->setText("Load");

        /* Remove the "show" button */
        if (widgets->showButton != nullptr) {
            pluginLayout->removeWidget(widgets->showButton);
            delete widgets->showButton;
            widgets->showButton = nullptr;
        }
    }
}

/* Toggle visibility of a plugin */
void DockPlugins::showButtonClicked(void)
{
    /* Determine sender */
    QObject *obj = sender();
    PluginWidgets *widgets = nullptr;

    for (int i=0; i<pluginWidgets.count(); ++i) {
        if (pluginWidgets.value(i)->showButton == obj) {
            widgets = pluginWidgets.value(i);
            break;
        }
    }

    /* Check not really necessary. Other objects shouldn't get a "show" button */
    if (auto widget = qobject_cast<QWidget*>(widgets->instance)) {
        if (widget->isVisible()) {
            widget->hide();
            widgets->showButton->setText("Show");
        }
        else {
            widget->show();
            widgets->showButton->setText("Hide");
        }
    }
}

/* Unload all plugins */
void DockPlugins::unloadAll(void)
{
    for (auto &widgets : pluginWidgets) {
        if (widgets->loader->isLoaded())
            widgets->loader->unload();
    }
}
