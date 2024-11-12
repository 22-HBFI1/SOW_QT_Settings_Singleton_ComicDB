#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QSettings>
#include <QFile>
#include <QLocale>

class Settings : public QWidget
{
    Q_OBJECT

public:
    static Settings* getSettings();
    QVariant getSetting(QString key, QVariant defaultValue);
    void setSetting(QString key, QVariant value);

    QString getLanguage();
    QPoint getPosition();
    QSize getSize();
    bool getIsMax();

    void setLanguage(QString lang);
    void setPosition(QPoint pos);
    void setSize(QSize size);
    void setIsMax(bool isMax);


    const QString settingsKeyLanguage = "lang";
    const QString settingsKeyPosition = "pos";
    const QString settingsKeySize = "size";
    const QString settingsKeyMaximized = "isMax";

protected:
    ~Settings();
private:
    explicit Settings(QWidget *parent = nullptr);
    static Settings* pointer;
    QSettings* settings;
    bool load();
    Settings& operator=(const Settings& s) = delete;
    Settings(const Settings& s) = delete;
    class MemGuard{public: ~MemGuard();};

signals:
};

#endif // SETTINGS_H
