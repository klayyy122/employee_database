#include "window.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QStatusBar>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QTableWidgetItem>
#include <QHeaderView>
#include<vector>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), db(nullptr), dbOpened(false){
    setupUI();
    connectSignals();
    refreshTable();
}

void MainWindow::setupUI(){
    setWindowTitle("БД сотрудников");
    setMinimumSize(1000, 700);
    
    setupMenu();
    setupToolbar();
    setupMainArea();
    setupStatusBar();

    sortContext.setStrategy(std::make_unique<SortById>());
}

void MainWindow::setupMenu(){
    QMenu *fileMenu = menuBar()->addMenu("&Файл");
    
    QAction *createAction = fileMenu->addAction("&Создать БД");
    QAction *openAction = fileMenu->addAction("&Открыть БД");
    QAction *saveAction = fileMenu->addAction("&Сохранить БД");
    fileMenu->addSeparator();
    QAction *clearAction = fileMenu->addAction("&Очистить БД");
    QAction *deleteAction = fileMenu->addAction("&Удалить БД");
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("&Выход");

    connect(createAction, &QAction::triggered, this, &MainWindow::createDatabase);
    connect(openAction, &QAction::triggered, this, &MainWindow::openDatabase);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveDatabase);
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearDatabase);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteDatabase);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    QMenu* toolsMenu = menuBar()->addMenu("Инструменты");
    QAction* backupAction = toolsMenu->addAction("Создать бэкап");
    QAction* restoreAction = toolsMenu->addAction("Восстановить из бэкапа");
    QAction* exportAction = toolsMenu->addAction("Экспортировать Excel");

    connect(backupAction, &QAction::triggered, this, &MainWindow::createBackup);
    connect(restoreAction, &QAction::triggered, this, &MainWindow::restoreFromBackup);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportToExcel);
}

void MainWindow::setupStatusBar(){
    statusLabel = new QLabel("База данных не открыта.");
    statusBar()->addWidget(statusLabel);
}
void MainWindow::setupToolbar(){
    QToolBar* toolBar = addToolBar("Основная панель");

    // Оставляем только основные кнопки
    refreshButton = new QPushButton("Обновить таблицу");
    backupButton = new QPushButton("Бекап");
    restoreButton = new QPushButton("Восстановить");
    exportButton = new QPushButton("Экспорт");

    toolBar->addWidget(refreshButton);
    toolBar->addWidget(backupButton);
    toolBar->addWidget(restoreButton);
    toolBar->addWidget(exportButton);
}
void MainWindow::setupMainArea(){
    QWidget* mainWidget = new QWidget;
    setCentralWidget(mainWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);

    // data input
    QGroupBox* inputGroup = new QGroupBox("Данные сотрудника");
    QFormLayout* formLayout = new QFormLayout;

    idEdit = new QLineEdit;
    nameEdit = new QLineEdit;
    positionEdit = new QLineEdit;
    salaryEdit = new QLineEdit;
    ageEdit = new QLineEdit;

    idEdit->setPlaceholderText("Уникальный ID");
    nameEdit->setPlaceholderText("ФИО сотрудника");
    positionEdit->setPlaceholderText("Должность");
    salaryEdit->setPlaceholderText("Зарплата");
    ageEdit->setPlaceholderText("Возраст");

    idEdit->setReadOnly(true);
    idEdit->setText("auto");

    // КНОПКИ ДЛЯ ДАННЫХ
    QHBoxLayout* dataButtonsLayout = new QHBoxLayout;
    addButton = new QPushButton("Добавить");
    editButton = new QPushButton("Редактировать");
    clearFormButton = new QPushButton("Очистить форму");
    
    dataButtonsLayout->addWidget(addButton);
    dataButtonsLayout->addWidget(editButton);
    dataButtonsLayout->addWidget(clearFormButton);
    dataButtonsLayout->addStretch();

    formLayout->addRow("ID: ", idEdit);
    formLayout->addRow("ФИО: ", nameEdit);
    formLayout->addRow("Должность: ", positionEdit);
    formLayout->addRow("Зарплата: ", salaryEdit);
    formLayout->addRow("Возраст: ", ageEdit);
    formLayout->addRow(dataButtonsLayout); // Добавляем кнопки под формой

    inputGroup->setLayout(formLayout);

    // ПОИСК
    QGroupBox* searchGroup = new QGroupBox("Поиск");
    QHBoxLayout* searchLayout = new QHBoxLayout;

    searchFieldCombo = new QComboBox;
    searchFieldCombo->addItems({"ID", "ФИО"});
    searchValueEdit = new QLineEdit;
    searchButton = new QPushButton("Найти");
    showAllButton = new QPushButton("Показать все");

    searchLayout->addWidget(new QLabel("Поле:"));
    searchLayout->addWidget(searchFieldCombo);
    searchLayout->addWidget(new QLabel("Значение: "));
    searchLayout->addWidget(searchValueEdit);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(showAllButton);
    searchLayout->addStretch();

    searchGroup->setLayout(searchLayout);
    

    // УДАЛЕНИЕ
    QGroupBox* deleteGroup = new QGroupBox("Удаление");
    QHBoxLayout* deleteLayout = new QHBoxLayout;

    deleteFieldCombo = new QComboBox;
    deleteFieldCombo->addItems({"ID", "ФИО"});
    deleteValueEdit = new QLineEdit;
    deleteButton = new QPushButton("Удалить");

    deleteLayout->addWidget(new QLabel("Поле:"));
    deleteLayout->addWidget(deleteFieldCombo);
    deleteLayout->addWidget(new QLabel("Значение:"));
    deleteLayout->addWidget(deleteValueEdit);
    deleteLayout->addWidget(deleteButton);
    deleteLayout->addStretch();

    deleteGroup->setLayout(deleteLayout);

    // ТАБЛИЦА
    tableWidget = new QTableWidget;
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels({"ID", "ФИО", "Должность", "Зарплата", "Возраст"});
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setAlternatingRowColors(true);

    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(searchGroup);
    mainLayout->addWidget(deleteGroup);
    mainLayout->addWidget(tableWidget);
    
    tableWidget->horizontalHeader()->setSectionsClickable(true);
}

