#include "../scc_algorithms.hpp"

#include <numeric>
#include <algorithm>
#include <list>
#include <queue>

#include <atomic>
#include <mutex>
#include <functional>
#include <iostream>



//////////////////////////////////////
///////////// UTILITIES //////////////
//////////////////////////////////////

// I made a few utilities that build around pthreads.
// This is mainly done for three reasons:
// 1. Less error-prone than using pthreads directly
// 2. More "C++ - like" code (which is my preference)
// 3. I can use it with minimal alterations to the
//    existing algorithm (using pthreads directly
//    would have been more intrusive).


struct Thread{
  // Wraps around a pthread
  // Can launch a new pthread and run f on it
  // I made this bc I'm afraid of pointers, so I'll never
  // have to see them ever again :)
  std::function<void()> f;
  pthread_t pid;

  Thread(std::function<void()> _f)
  :f(_f){
  }

  static void * pthread_call(void * p){
    Thread* tp = static_cast<Thread*>(p); // tp is a pointer to this Thread object
    tp->f();
    return NULL;
  }

  void launch(){
    // As a function, we pass pthread_call
    // As arguments, we pass a reference to this object.
    pthread_create(&pid, NULL, (void *(*)(void*))(Thread::pthread_call), (void *)(this)) ;
  }

  void join(){
    pthread_join(pid, NULL);
  }
};


struct WorkUnit{
  // A callable object that represents a unit of work
  std::function<void()> f;
  WorkUnit(std::function<void()> _f)
  :f(_f){
  }

  void operator()(){
    f();
  }
};


struct ThreadPool{ 
  unsigned int n_threads;
  std::vector<Thread> threads;
  std::vector<std::queue<WorkUnit>> work_queues;
  std::vector<std::mutex> queue_mutexes;

  static void run(ThreadPool* tp, unsigned int thread_id){
    // Every pthread run this function
    // Stat counters:
    int n_executed = 0;
    int n_stolen = 0;
    auto& my_queue = tp->work_queues[thread_id]; 
    auto& my_mutex = tp->queue_mutexes[thread_id];
    unsigned int n_threads = tp->n_threads;
    //Works on its own queue
    while(true){
      if(my_mutex.try_lock()){
        if(my_queue.empty()){ 
            my_mutex.unlock();
            break;
        }else{
            WorkUnit work = my_queue.front();
            my_queue.pop();
            tp->queue_mutexes[thread_id].unlock();
            work();
            n_executed++;
            // std::cout << "Thread " << thread_id << " completed a piece of work!" << std::endl;
        }
      }
    }
    // std::cout << "Thread " << thread_id << " finished its own work" << std::endl;
    
    //Finished own queue, start stealing work from the other queues
    bool any_work_exists = true;
    while(any_work_exists){
      any_work_exists = false;
      for(int i = 0; i<n_threads; i++){
        unsigned int neighbour_id = (thread_id + i)%n_threads;
        auto& neighbour_queue = tp->work_queues[neighbour_id];
        auto& neighbour_mutex = tp->queue_mutexes[neighbour_id];
        if(neighbour_mutex.try_lock()){
            if(neighbour_queue.empty()){ 
                neighbour_mutex.unlock();
                continue;
            }else{
                WorkUnit work = neighbour_queue.front();
                neighbour_queue.pop();
                tp->queue_mutexes[neighbour_id].unlock();
                any_work_exists = true;
                work();
                n_stolen++;
            }
          }
        //   std::cout << "Thread " << thread_id << " completed *stolen* work!" << std::endl;
        
      }
    }

    // std::cout << "Thread " << thread_id << ": n_executed=" << n_executed << ", n_stolen=" << n_stolen << std::endl;

  }

  ThreadPool(unsigned int n_threads)
  :work_queues(n_threads), queue_mutexes(n_threads){
    this->n_threads = n_threads;
    for(unsigned int i=0; i<n_threads; i++){
      threads.push_back(Thread(std::bind(run,this,i)));
    }
  }

  void launch(){
    for(auto& thread : threads){
      thread.launch();
    }
  }

  void join(){
    for(auto& thread : threads){
      thread.join();
    }
  }

  void schedule(WorkUnit w, unsigned int thread_id){
    work_queues[thread_id].push(w);
  }

};

template <typename F>
struct for_loop_partitioner{
  // creates a thread pool and distributes the work to 
  // the work queues 
  F& f;

