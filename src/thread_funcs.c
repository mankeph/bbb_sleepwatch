#include "thread_funcs.h"

int start_watch_thread(WTHR *thread, char sensnr, unsigned int delay_ms,
                       void *(*func)(void *)) {
  int response;

  if (sensnr == 3) {
    sensnr++;
  }

  thread->running = 1;
  thread->sensnr = sensnr;
  thread->delay = delay_ms;

  // build pthread attribute valiable
  pthread_attr_t thread_attr;
  pthread_attr_init(&thread_attr);

  if (sensnr == 2) {
    pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
    struct sched_param par;
    par.sched_priority = 1;
    pthread_attr_setschedparam(&thread_attr, &par);
  }

  // create the thread
  response =
      pthread_create(&(thread->t_id), &thread_attr, func, (void *)thread);

  // cleanup
  pthread_attr_destroy(&thread_attr);
  return response;
}

int start_other_thread(OTHR *thread, unsigned int delay_ms,
                       void *(*func)(void *)) {
  int response;

  // set thread data
  thread->running = 1;
  thread->delay = delay_ms;

  // build pthread attribute valiable
  pthread_attr_t thread_attr;
  pthread_attr_init(&thread_attr);

  // create the thread
  response =
      pthread_create(&(thread->t_id), &thread_attr, func, (void *)thread);

  // cleanup
  pthread_attr_destroy(&thread_attr);
  return response;
}

int stop_watch_thread(WTHR *thread) {
  // tell the thread to stop next time it runs through the loop
  pthread_mutex_lock(thread->spinlock);
  thread->running = 0;
  pthread_mutex_unlock(thread->spinlock);

  void **t_ret;

  // join with the thread
  return pthread_join(thread->t_id, t_ret);
}

int stop_other_thread(OTHR *thread) {

  // tell the thread to stop next time it runs through the loop
  pthread_mutex_lock(thread->spinlock);
  thread->running = 0;
  pthread_mutex_unlock(thread->spinlock);

  void **t_ret;

  // join with the thread
  return pthread_join(thread->t_id, t_ret);
}
