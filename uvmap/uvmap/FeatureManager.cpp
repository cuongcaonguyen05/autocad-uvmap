// FeatureManager.cpp : Hiện thực class FeatureManager.
#include "pch.h"
#include "FeatureManager.h"

bool FeatureManager::Register(std::unique_ptr<IFeature> feature)
{
    if (!feature)
        return false;

    if (Has(feature->Name()))
        return false;  // không cho trùng tên

    m_features.push_back(std::move(feature));
    return true;
}

int FeatureManager::Run(const std::string& name)
{
    IFeature* f = Find(name);
    if (f == nullptr)
        return -1;  // không tìm thấy tính năng

    return f->Execute();
}

bool FeatureManager::Has(const std::string& name) const
{
    return Find(name) != nullptr;
}

std::vector<std::string> FeatureManager::Names() const
{
    std::vector<std::string> names;
    names.reserve(m_features.size());
    for (const auto& f : m_features)
        names.push_back(f->Name());
    return names;
}

IFeature* FeatureManager::Find(const std::string& name) const
{
    for (const auto& f : m_features) {
        if (f->Name() == name)
            return f.get();
    }
    return nullptr;
}
