#pragma once

// Copyright 2021 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "ledger/LedgerManager.h"
#include "transactions/TransactionFrame.h"
#include "util/UnorderedMap.h"
#include "util/UnorderedSet.h"

namespace stellar
{
class QueueLimiterTxMap;
class TxQueueLimiter
{
    // size of the transaction queue, in operations
    size_t mQueueSizeOps{0};
    // number of ledgers we can pool in memory
    int const mPoolLedgerMultiplier;
    LedgerManager& mLedgerManager;

    // all known transactions
    std::unique_ptr<QueueLimiterTxMap> mTxs;
    TransactionFrameBasePtr getWorstTransaction();

  public:
    TxQueueLimiter(int multiplier, LedgerManager& lm);
    ~TxQueueLimiter();

    size_t
    size() const
    {
        return mQueueSizeOps;
    }
    size_t maxQueueSizeOps() const;

    void addTransaction(TransactionFrameBasePtr const& tx);
    void removeTransaction(TransactionFrameBasePtr const& tx);

    // evict the worst transactions until there
    // is enough capacity to insert ops operations
    // by calling `evict` - note that evict must call `removeTransaction`
    // as to make space
    // returns false if it ran out of transactions before
    // reaching its goal
    bool evictTransactions(size_t ops,
                           std::function<void(TransactionFrameBasePtr const&)> evict);

    // oldTx is set when performing a replace by fee
    bool canAddTx(TransactionFrameBasePtr const& tx,
                  TransactionFrameBasePtr const& oldTx) const;

    void reset();
};
}
