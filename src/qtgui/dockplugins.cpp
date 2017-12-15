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
#include <QMainWindow>

#include "dockplugins.h"
#include "ui_dockplugins.h"



DockPlugins::DockPlugins(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockPlugins)
{
    ui->setupUi(this);

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
}

void DockPlugins::on_refreshButton_clicked(void)
{

    unloadAll();

    /* get files in searchpath */
    auto files = QDir(searchDir).entryList(QDir::Files);

    int i = 0;
    for (auto &file : files) {
        if (not QLibrary::isLibrary(file))
            continue;

        auto path = QDir::cleanPath(searchDir + QDir::separator() + file);

        auto widgets = new PluginWidgets;

        widgets->loader = new QPluginLoader(path, this);
        auto meta = widgets->loader->metaData()["MetaData"].toObject();

        widgets->instance = nullptr;
        widgets->label = new QLabel(file, this);

        auto desc = meta["description"];
        if (desc.isString())
            widgets->label->setToolTip(desc.toString());

        widgets->showButton = nullptr;
        widgets->loadButton = new QPushButton("Load", this);
        widgets->loadButton->setMaximumWidth(60);
        widgets->loadButton->setMinimumWidth(60);
        connect(widgets->loadButton, SIGNAL(clicked(bool)), this, SLOT(loadButtonClicked(void)));

        pluginWidgets.append(widgets);
        ui->gridLayout->addWidget(widgets->label, i, 0);
        ui->gridLayout->addWidget(widgets->loadButton, i, 2);

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

        /* Try to get an instance */
        if (auto instance = widgets->loader->instance()) {
            /* At this point, we have loaded the plugin */
            widgets->loadButton->setText("Unload");

            /* Check if plugin implements our interface */
            if (auto plugin = qobject_cast<PluginInterface*>(instance)) {
                qDebug() << "PluginInterface detected";
                widgets->instance = instance;

                loadedPlugins.append(plugin);
                emit pluginsRunning(loadedPlugins.count() != 0);

                /* Enable visibility button if it's a GUI widget */
                if (qobject_cast<QWidget*>(instance)) {
                    qDebug() << "Plugin is a widget";
                    widgets->showButton = new QPushButton("Toggle", this);
                    widgets->showButton->setMinimumWidth(60);
                    widgets->showButton->setMaximumWidth(60);

                    /*
                     * TODO: This is terrible code. The whole initialization stuff
                     * should not depend on the type of the widget. We shouldn't
                     * have to hardcode different cases for different widget types.
                     */
                    if (auto widget = qobject_cast<QDockWidget *>(instance)) {
                        auto main = qobject_cast<QMainWindow *>(this->parent());
                        main->addDockWidget(Qt::LeftDockWidgetArea, widget);
                        plugin->initialize(this->parent());
                    }
                    else {
                        plugin->initialize(nullptr);
                    }

                    ui->gridLayout->addWidget(widgets->showButton, i, 1);
                    connect(widgets->showButton, SIGNAL(clicked(bool)), this, SLOT(showButtonClicked(void)));
                }
                else {
                    qDebug() << "Plugin is not a widget";

                    /* TODO: Terrible as well, belongs to the part above */
                    plugin->initialize(this->parent());
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

        if (not widgets->loader->unload())
            qDebug() << "Plugin could not be unloaded";

        widgets->loadButton->setText("Load");

        /* Remove the "show" button */
        if (widgets->showButton != nullptr) {
            ui->gridLayout->removeWidget(widgets->showButton);
            delete widgets->showButton;
            widgets->showButton = nullptr;
        }

        emit pluginsRunning(loadedPlugins.count() != 0);
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
        }
        else {
            widget->show();
        }
    }
}

/* Unload all plugins */
void DockPlugins::unloadAll(void)
{
    /* Clean up the layout */
    while (not pluginWidgets.isEmpty()) {
        auto widgets = pluginWidgets.takeFirst();

        if (widgets->loader->isLoaded())
            if (not widgets->loader->unload())
                qDebug() << "Plugin could not be unloaded";

        if (widgets->label != nullptr) {
            ui->gridLayout->removeWidget(widgets->label);
            delete widgets->label;
        }
        if (widgets->showButton != nullptr) {
            ui->gridLayout->removeWidget(widgets->showButton);
            delete widgets->showButton;
        }
        if (widgets->loadButton != nullptr) {
            ui->gridLayout->removeWidget(widgets->loadButton);
            delete widgets->loadButton;
        }
        delete widgets;
    }

    loadedPlugins.clear();
    emit pluginsRunning(loadedPlugins.count() != 0);
}

/* Notify all plugins about new samples */
void DockPlugins::processSamples(float *buffer, int length)
{
    for (auto &plugin : loadedPlugins) {
        plugin->processSamples(buffer, length);
    }
}
