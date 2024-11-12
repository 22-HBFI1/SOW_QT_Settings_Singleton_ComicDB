#include "settings.h"
Settings* Settings::pointer = nullptr;

Settings *Settings::getSettings()
{
    static MemGuard memGuard;

    if (pointer == nullptr)
        pointer = new Settings();

    return pointer;
}

QVariant Settings::getSetting(QString key, QVariant defaultValue)
{
    return settings->value(key, defaultValue);
}

void Settings::setSetting(QString key, QVariant value)
{
    settings->setValue(key, value);
}

QString Settings::getLanguage()
{
    return getSetting(settingsKeyLanguage, QLocale::system()).toString();
}

QPoint Settings::getPosition()
{
    return getSetting(settingsKeyPosition, QPoint()).toPoint();
}

QSize Settings::getSize()
{
    return getSetting(settingsKeySize, QSize()).toSize();
}

bool Settings::getIsMax()
{
    return getSetting(settingsKeyMaximized, false).toBool();
}

void Settings::setLanguage(QString lang)
{
    setSetting(settingsKeyLanguage, lang);
}

void Settings::setPosition(QPoint pos)
{
    setSetting(settingsKeyPosition, pos);
}

void Settings::setSize(QSize size)
{
    setSetting(settingsKeySize, size);
}

void Settings::setIsMax(bool isMax)
{
    setSetting(settingsKeyMaximized, isMax);
}

Settings::Settings(QWidget *parent)
    : QWidget{parent}
{
    settings = new QSettings("settings.ini", QSettings::IniFormat, this);
}

Settings::~Settings()
{
    delete settings;
}

Settings::MemGuard::~MemGuard()
{
    delete Settings::getSettings();
}