void MainWindow::connectSignals(){
    // Кнопки данных
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addData);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editData);
    connect(clearFormButton, &QPushButton::clicked, this, &MainWindow::clearForm);
    
    // Кнопки поиска
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchData);
    connect(showAllButton, &QPushButton::clicked, this, &MainWindow::refreshTable);
    
    // Кнопка удаления
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteData);
    
    // Кнопки тулбара
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshTable);
    connect(backupButton, &QPushButton::clicked, this, &MainWindow::createBackup);
    connect(restoreButton, &QPushButton::clicked, this, &MainWindow::restoreFromBackup);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportToExcel);

    // Взаимодействие с таблицей
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableCellDoubleClick);
    connect(tableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTableSelectionChanged);
    
    // Поиск при нажатии Enter в поле поиска
    connect(searchValueEdit, &QLineEdit::returnPressed, this, &MainWindow::searchData);
    connect(deleteValueEdit, &QLineEdit::returnPressed, this, &MainWindow::deleteData);

     connect(tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, 
            this, &MainWindow::onHeaderClicked);
}

void MainWindow::createDatabase(){
    QString fileName = QFileDialog::getSaveFileName(this, "Создать БД", "", "Database Files (*.db);;All Files (*)");
    if(!fileName.isEmpty()){
        if(!fileName.endsWith(".db", Qt::CaseInsensitive)){
            fileName += ".db";
        }
    
        db = new Database(fileName.toStdString());
        if(db->create()){
            dbOpened = true;
            showSuccess("БД создана: " + fileName);
            refreshTable();
            updateStatusBar();
        }else{
            showError("Ошибка создания БД");
            delete db;
            db = nullptr;
            dbOpened = false;
        }
    }
}
void MainWindow::openDatabase(){
    
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть БД", "", "Database files (*db)");
    if(!fileName.isEmpty()){
        db = new Database(fileName.toStdString());
        if(db->open()){
            dbOpened = true;
            showSuccess("БД открыта: " + fileName);
            refreshTable();
            updateStatusBar();
        }else{
        showError("Ошибка открытия БД");
        delete db;
        db = nullptr;
        dbOpened = false;
        }
    }
}

void MainWindow::saveDatabase(){

    if(db && dbOpened){
        if(db->save()){
            showSuccess("Бд сохранена");

        }else{
            showError("Ошибка: не удалось сохранить бд");
        }
    }else{
        showError("Ошибка: БД не открыта");

    }
}

void MainWindow::clearDatabase(){

    if(db && dbOpened){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Да", "Вы уверены что хотите очистить бд?", QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes){
            if(db->clear()){
                showSuccess("Бд очищена");
                refreshTable();
                
            } else{
                showError("Ошибка: не удалось очистить бд");
            }
        }
    }else{
        showError("База данных не открыта");
    }
}

