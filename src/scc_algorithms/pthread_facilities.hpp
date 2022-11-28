#include <functional>
#include <iostream>
#include <mutex>

// I made a few facilities that wrap around pthreads.
// This is mainly done for three reasons:
// 1. Less error-prone than using pthreads directly
// 2. More "C++ - like" code (which is my preference)
// 3. I can use it with minimal alterations to the
//    existing algorithm (using pthreads directly
//    would have been more intrusive).
// There might be some small additional overhead from this
// approach, but it performs very adequately for our purposes.



struct Thread{
  // Wraps around a pthread
  // Can launch a new pthread and run f on it
  // This way, I avoid dealing with function pointers ;)
  std::function<void()> f;
  pthread_t pid;

  Thread(std::function<void()> _f)
  :f(_f){
  }

  static void * pthread_call(void * p){
    // Entry point function for new pthreads
    Thread* tp = static_cast<Thread*>(p);
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

  for_loop_partitioner(F& for_func, int i_begin, int i_end, int n_threads=8)
  :f(for_func)
  {
        int total_elements = i_end-i_begin;
        int n_chunks = std::min(n_threads*8, total_elements);
        int chunk_size = total_elements/n_chunks;

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

        tp.launch();
        //let the local thread join the fun :)
        ThreadPool::run(&tp, 0);
        tp.join();
    }

};
