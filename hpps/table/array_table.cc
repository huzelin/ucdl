/*!
 * \file array_table.cc
 * \brief The array table
 */
#include "hpps/table/array_table.h"

#include "hpps/common/io/io.h"
#include "hpps/common/log.h"
#include "hpps/updater/updater.h"

namespace hpps {

template <typename T>
ArrayWorker<T>::ArrayWorker(size_t size) : WorkerTable(), size_(size) {
  num_server_ = Zoo::Get()->num_servers();
  server_offsets_.push_back(0);
  CHECK(size_ > Zoo::Get()->num_servers());
  integer_t length = static_cast<integer_t>(size_) / Zoo::Get()->num_servers();
  for (auto i = 1; i < Zoo::Get()->num_servers(); ++i) {
    server_offsets_.push_back(i * length); // may not balance
  }
  server_offsets_.push_back(size_);
  LOG_DEBUG("worker %d create arrayTable with %d elements.",
             Zoo::Get()->rank(), size);
}

template <typename T>
ArrayWorker<T>::ArrayWorker(const ArrayTableOption<T> &option) :
    ArrayWorker<T>(option.size) {
}

template <typename T>
void ArrayWorker<T>::Get(T* data, size_t size) {
  CHECK(size == size_);
  data_ = data;
  integer_t all_key = -1;
  Blob whole_table(&all_key, sizeof(integer_t));
  WorkerTable::Get(whole_table);
  LOG_DEBUG("worker %d getting all parameters.", Zoo::Get()->rank());
}

template <typename T>
int ArrayWorker<T>::GetAsync(T* data, size_t size) {
  CHECK(size == size_);
  data_ = data;
  integer_t all_key = -1;
  Blob whole_table(&all_key, sizeof(integer_t));
  return WorkerTable::GetAsync(whole_table);
}

template <typename T>
void ArrayWorker<T>::Add(T* data, size_t size, const AddOption* option) {
  CHECK(size == size_);
  integer_t all_key = -1;

  Blob key(&all_key, sizeof(integer_t));
  Blob val(data, sizeof(T) * size);
  WorkerTable::Add(key, val, option);
  LOG_DEBUG("worker %d adding parameters with size of %d.\n",
             Zoo::Get()->rank(), size);
}

template <typename T>
int ArrayWorker<T>::AddAsync(T* data, size_t size, const AddOption* option) {
  CHECK(size == size_);
  integer_t all_key = -1;

  Blob key(&all_key, sizeof(integer_t));
  Blob val(data, sizeof(T) * size);
  return WorkerTable::AddAsync(key, val, option);
}

template <typename T>
int ArrayWorker<T>::Partition(const std::vector<Blob>& kv,
                              MsgType,
                              std::unordered_map<int, std::vector<Blob> >* out) {
  CHECK(kv.size() == 1 || kv.size() == 2 || kv.size() == 3);
  for (int i = 0; i < num_server_; ++i) (*out)[i].push_back(kv[0]);
  if (kv.size() >= 2) {
    CHECK(kv[1].size() == size_ * sizeof(T));
    for (int i = 0; i < num_server_; ++i) {
      Blob blob(kv[1].data() + server_offsets_[i] * sizeof(T),
        (server_offsets_[i + 1] - server_offsets_[i]) * sizeof(T));
      (*out)[i].push_back(blob);
      if (kv.size() == 3) {// update option blob
        (*out)[i].push_back(kv[2]);
      }
    }
  }
  return num_server_;
}

template <typename T>
void ArrayWorker<T>::ProcessReplyGet(std::vector<Blob>& reply_data) {
  CHECK(reply_data.size() == 2);
  int id = (reply_data[0]).As<int>();
  CHECK(reply_data[1].size<T>() == (server_offsets_[id + 1] - server_offsets_[id]));

  memcpy(data_ + server_offsets_[id], reply_data[1].data(), reply_data[1].size());
}

template <typename T>
ArrayServer<T>::ArrayServer(size_t size, const std::string& solver, const std::string& ps_mode) : ServerTable(ps_mode) {
  server_id_ = Zoo::Get()->server_rank();
  size_ = size / Zoo::Get()->num_servers();
  if (server_id_ == Zoo::Get()->num_servers() - 1) { // last server 
    size_ += size % Zoo::Get()->num_servers();
  }
  storage_.resize(size_);
  updater_.reset(Updater<T>::GetUpdater(size_, solver));
  LOG_INFO("server %d create ArrayTable with %d elements of %d elements(solver=%s ps_mode=%s).", 
           server_id_, size_, size, solver.c_str(), ps_mode.c_str());
}

template <typename T>
ArrayServer<T>::ArrayServer(const ArrayTableOption<T> &option) :
    ArrayServer<T>(option.size, option.solver, option.ps_mode) {
  ParamInitializer<T>::ResetRandomOption(option.random_option);
  this->random_.Gen(const_cast<T*>(storage_.data()), size_);
}

template <typename T>
void ArrayServer<T>::ProcessAdd(const std::vector<Blob>& data) {
  Blob keys = data[0], values = data[1];
  AddOption option;
  if (data.size() == 3)
    option.CopyFrom(data[2].data(), data[2].size());
  // Always request whole table
  CHECK(keys.size<integer_t>() == 1 && keys.As<integer_t>() == -1); 
  CHECK(values.size() == size_ * sizeof(T));
  T* pvalues = reinterpret_cast<T*>(values.data());
  updater_->Update(size_, storage_.data(), pvalues, &option);
}

template <typename T>
void ArrayServer<T>::ProcessGet(const std::vector<Blob>& data,
                                std::vector<Blob>* result) {
  size_t key_size = data[0].size<integer_t>();
  CHECK(key_size == 1 && data[0].As<integer_t>() == -1); 
  // Always request the whole table
  Blob key(sizeof(integer_t)); key.As<integer_t>() = server_id_;
  Blob values(sizeof(T) * size_);
  T* pvalues = reinterpret_cast<T*>(values.data());
  updater_->Access(size_, storage_.data(), pvalues);
  result->push_back(key);
  result->push_back(values);
}

template <typename T>
void ArrayServer<T>::Store(Stream* s) {
  s->Write(storage_.data(), storage_.size() * sizeof(T));
}

template <typename T>
void ArrayServer<T>::Load(Stream* s) {
  s->Read(storage_.data(), storage_.size() * sizeof(T));
}

HPPS_INSTANTIATE_CLASS_WITH_REAL_TYPE(ArrayWorker)
HPPS_INSTANTIATE_CLASS_WITH_REAL_TYPE(ArrayServer)

}  // namespace hpps

