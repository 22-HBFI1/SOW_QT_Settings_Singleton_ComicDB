#include "mainform.h"
MainForm::MainForm(QWidget *parent)
    : QWidget(parent)
    , translator(new QTranslator(this))
    , usedLocale(QLocale::system())
{
    settings = Settings::getSettings();
    QApplication::installTranslator(translator);
    translator->load(QString(":/languages/comicdb_%1").arg(usedLocale.name()));
    QString dbName("comic.sqlite3");
    QString dbPath = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(dbName);
    if (!QFile::exists(dbName)) {
        QMessageBox::critical(this,
                              tr("DB-Fehler"),
                              tr("Datenbank '%1' existiert nicht.").arg(dbPath),
                              tr("Programm&ende"));
        exit(1);
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        QMessageBox::critical(this,
                              tr("DB-Fehler"),
                              tr("Datenbank '%1' konnte nicht geöffnet werden.\n%2")
                                  .arg(dbName)
                                  .arg(db.lastError().text()),
                              tr("Programm&ende"));
        exit(2);
    }
    QSqlQuery query;

    if (!fillTableQuery(query)) {
        QMessageBox::critical(this,
                              tr("DB-Fehler"),
                              tr("Abfrage konnte nicht ausgeführt werden.\n%2")
                                  .arg(query.lastError().text()),
                              tr("Programm&ende"));
        exit(3);
    }
    setupUi(this);
    if (usedLocale.language() != QLocale::English)
        rbDe->setChecked(true);
    else
        rbEn->setChecked(true);
    clearInputFields();
    fillTableWidget(query);

    restoreSettings();
}

bool MainForm::fillTableQuery(QSqlQuery &query)
{
    QString queryText = QString("select id as %1, name as %2,\n"
                                "bewertung as %3, groesse as %4\n"
                                "from comicfigures;")
                            .arg(tr("ID"))
                            .arg(tr("Name"))
                            .arg(tr("Bewertung"))
                            .arg(tr("Größe"));
    return query.exec(queryText);
}

void MainForm::fillTableWidget(QSqlQuery &query)
{
    twComics->setRowCount(0);

    QSqlRecord record = query.record();
    int colCount = record.count();
    twComics->setColumnCount(colCount);
    QStringList headerList;
    for (int i = 0; i < colCount; i++)
        headerList << record.fieldName(i);
    twComics->setHorizontalHeaderLabels(headerList);

    while (query.next()) {
        QLocale l = usedLocale;
        int row = twComics->rowCount();
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        int rating = query.value(2).toInt();
        double size = query.value(3).toDouble();
        twComics->setRowCount(row + 1);

        QTableWidgetItem *item = new QTableWidgetItem(l.toString(id));
        fillItemData(item, id, name, rating, size);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        twComics->setItem(row, 0, item);
        item = new QTableWidgetItem(name);
        fillItemData(item, id, name, rating, size);
        twComics->setItem(row, 1, item);
        item = new QTableWidgetItem(tr("%1 Punkte").arg(l.toString(rating)));
        fillItemData(item, id, name, rating, size);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        twComics->setItem(row, 2, item);
        item = new QTableWidgetItem(tr("%1 m").arg(l.toString(size, 'f', 2)));
        fillItemData(item, id, name, rating, size);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        twComics->setItem(row, 3, item);
    }
    twComics->resizeColumnsToContents();
}

void MainForm::fillItemData(QTableWidgetItem *item, int id, QString name, int rating, double size)
{
    item->setData(ID_ROLE, id);
    item->setData(NAME_ROLE, name);
    item->setData(RATING_ROLE, rating);
    item->setData(SIZE_ROLE, size);
}

void MainForm::clearInputFields(void)
{
    editName->clear();
    sbRating->setValue(sbRating->minimum());
    sbSize->setValue(sbSize->minimum());
}

void MainForm::restoreSettings()
{
    // Lang
    QVariant lang = settings->getLanguage();
    if (lang.toString() == "en") {
        rbEn->setChecked(true);
        rbDe->setChecked(false);
        on_rbEn_clicked(true);
    } else {
        rbEn->setChecked(false);
        rbDe->setChecked(true);
        on_rbDe_clicked(true);
    }

    this->move(settings->getPosition());
    this->resize(settings->getSize());

    if (settings->getIsMax())
        this->showMaximized();
}

void MainForm::closeEvent(QCloseEvent *event)
{
    settings->setPosition(this->pos());
    settings->setSize(this->size());
    settings->setIsMax(this->isMaximized());
}

