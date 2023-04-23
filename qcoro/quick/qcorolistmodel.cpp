// SPDX-FileCopyrightText: 2023 Jonah Brüchert <jbb@kaidan.im>
//
// SPDX-License-Identifier: MIT

#include "qcorolistmodel.h"

namespace QCoro {

ListModel::ListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    auto *writableModel = const_cast<ListModel *>(this);

    // Case: We were asked for the data earlier, and we already have it
    if (auto itr = m_delayedData.find({ index, role }); itr != m_delayedData.cend()) {
        auto data = itr->second;

        // Delete again after use
        writableModel->m_delayedData.erase(itr);
        return data;
    }
    // Case: We don't have the data yet, and need to fetch it in the background.
    else {
        auto task = asyncData(index, role);
        task.then([=](auto &&data) {
            // Store received data
            writableModel->m_delayedData.insert({detail::ModelDataRef { index, role }, std::move(data)});

            // Ask view to reload the data for the role
            writableModel->dataChanged(index, index, {role});
        });

        // Return empty QVariant while we are fetching the actual data
        return {};
    }
}

} // namespace QCoro
