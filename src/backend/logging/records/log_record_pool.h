/*-------------------------------------------------------------------------
 *
 * log_record_pool.h
 * file description
 *
 * Copyright(c) 2015, CMU
 *
 * /peloton/src/backend/logging/records/log_record_pool.h
 *
 *-------------------------------------------------------------------------
 */

#pragma once

#include "tuple_record.h"
#include "backend/storage/abstract_backend.h"

namespace peloton {
namespace logging {

//===--------------------------------------------------------------------===//
// Linked list node for Tuple Record Log info
//===--------------------------------------------------------------------===//

typedef struct LogRecordNode {
  LogRecordType _log_record_type = LOGRECORD_TYPE_INVALID;
  // table id
  oid_t _table_oid;
  // inserted tuple location
  ItemPointer _insert_location;
  // deleted tuple location
  ItemPointer _delete_location;
  // database id
  oid_t _db_oid;
  // next node in the list
  LogRecordNode *next_node;
} LogRecordNode;

//===--------------------------------------------------------------------===//
// Log record list for a certain transaction
//===--------------------------------------------------------------------===//

class LogRecordList {

 public:
  ~LogRecordList() {
    Clear();
  }

  //===--------------------------------------------------------------------===//
  // Accessor
  //===--------------------------------------------------------------------===//

  void init(storage::AbstractBackend *backend) {
    assert(backend != nullptr);
    head_node = nullptr;
    next_list = nullptr;
    _backend = backend;
    commit = false;
    tail_node = nullptr;
    txn_id = INVALID_TXN_ID;
  }

  void Clear();

  bool IsEmpty() {
    return head_node == nullptr;
  }

  bool IsCommit() {
    return commit;
  }

  void SetCommit() {
    commit = true;
  }

  txn_id_t GetTxnID() {
    return txn_id;
  }

  void SetTxnId(txn_id_t id) {
    txn_id = id;
  }

  LogRecordNode* GetHeadNode() {
    return head_node;
  }

  LogRecordList* GetNextList() {
    return next_list;
  }

  void SetNextList(LogRecordList* next) {
    next_list = next;
  }

  int AddLogRecord(TupleRecord *record);

  //===--------------------------------------------------------------------===//
  // Member Variables
  //===--------------------------------------------------------------------===//
 private:
  LogRecordNode *head_node = nullptr;
  LogRecordList *next_list = nullptr;
  storage::AbstractBackend *_backend = nullptr;
  txn_id_t txn_id = INVALID_TXN_ID;
  bool commit = false;
  LogRecordNode *tail_node = nullptr; // TODO May need check&update after recovery.
};

//===--------------------------------------------------------------------===//
// Log record pool
//===--------------------------------------------------------------------===//

class LogRecordPool {

 public:
  ~LogRecordPool() {
    Clear();
  }

  //===--------------------------------------------------------------------===//
  // Accessor
  //===--------------------------------------------------------------------===//

  void init(storage::AbstractBackend *backend) {
    assert(backend != nullptr);
    head_list = nullptr;
    _backend = backend;
    tail_list = nullptr;
  }

  void Clear();

  bool IsEmpty() {
    return head_list == NULL;
  }

  LogRecordList* GetHeadList() {
    return head_list;
  }

  int CreateTxnLogList(txn_id_t txn_id);

  int AddLogRecord(TupleRecord *record);

  void RemoveTxnLogList(txn_id_t txn_id);

  LogRecordList* SearchRecordList(txn_id_t txn_id);

 private:

  void RemoveLogList(LogRecordList *prev, LogRecordList *node);

  //===--------------------------------------------------------------------===//
  // Member Variables
  //===--------------------------------------------------------------------===//
  LogRecordList *head_list = nullptr;
  storage::AbstractBackend *_backend = nullptr;
  LogRecordList *tail_list = nullptr; // TODO May need check&update after recovery.
};

}  // namespace logging
}  // namespace peloton