void MainWindow::deleteDatabase()
{
    if (db && dbOpened) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены что хотите удалить базу данных?",
                                     QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QString fileName = QString::fromStdString(db->getFileName());
            if (db->removeDB()) {
                showSuccess("База данных удалена: " + fileName);
                dbOpened = false;
                delete db;
                db = nullptr;
                refreshTable();
                updateStatusBar();
            } else {
                showError("Ошибка удаления базы данных");
            }
        }
    } else {
        showError("База данных не открыта");
    }
}

void MainWindow::createBackup()
{
    if (db && dbOpened) {
        // Автоматическое имя бэкапа: имя_базы_время.bak
        QString baseName = QString::fromStdString(db->getFileName());
        baseName = baseName.left(baseName.lastIndexOf('.')); // убираем .db
        QString backupName = baseName + "_backup_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".bak";
        
        QString fileName = QFileDialog::getSaveFileName(this, "Создать бекап", backupName, "Backup Files (*.bak);;All Files (*)");
        if (!fileName.isEmpty()) {
            if (!fileName.endsWith(".bak", Qt::CaseInsensitive)) {
                fileName += ".bak";
            }
            if (db->createBackup(fileName.toStdString())) {
                showSuccess("Бекап создан: " + fileName);
            } else {
                showError("Ошибка создания бекапа");
            }
        }
    } else {
        showError("База данных не открыта");
    }
}

void MainWindow::restoreFromBackup()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Восстановить из бекапа", "", "Backup Files (*.bak);;All Files (*)");
    if (!fileName.isEmpty()) {
        // Предлагаем выбрать куда сохранить восстановленную БД
        QString restoredName = QFileDialog::getSaveFileName(this, "Сохранить восстановленную БД как", "restored_database.db", "Database Files (*.db);;All Files (*)");
        if (restoredName.isEmpty()) {
            return;
        }
        
        if (!restoredName.endsWith(".db", Qt::CaseInsensitive)) {
            restoredName += ".db";
        }
        
        db = new Database(restoredName.toStdString());
        if (db->loadFromBackup(fileName.toStdString())) {
            dbOpened = true;
            showSuccess("База данных восстановлена из: " + fileName);
            refreshTable();
            updateStatusBar();
        } else {
            showError("Ошибка восстановления из бекапа");
            delete db;
            db = nullptr;
            dbOpened = false;
        }
    }
}



void MainWindow::addData()
{
    if (!db || !dbOpened) {
        showError("База данных не открыта");
        return;
    }
    
    QString name = nameEdit->text().trimmed();
    QString position = positionEdit->text().trimmed();
    QString salaryText = salaryEdit->text().trimmed();
    QString ageText = ageEdit->text().trimmed();
    
    if (name.isEmpty() || position.isEmpty() || salaryText.isEmpty() || ageText.isEmpty()) {
        showError("Заполните все поля!");
        return;
    }
    
    // Валидация зарплаты
    bool ok;
    double salary = salaryText.toDouble(&ok);
    if (!ok || salary < 0) {
        showError("Некорректная зарплата!");
        return;
    }
    
    // Валидация возраста
    int age = ageText.toInt(&ok);
    if (!ok || age < 0) {
        showError("Некорректный возраст!");
        return;
    }
    
    info emp;
    emp.id = db->getLastID() + 1; // Простая логика генерации ID
    emp.name = name.toStdString();
    emp.position = position.toStdString();
    emp.salary = salary;
    emp.age = age;
    
    if (db->addInfo(emp)) {
        showSuccess("Сотрудник добавлен: " + name);
        clearForm();
        refreshTable();
    } else {
        showError("Ошибка добавления сотрудника");
    }
}