void MainForm::on_twComics_currentItemChanged(QTableWidgetItem * /*current*/,
                                              QTableWidgetItem * /*previous*/)
{
    int row = twComics->currentRow();
    if (row < 0) {
        clearInputFields();
        return;
    }
    QTableWidgetItem *item = twComics->item(row, 0);
    if (!item) {
        clearInputFields();
        return;
    }
    editName->setText(item->data(NAME_ROLE).toString());
    sbRating->setValue(item->data(RATING_ROLE).toInt());
    sbSize->setValue(item->data(SIZE_ROLE).toDouble());
}

void MainForm::on_btnAdd_clicked(void)
{
    QString name = editName->text().trimmed();
    if (name.isEmpty()) {
        editName->clear();
        editName->setFocus();
        return;
    }
    int rating = sbRating->value();
    if (rating <= sbRating->minimum()) {
        sbRating->setFocus();
        return;
    }
    double size = sbSize->value();
    if (size <= sbSize->minimum()) {
        sbSize->setFocus();
        return;
    }
    QString queryString = QString("insert into comicfigures values( "
                                  " NULL, :name, :rating, :size );");
    QSqlQuery query;
    bool ok = query.prepare(queryString);
    if (ok) {
        query.bindValue(":name", name);
        query.bindValue(":rating", rating);
        query.bindValue(":size", size);
        ok = ok && query.exec();
    }
    if (!ok)
        QMessageBox::warning(this,
                             tr("DB-Fehler"),
                             tr("Datensatz konnte nicht hinzugefügt werden.\n%1")
                                 .arg(query.lastError().text()),
                             tr("&Ok"));
    else if (fillTableQuery(query))
        fillTableWidget(query);
}

void MainForm::on_btnChange_clicked(void)
{
    int row = twComics->currentRow();
    if (row < 0)
        return;
    QTableWidgetItem *item = twComics->item(row, 0);
    if (!item)
        return;
    QString name = editName->text().trimmed();
    if (name.isEmpty()) {
        editName->clear();
        editName->setFocus();
        return;
    }
    int rating = sbRating->value();
    if (rating <= sbRating->minimum()) {
        sbRating->setFocus();
        return;
    }
    double size = sbSize->value();
    if (size <= sbSize->minimum()) {
        sbSize->setFocus();
        return;
    }
    int id = item->data(ID_ROLE).toInt();
    QSqlQuery query;
    QString queryString = QString("update comicfigures\n"
                                  "set name = :name, "
                                  "bewertung = :rating, "
                                  "groesse = :size\n"
                                  "where id = :id;");
    bool ok = query.prepare(queryString);
    if (ok) {
        query.bindValue(":name", name);
        query.bindValue(":rating", rating);
        query.bindValue(":size", size);
        query.bindValue(":id", id);
        ok = ok && query.exec();
    }
    if (!ok)
        QMessageBox::warning(this,
                             tr("DB-Fehler"),
                             tr("Datensatz konnte nicht geändert werden.\n%1")
                                 .arg(query.lastError().text()),
                             tr("&Ok"));
    else if (fillTableQuery(query))
        fillTableWidget(query);
}

void MainForm::on_btnDelete_clicked(void)
{
    int row = twComics->currentRow();
    if (row < 0)
        return;
    QTableWidgetItem *item = twComics->item(row, 0);
    if (!item)
        return;
    int id = item->data(ID_ROLE).toInt();
    QSqlQuery query;
    QString queryString = QString("delete from comicfigures\n"
                                  "where id = %1;")
                              .arg(id);
    if (!query.exec(queryString))
        QMessageBox::warning(this,
                             tr("DB-Fehler"),
                             tr("Datensatz konnte nicht geändert werden.\n%1")
                                 .arg(query.lastError().text()),
                             tr("&Ok"));
    else if (fillTableQuery(query))
        fillTableWidget(query);
}

void MainForm::on_rbDe_clicked(bool checked)
{
    if (!checked)
        return;
    usedLocale = QLocale(QLocale::German, QLocale::Germany);
    translator->load(":/languages/comicdb_de");
    retranslateUi(this);
    settings->setLanguage("de");
    QSqlQuery query;
    if (fillTableQuery(query))
        fillTableWidget(query);
}

void MainForm::on_rbEn_clicked(bool checked)
{
    if (!checked)
        return;
    usedLocale = QLocale(QLocale::English, QLocale::UnitedStates);
    translator->load(":/languages/comicdb_en");
    retranslateUi(this);
    settings->setLanguage("en");
    QSqlQuery query;
    if (fillTableQuery(query))
        fillTableWidget(query);
}
