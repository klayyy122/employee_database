#pragma once
#include<iostream>
struct  info
{
    u_int64_t id;
    std::string name;
    std::string position;
    double salary;
    int age;

    bool operator<(const info& other) const noexcept {return this->id < other.id;}
    bool operator==(const info& other) const noexcept { return id == other.id; }
     std::string serialize() const {
        return std::to_string(id) + "|" + name + "|" + position + "|" + 
               std::to_string(salary) + "|" + std::to_string(age);
    }
      static info deserialize(const std::string& data) {
        info result;
        size_t pos = 0;
        size_t next_pos = 0;
        
        // ID
        next_pos = data.find('|', pos);
        result.id = std::stoull(data.substr(pos, next_pos - pos));
        pos = next_pos + 1;
        
        // Name
        next_pos = data.find('|', pos);
        result.name = data.substr(pos, next_pos - pos);
        pos = next_pos + 1;
        
        // Position
        next_pos = data.find('|', pos);
        result.position = data.substr(pos, next_pos - pos);
        pos = next_pos + 1;
        
        // Salary
        next_pos = data.find('|', pos);
        result.salary = std::stod(data.substr(pos, next_pos - pos));
        pos = next_pos + 1;
        
        // Age
        result.age = std::stoi(data.substr(pos));
        
        return result;
    }
};