  for_loop_partitioner(F& for_func, int i_begin, int i_end, int n_threads=8, int min_chunk_size = 4096)
  :f(for_func)
  {
    int total_elements = i_end-i_begin;
    int n_chunks = n_threads*4;
    int chunk_size = total_elements/n_chunks;

    //if work is small, do it sequentially right here;
    if(chunk_size < min_chunk_size){
        f(i_begin, i_end);
        return;
    }

    ThreadPool tp(n_threads); 
    int for_idx = i_begin;
    int i = 0;
    while(for_idx < i_end){ 
      std::function<void()> func = std::bind(std::ref(f),for_idx, std::min(for_idx+chunk_size, i_end));
      WorkUnit w(func);        
      tp.schedule(w, i%n_threads);
      for_idx += chunk_size;
      i++;
    }

    // ThreadPool::run(&tp, 0);
    tp.launch();
    //let the local thread join the fun :)
    tp.join();
  }
};





std::pair<std::vector<int>, int> ColoringSCCAlgorithm_PThreads(GraphCSC& graph) {
    // std::cout << "Starting coloring algorithm\n";
    unsigned int iteration_counter = 1;


    //scc_id of -1 means that the node hasn't been added to a SCC yet
    std::vector<int> scc_ids(graph.size, -1);
    std::atomic<int> max_ssc_id(0);

    //init queue

    std::vector<unsigned int> queue;

    std::vector<unsigned int> colors;
    std::mutex color_mutex;
    colors.resize(graph.size);
    queue.reserve(graph.size);

    for (unsigned int i = 0; i < graph.size; i++) {
        queue.push_back(i);
    }

    //start algorithm
    while (!queue.empty()) {
        // std::cout << "Starting iteration " << iteration_counter << "\n";
        //init colors
        for (auto& elem : queue) {
            colors[elem] = elem;
        }

        //color propagation
        bool any_changed_color = true;
        while (any_changed_color) {
            any_changed_color = false;

            auto f_color_propagation = [&](int v_idx_begin, int v_idx_end){
                for(int v_idx=v_idx_begin; v_idx<v_idx_end; v_idx++) {
                    auto& v = queue[v_idx];
                    //get all "u"s that point to this "v"
                    unsigned int u_idx_start = graph.vec_to_idx[v];
                    unsigned int u_idx_end = graph.vec_to_idx[v + 1];
                    for (unsigned int u_idx = u_idx_start; u_idx < u_idx_end; u_idx++) {
                        auto u = graph.vec_from[u_idx];
                        //color_mutex.lock();
                        if ((scc_ids[u] == -1) && (colors[v] > colors[u])) { //does the order here matter?
                            colors[v] = colors[u];
                            any_changed_color = true;
                        }
                        //color_mutex.unlock();
                    }
                }
            };
            for_loop_partitioner(f_color_propagation, 0, queue.size());
        }
        //color propagation finished
        //From every node where node_id == node_color, start BFS
        std::mutex mut;

        auto f_bfs = [&](int v_idx_begin, int v_idx_end){

            for (int v_idx=v_idx_begin; v_idx<v_idx_end; v_idx++) {
            //for (int v_idx=0; v_idx<queue.size(); v_idx++) {
            
                auto v = queue[v_idx];
                if (colors[v] == v){
                    unsigned int curr_color = colors[v];
                    int curr_scc_id = max_ssc_id.fetch_add(1);
                    //BFS from v to its predecessors of the same color, and assign
                    //all visited nodes to the SCC
                    std::queue<unsigned int> bfs_queue;
                    bfs_queue.push(v);
                    scc_ids[v] = curr_scc_id;
                    while (!bfs_queue.empty()) {
                        unsigned int curr_node = bfs_queue.front();
                        bfs_queue.pop();
                        //get predecessors
                        unsigned int u_idx_start = graph.vec_to_idx[curr_node];
                        unsigned int u_idx_end = graph.vec_to_idx[curr_node + 1];
                        for (unsigned int u_idx = u_idx_start; u_idx < u_idx_end; u_idx++) {
                            auto u = graph.vec_from[u_idx];
                            // mut.lock();
                            if (scc_ids[u] == -1 && colors[u] == curr_color) {
                                bfs_queue.push(u);
                                scc_ids[u] = curr_scc_id;
                            }
                            // mut.unlock();
                        }
                    }
                }
            }
        }; // end of f_bfs definition
        for_loop_partitioner(f_bfs, 0, queue.size());

        // std::cout << "Completed iteration " << iteration_counter++ << "\n";
        //update queue so that it only contains nodes where scc_id[node]==-1
        queue.erase(
            std::remove_if(queue.begin(), queue.end(), [&scc_ids](auto& elem) { return scc_ids[elem] != -1; }),
            queue.end());
    }

    return {std::move(scc_ids), max_ssc_id}; //make a pair by moving (aka not copying) the vector into it
}
