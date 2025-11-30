#pragma once
#include<iostream>
#include<algorithm>
#include<functional>
#include<memory>
#include"../info.hpp"
class Sort{
    public:
    virtual ~Sort() noexcept = default;
    virtual bool sortingCriteria(const info& first, const info& second) = 0;
    bool operator()(const info& first, const info& second) {
        return sortingCriteria(first, second);
    }
};

class SortById final : public Sort {
public:
    bool sortingCriteria(const info& first, const info& second) override {
        return first.id < second.id;
    }
};

class SortByName final : public Sort {
public:
    bool sortingCriteria(const info& first, const info& second) override {
        return first.name < second.name;
    }
};

class SortByPosition final : public Sort {
public:
    bool sortingCriteria(const info& first, const info& second) override {
        return first.position < second.position;
    }
};

class SortByAge final : public Sort {
public:
    bool sortingCriteria(const info& first, const info& second) override {
        return first.age < second.age;
    }
};

class SortBySalary final : public Sort {
public:
    bool sortingCriteria(const info& first, const info& second) override {
        return first.salary < second.salary;
    }
};


class SortContext {
private:
    std::unique_ptr<Sort> strategy;
    bool ascending = true;

public:
    void setStrategy(std::unique_ptr<Sort> newStrategy) {
        strategy = std::move(newStrategy);
    }
    
    void setAscending(bool isAscending) {
        ascending = isAscending;
    }
    
    void toggleOrder() {
        ascending = !ascending;
    }
    
    bool isAscending() const {
        return ascending;
    }
    
    // Сортировка вектора данных
    void sortData(std::vector<info>& data) const {
        if (!strategy) return;
        
        if (ascending) {
            std::sort(data.begin(), data.end(), [this](const info& a, const info& b) {
                return strategy->sortingCriteria(a, b);
            });
        } else {
            std::sort(data.begin(), data.end(), [this](const info& a, const info& b) {
                return !strategy->sortingCriteria(a, b);
            });
        }
    }
    
    // Получение имени текущей стратегии (для отображения в UI)
    std::string getCurrentStrategyName() const {
        if (!strategy) return "None";
        
        if (dynamic_cast<SortById*>(strategy.get())) return "ID";
        if (dynamic_cast<SortByName*>(strategy.get())) return "Name";
        if (dynamic_cast<SortByPosition*>(strategy.get())) return "Position";
        if (dynamic_cast<SortByAge*>(strategy.get())) return "Age";
        return "Unknown";
    }
};