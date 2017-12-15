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
#ifndef DOCKPLUGINS_H
#define DOCKPLUGINS_H

#include <QDockWidget>
#include <QSettings>
#include <QStringList>
#include <QList>
#include <QPluginLoader>
#include <QGridLayout>
#include <QPushButton>

#include "plugins/plugininterface.h"


namespace Ui {
    class DockPlugins;
}

/**
 * Widgets for a single plugin
 */
struct PluginWidgets {
    QPluginLoader *loader;
    QObject *instance;
    QLabel *label;
    QPushButton *showButton;
    QPushButton *loadButton;
};

/**
 * @brief Dock window with plugin options.
 * @ingroup UI
 *
 * This dock widget encapsulates controls for loading/unloading
 * and showing/hiding plugins. It automatically scans configured
 * directories for available plugins.
 */
class DockPlugins : public QDockWidget
{
    Q_OBJECT

public:

    explicit DockPlugins(QWidget *parent = 0);
    ~DockPlugins();

private slots:
    void on_refreshButton_clicked(void);
    void loadButtonClicked(void);
    void showButtonClicked(void);

private:
    void unloadAll(void);

private:
    Ui::DockPlugins *ui;                /* The Qt designer UI file */
    QString searchDir;                  /* Directory to look for plugins */
    QGridLayout *pluginLayout;          /* Grid layout containing plugin controls */
    QList<PluginWidgets *> pluginWidgets; /* Created plugin loaders */
    QList<PluginInterface *> loadedPlugins; /* Currently loaded plugins */
};

#endif // DOCKPLUGINS_H
