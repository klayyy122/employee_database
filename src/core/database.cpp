#include "database.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

// File operations
bool Database::create() {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    
    idKey.clear();
    last_id = 0;
    return true;
}

bool Database::open() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    idKey.clear();
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            info emp = info::deserialize(line);
            idKey[emp.id] = emp;  // O(1) вставка
        }
    }
    
    return true;
}

bool Database::removeDB() {
    idKey.clear();
    last_id = 0;
    return std::remove(filename.c_str()) == 0;
}

bool Database::clear() {
    idKey.clear();
    last_id = 0;
    
    std::ofstream file(filename, std::ios::trunc);
    return file.is_open();
}

bool Database::save() {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    
    // Получаем отсортированные данные для сохранения
    auto allData = getAllData();
    for (const auto& emp : allData) {
        file << emp.serialize() << "\n";
    }
    
    return true;
}

// CRUD operations
bool Database::addInfo(const info& data) {
    // Проверяем через hash map - O(1)
    if (idKey.find(data.id) != idKey.end()) {
        return false; // ID already exists
    }
    
    // Вставляем - O(1)
    idKey[data.id] = data;
    return true;
}

bool Database::deleteById(u_int64_t id) {
    // КОНСТАНТНОЕ ВРЕМЯ! O(1)
    return idKey.erase(id) > 0;
}

bool Database::deleteByName(const std::string& name) {
    std::vector<u_int64_t> ids_to_delete;
    
    // Собираем ID для удаления - O(n)
    for (const auto& pair : idKey) {
        if (pair.second.name == name) {
            ids_to_delete.push_back(pair.first);
        }
    }
    
    // Удаляем по ID - O(m) × O(1) = O(m)
    for (u_int64_t id : ids_to_delete) {
        idKey.erase(id);
    }
    
    return !ids_to_delete.empty();
}

bool Database::updateInfo(const info& data) {
    // Проверяем существует ли запись - O(1)
    if (idKey.find(data.id) == idKey.end()) {
        return false;
    }
    
    // Обновляем данные - O(1)
    idKey[data.id] = data;
    return true;
}

// Search operations
std::shared_ptr<info> Database::findById(u_int64_t id) const {
    auto it = idKey.find(id);  // O(1)
    if (it != idKey.end()) {
        return std::make_shared<info>(it->second);
    }
    return nullptr;
}

std::vector<info> Database::findByName(const std::string& name) const {
    std::vector<info> results;
    
    // Линейный поиск - O(n)
    for (const auto& pair : idKey) {
        if (pair.second.name == name) {
            results.push_back(pair.second);
        }
    }
    
    return results;
}

// Backup operations
bool Database::createBackup(const std::string& backupFile) {
    std::ofstream backup(backupFile, std::ios::trunc);
    if (!backup.is_open()) {
        return false;
    }
    
    auto allData = getAllData();
    for (const auto& emp : allData) {
        backup << emp.serialize() << "\n";
    }
    
    return true;
}

bool Database::loadFromBackup(const std::string& backupFile) {
    std::ifstream backup(backupFile);
    if (!backup.is_open()) {
        return false;
    }
    
    idKey.clear();
    
    std::string line;
    while (std::getline(backup, line)) {
        if (!line.empty()) {
            info emp = info::deserialize(line);
            idKey[emp.id] = emp;
        }
    }
    
    return save();
}

// Export operations
bool Database::exportToXlsx(const std::string& exportFile) {
    std::ofstream file(exportFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "ID,Name,Position,Salary,Age\n";
    
    auto allData = getAllData();
    for (const auto& emp : allData) {
        file << emp.id << ","
             << "\"" << emp.name << "\","
             << "\"" << emp.position << "\","
             << emp.salary << ","
             << emp.age << "\n";
    }
    
    return true;
}

// Utility
int Database::getLastID() const {
    int max_id = 0;
    for (const auto& pair : idKey) {
        if (pair.first > max_id) {
            max_id = pair.first;
        }
    }
    return max_id;
}

std::vector<info> Database::getAllData() const {
    std::vector<info> results;
    
    // Копируем данные в вектор - O(n)
    for (const auto& pair : idKey) {
        results.push_back(pair.second);
    }
    
    // Сортируем по ID для отображения - O(n log n)
    std::sort(results.begin(), results.end());
    
    return results;
}