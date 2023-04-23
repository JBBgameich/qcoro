// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QAbstractListModel>

#include <QCoro/QCoroTask>

#include <functional>

namespace QCoro::detail {
// Identifies information in the model by index and role
struct ModelDataRef {
    QModelIndex index;
    int role;

    bool operator==(const ModelDataRef &other) const = default;
};
}
namespace std {
template <>
class hash<QCoro::detail::ModelDataRef>
{
public:
    std::uint64_t operator()(const QCoro::detail::ModelDataRef& ref) const
    {
        return qHash(ref.index) ^ std::hash<int>{}(ref.role);
    }
};
}

namespace QCoro {

class ListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ListModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

    virtual QCoro::Task<QVariant> asyncData(const QModelIndex &index, int role) const = 0;

private:
    std::unordered_map<QCoro::detail::ModelDataRef, QVariant> m_delayedData;
};

} // namespace QCoro
