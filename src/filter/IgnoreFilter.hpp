#pragma once

#include <filesystem>
#include <string>
#include <vector>

class IgnoreFilter
{
public:
    IgnoreFilter();
    explicit IgnoreFilter(const std::filesystem::path& gitignorePath);
    ~IgnoreFilter();

    void AddRule(const std::string& rule);
    [[nodiscard]] bool ShouldIgnore(const std::filesystem::path& path) const;

private:
    std::vector<std::string> m_rules;
};