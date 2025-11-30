#pragma once
#include <memory>
#include "info.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

class Database {
private:
    std::unordered_map<int, info> idKey;  // Основное хранилище O(1)
    std::string filename;
    mutable int last_id;

public:
    Database(const std::string& file) : filename(file), last_id(0) {}

    // File operations
    bool create();
    bool open();
    bool removeDB();
    bool clear();
    bool save();
    
    // CRUD operations
    bool addInfo(const info& data);
    bool deleteById(u_int64_t id);          // O(1) - константное время!
    bool deleteByName(const std::string& name);
    bool updateInfo(const info& data);
    
    // Search operations
    std::shared_ptr<info> findById(u_int64_t id) const;    // O(1)
    std::vector<info> findByName(const std::string& name) const;
    
    // Backup operations
    bool createBackup(const std::string& backupFile);
    bool loadFromBackup(const std::string& backupFile);
    
    // Export operations
    bool exportToXlsx(const std::string& exportFile);
    
    // Utility
    std::string getFileName() const { return filename; }
    int getLastID() const;
    std::vector<info> getAllData() const;  // Возвращает вектор вместо set
};