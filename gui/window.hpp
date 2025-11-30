#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include<QWidget>
#include<QApplication>
#include "src/core/database.hpp"
#include"src/core/sorting/sorting.hpp"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(){
        delete db;
    }

private slots:
    void onHeaderClicked(int column);
    void displaySearchResults(const std::vector<info> &results);
    // File operations
    void createDatabase();
    void openDatabase();
    void saveDatabase();
    void clearDatabase();
    void deleteDatabase();
    
    // Backup operations
    void createBackup();
    void restoreFromBackup();
    
    // Data operations
    void addData();
    void deleteData();
    void searchData();
    void editData();
    void refreshTable();
    
    // Export
    void exportToExcel();
    void clearForm();
    // Table interactions
    void onTableSelectionChanged();
    void onTableCellDoubleClick(int row, int column);

private:
    void setupUI();
    void setupMenu();
    void setupToolbar();
    void setupMainArea();
    void setupStatusBar();
    void connectSignals();
    
    void showError(const QString& message);
    void showSuccess(const QString& message);
    void updateStatusBar();
    
    // Database
    Database* db;
    bool dbOpened;
    
    // Main widgets
    QTableWidget* tableWidget;
    
    // Input fields
    QLineEdit* idEdit;
    QLineEdit* nameEdit;
    QLineEdit* positionEdit;
    QLineEdit* salaryEdit;
    QLineEdit* ageEdit;
    
    // Search widgets
    QComboBox* searchFieldCombo;
    QLineEdit* searchValueEdit;
    QComboBox* deleteFieldCombo;
    QLineEdit* deleteValueEdit;
    
    // Buttons
    QPushButton* addButton;
    QPushButton* deleteButton;
    QPushButton* searchButton;
    QPushButton* editButton;
    QPushButton* refreshButton;
    QPushButton* backupButton;
    QPushButton* restoreButton;
    QPushButton* exportButton;
    QPushButton* clearFormButton;
    QPushButton* showAllButton;
    

    SortContext sortContext;
    int currentSortColumn = 0;
    bool sortAscending = true;

    // Status
    QLabel* statusLabel;
   
};
