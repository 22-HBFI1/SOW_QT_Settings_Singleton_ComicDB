#ifndef MAINFORM_H
#define MAINFORM_H

#include <QtWidgets>
#include <QtSql>
#include "ui_mainform.h"
#include "settings.h"

#define ID_ROLE     ( Qt::UserRole + 0 )
#define NAME_ROLE   ( Qt::UserRole + 1 )
#define RATING_ROLE ( Qt::UserRole + 2 )
#define SIZE_ROLE   ( Qt::UserRole + 3 )

class MainForm : public QWidget, private Ui::MainForm
{
  Q_OBJECT
  private:
    QTranslator* translator;
    QLocale      usedLocale;
    Settings*   settings;


    bool         fillTableQuery( QSqlQuery& query );
    void         fillTableWidget( QSqlQuery& query );
    void         fillItemData( QTableWidgetItem* item, int id, QString name, int rating, double size );
    void         clearInputFields( void );
    void         restoreSettings();


protected:
    void closeEvent(QCloseEvent* event) override;

  public:
    explicit     MainForm( QWidget* parent = nullptr );
  private slots:
    void         on_twComics_currentItemChanged( QTableWidgetItem* current, QTableWidgetItem* previous );
    void         on_btnAdd_clicked( void );
    void         on_btnChange_clicked( void );
    void         on_btnDelete_clicked( void );
    void         on_rbDe_clicked( bool checked );
    void         on_rbEn_clicked( bool checked );
};

#endif // MAINFORM_H
