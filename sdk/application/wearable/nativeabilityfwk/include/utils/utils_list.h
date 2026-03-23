/*
 * Copyright (c) CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NATIVEABILITYFWK_UTILS_LIST_H
#define NATIVEABILITYFWK_UTILS_LIST_H

#include <cstdint>

namespace OHOS {
template<typename T>
class ListsNode {
public:
    ListsNode() = default;
    ListsNode(T value) : value_(value), next_(nullptr), prev_(nullptr) {}

    T value_;
    ListsNode<T> *next_;
    ListsNode<T> *prev_;
};

template<typename T>
class Lists {
public:
    Lists() : count_(0)
    {
        head_ = new ListsNode<T>();
        head_->next_ = head_;
        head_->prev_ = head_;
    }

    ~Lists()
    {
        RemoveAll();
        delete head_;
        head_ = nullptr;
    }

    const T Front() const
    {
        if (count_ == 0) {
            return head_->value_;
        }

        return head_->next_->value_;
    }

    void PushFront(T value)
    {
        auto node = new ListsNode<T>(value);
        if (node == nullptr) {
            return;
        }

        node->prev_ = head_;
        node->next_ = head_->next_;
        head_->next_->prev_ = node;
        head_->next_ = node;
        count_++;
    }

    void PopFront()
    {
        if (count_ == 0) {
            return;
        }

        ListsNode<T> *node = head_->next_;
        node->next_->prev_ = head_;
        head_->next_ = node->next_;
        delete node;
        count_--;
    }

    const T Back() const
    {
        if (count_ == 0) {
            return head_->value_;
        }

        return head_->prev_->value_;
    }

    void PushBack(T value)
    {
        auto node = new ListsNode<T>(value);

        node->next_ = head_;
        node->prev_ = head_->prev_;
        head_->prev_->next_ = node;
        head_->prev_ = node;
        count_++;
    }

    void PopBack()
    {
        if (count_ == 0) {
            return;
        }

        ListsNode<T> *node = head_->prev_;
        node->prev_->next_ = head_;
        head_->prev_ = node->prev_;
        delete node;
        count_--;
    }

    void Remove(ListsNode<T> *node)
    {
        if ((count_ == 0) || (node == nullptr)) {
            return;
        }

        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;

        delete node;
        count_--;
    }

    void RemoveAll()
    {
        ListsNode<T> *node = head_->next_;
        while (node != head_) {
            ListsNode<T> *temp = node;
            node = node->next_;
            delete temp;
        }
        head_->next_ = head_;
        head_->prev_ = head_;
        count_ = 0;
    }

    ListsNode<T> *Begin() const
    {
        return head_->next_;
    }

    const ListsNode<T> *End() const
    {
        return head_;
    }

    uint16_t Size() const
    {
        return count_;
    }

    bool IsEmpty() const
    {
        return count_ == 0;
    }

private:
    ListsNode<T> *head_;
    uint16_t count_;
};
} // namespace OHOS
#endif  // NATIVEABILITYFWK_UTILS_LIST_H
