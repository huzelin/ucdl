/*!
 * \file feeder.h
 * \brief The feeder for i/o reading.
 */
#pragma once

#include "hpps/common/thread_pool.h"
#include "hpps/feeder/plan.h"
#include "hpps/feeder/batch.h"
#include "hpps/feeder/blocking_queue.h"

namespace hpps {

class Feeder {
 public:
  static Feeder* Get() { static Feeder feeder; return &feeder; }
  virtual ~Feeder();

  void Start(int thread_num = 10);

  // Schedule one data source's sample.
  BlockingQueueIterator<std::shared_ptr<Batch>>* Schedule(
      Plan* plan, int max_queue_size = 1);

 protected:
  void Run(int tid);

  // Each entry mean one data source.
  struct Entry {
    Plan* plan;
    BlockingQueue<std::shared_ptr<Batch>>* blocking_queue;
  };
  void AddTask(const Entry& entry);

  // The task in data source.
  struct Task {
    size_t curr;
    Plan::Block block;
    BlockingQueue<std::shared_ptr<Batch>>* blocking_queue;
  };
  
  // Produce one batch
  void ProduceBatch(Queue<Task>* queue);
  // Assemble tensors into Batch 
  int AssembleBatch(Stream* stream, Plan* plan, std::shared_ptr<Batch>& batch);
  // Read batch tensor
  std::vector<std::vector<Tensor*>> ReadBatchTensor(Stream* stream, Plan* plan);
  // Assemble batch
  void AssembleBatch(std::vector<std::vector<Tensor*>>& batch_tensor,
                     Plan* plan,
                     std::shared_ptr<Batch>& batch);

  ThreadPool* thread_pool_;
  std::vector<Entry> entries_;
  std::vector<Queue<Task>*> task_queues_;
  std::mutex mutex_;
  mutable bool running_;
};

}  // namespace hpps