void MainWindow::deleteData(){
    if (!db || !dbOpened) {
        showError("База данных не открыта");
        return;
    }
    
    QString field = deleteFieldCombo->currentText();
    QString value = deleteValueEdit->text().trimmed();
    
    if (value.isEmpty()) {
        showError("Введите значение для удаления!");
        return;
    }
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение", 
                                 "Вы уверены что хотите удалить записи?\nПоле: " + field + "\nЗначение: " + value,
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        bool success = false;
        
        if (field == "ID") {
            bool ok;
            int id = value.toInt(&ok);
            if (ok) {
                success = db->deleteById(id);
            }
        } else if (field == "ФИО") {
            success = db->deleteByName(value.toStdString());
        
        }
        if (success) {
            showSuccess("Записи успешно удалены");
            deleteValueEdit->clear();
            refreshTable();
        } else {
            showError("Записи не найдены или ошибка удаления");
        }
    
}
}
void MainWindow::searchData(){
    if (!db || !dbOpened) {
        showError("База данных не открыта");
        return;
    }
    
    QString field = searchFieldCombo->currentText();
    QString value = searchValueEdit->text().trimmed();
    
    if (value.isEmpty()) {
        refreshTable(); // Показываем все записи если поле поиска пустое
        return;
    }
    
    std::vector<info> results;
    
    if (field == "ID") {
        bool ok;
        int id = value.toInt(&ok);
        if (ok) {
            std::shared_ptr<info> emp = db->findById(id);
            if (emp) {
                results.push_back(*emp);
            }
        } else {
            showError("Некорректный ID");
            return;
        }
    } else if (field == "ФИО") {
        results = db->findByName(value.toStdString());
    }
    
    // Отображаем результаты
    displaySearchResults(results);
    statusLabel->setText("Найдено записей: " + QString::number(results.size()));
}


// void MainWindow::refreshTable(){
//     if(!db || !dbOpened){
//         tableWidget->setRowCount(0);
//         statusLabel->setText("База данных не открыта");
//         return;
//     }

//     // Получаем все данные из БД по ссылке
//     const std::vector<info>& allData = db->getAllData();
    
//     if (allData.empty()) {
//         tableWidget->setRowCount(0);
//         statusLabel->setText("Нет данных для отображения");
//         return;
//     }

   
//     displaySearchResults(allData);
//     statusLabel->setText("Загружено записей: " + QString::number(allData.size()));
// }
void MainWindow::exportToExcel(){
    if(!db || !dbOpened){
        showError("База данных не открыта");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Экспорт в Excel", "", "Excel Files (*.xlsx);;All Files (*)");
    if(!filename.isEmpty()){
        if(!filename.endsWith(".xlsx", Qt::CaseInsensitive)){
            filename += ".xlsx";
        }
        if(db->exportToXlsx(filename.toStdString())){
            showSuccess("Данные экспортированы в: " + filename);
        }else{
            showError("Ошибка экспорта");
        }
    }
}
void MainWindow::editData(){
    if (!db || !dbOpened) {
        showError("База данных не открыта");
        return;
    }
    
    // Проверяем, что ID валидный (не "auto")
    if (idEdit->text() == "auto" || idEdit->text().isEmpty()) {
        showError("Выберите запись для редактирования (двойной клик по таблице)");
        return;
    }
    
    QString name = nameEdit->text().trimmed();
    QString position = positionEdit->text().trimmed();
    QString salaryText = salaryEdit->text().trimmed();
    QString ageText = ageEdit->text().trimmed();
    
    if (name.isEmpty() || position.isEmpty() || salaryText.isEmpty() || ageText.isEmpty()) {
        showError("Заполните все поля!");
        return;
    }
    
    // Валидация
    bool ok;
    double salary = salaryText.toDouble(&ok);
    if (!ok || salary < 0) {
        showError("Некорректная зарплата!");
        return;
    }
    
    int age = ageText.toInt(&ok);
    if (!ok || age < 0) {
        showError("Некорректный возраст!");
        return;
    }
    
    int id = idEdit->text().toInt(&ok);
    if (!ok) {
        showError("Некорректный ID!");
        return;
    }
    
    // Создаем обновленный объект
    info emp;
    emp.id = id;
    emp.name = name.toStdString();
    emp.position = position.toStdString();
    emp.salary = salary;
    emp.age = age;
    
    // Обновляем в базе
    if (db->updateInfo(emp)) {
        showSuccess("Данные обновлены!");
        clearForm();
        refreshTable();
    } else {
        showError("Ошибка обновления данных!");
    }
}

void MainWindow::onTableCellDoubleClick(int row, int column){
    Q_UNUSED(column);
    QTableWidgetItem* idItem = tableWidget->item(row,0);
    QTableWidgetItem* nameItem = tableWidget->item(row,1);
    QTableWidgetItem* positionItem = tableWidget->item(row,2);
    QTableWidgetItem* salaryItem = tableWidget->item(row,3);
    QTableWidgetItem* ageItem = tableWidget->item(row,4);

    if (idItem && nameItem && positionItem && salaryItem && ageItem) {
        idEdit->setText(idItem->text());
        nameEdit->setText(nameItem->text());
        positionEdit->setText(positionItem->text());
        salaryEdit->setText(salaryItem->text());
        ageEdit->setText(ageItem->text());
        
        statusLabel->setText("Загружена запись ID: " + idItem->text());
    }
}


void MainWindow::onTableSelectionChanged()
{
    // При выборе строки активируем кнопки редактирования/удаления
    bool hasSelection = !tableWidget->selectedItems().isEmpty();
    editButton->setEnabled(hasSelection);
}

// Utility methods
void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, "Ошибка", message);
    statusLabel->setText("Ошибка: " + message);
}

void MainWindow::showSuccess(const QString &message)
{
    QMessageBox::information(this, "Успех", message);
    statusLabel->setText(message);
}





void MainWindow::updateStatusBar()
{
    if (db && dbOpened) {
        statusLabel->setText("База данных открыта: " + QString::fromStdString(db->getFileName()));
    } else {
        statusLabel->setText("База данных не открыта");
    }
}

void MainWindow::displaySearchResults(const std::vector<info> &results)
{
    tableWidget->setRowCount(results.size());
    
    for (size_t i = 0; i < results.size(); i++) {
        const info &emp = results[i];
        tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(emp.id)));
        tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(emp.name)));
        tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(emp.position)));
        tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(emp.salary)));
        tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(emp.age)));
    }
    
    tableWidget->resizeColumnsToContents();
}


void MainWindow::clearForm()
{
    idEdit->setText("auto");
    nameEdit->clear();
    positionEdit->clear();
    salaryEdit->clear();
    ageEdit->clear();
    statusLabel->setText("Форма очищена");
}


void MainWindow::onHeaderClicked(int column) {
    if (!db || !dbOpened) return;
    
    // Если кликнули на тот же столбец - меняем порядок сортировки
    if (currentSortColumn == column) {
        sortAscending = !sortAscending;
        sortContext.toggleOrder();
    } else {
        // Новый столбец - сбрасываем в восходящий порядок
        currentSortColumn = column;
        sortAscending = true;
        sortContext.setAscending(true);
    }
    
    // Устанавливаем стратегию в зависимости от столбца
    switch (column) {
        case 0: // ID
            sortContext.setStrategy(std::make_unique<SortById>());
            break;
        case 1: // Name
            sortContext.setStrategy(std::make_unique<SortByName>());
            break;
        case 2: // Position
            sortContext.setStrategy(std::make_unique<SortByPosition>());
            break;
        case 3: // Salary
            sortContext.setStrategy(std::make_unique<SortBySalary>());
            break;
        case 4: // Age
            sortContext.setStrategy(std::make_unique<SortByAge>());
            break;
        default:
            return;
    }
    
    // Обновляем отображение с сортировкой
    refreshTable();
    
    // Показываем текущий порядок сортировки в статусе
    QString order = sortAscending ? "↑" : "↓";
    statusLabel->setText(QString("Сортировка по %1 %2")
                        .arg(sortContext.getCurrentStrategyName().c_str())
                        .arg(order));

}
void MainWindow::refreshTable() {
    if (!db || !dbOpened) {
        tableWidget->setRowCount(0);
        statusLabel->setText("База данных не открыта");
        return;
    }

    try {
        // Получаем данные из БД
        auto allData = db->getAllData();
        
        // Проверяем что данные не пустые
        if (allData.empty()) {
            tableWidget->setRowCount(0);
            statusLabel->setText("Нет данных для отображения");
            return;
        }
        
        // 🔥 ПРИМЕНЯЕМ СОРТИРОВКУ к данным
        sortContext.sortData(allData);
        
        // Отображаем отсортированные данные
        displaySearchResults(allData);
        
        // Обновляем статус с информацией о сортировке
        QString order = sortContext.isAscending() ? "↑" : "↓";
        statusLabel->setText(QString("Записей: %1 | Сортировка: %2 %3")
                            .arg(allData.size())
                            .arg(sortContext.getCurrentStrategyName().c_str())
                            .arg(order));
                            
    } catch (const std::exception& e) {
        qCritical() << "Refresh table error:" << e.what();
        showError("Ошибка обновления таблицы");
        // Показываем данные без сортировки в случае ошибки
        if (db && dbOpened) {
            auto allData = db->getAllData();
            displaySearchResults(allData);
        }
    }
}