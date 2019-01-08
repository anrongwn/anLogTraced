#pragma once
#include <mutex>

namespace an {


	//��������
#define AN_RECONNECT_COUNT	(1000)
	class an_waitconnect_semaphore {
	public:
		an_waitconnect_semaphore(const an_waitconnect_semaphore&) = delete;
		an_waitconnect_semaphore& operator=(const an_waitconnect_semaphore&) = delete;

		//Ĭ�����ź�
		an_waitconnect_semaphore() : count{ 0 }, status{ 0 } {
		}
		/*
		//Ĭ�����ź�
		explicit an_waitconnect_semaphore(int value = 0) : count{ value }, status{ 0 } {
		}
		*/
		~an_waitconnect_semaphore() {

		}

		void reset() {
			count = 0;
			status = 0;
		}

		int get_count() {
			return count;
		}

		int get_status() {
			return status;
		}

		void signal_once() {
			std::lock_guard<std::mutex>lock{ mutex };
			if (++count >= AN_RECONNECT_COUNT) {// ��������
				status = 2;
				condition.notify_one();// notify one !
			}
		}

		void signal() {
			std::lock_guard<std::mutex>lock{ mutex };
			count = 1;
			status = 1;
			condition.notify_one();// notify one !
		}

		void wait() {
			std::unique_lock<std::mutex>lock{ mutex };
			if (count < AN_RECONNECT_COUNT) {//��������
				condition.wait(lock, [&]()->bool {return status > 0; });// suspend and wait ...
																	  //--status;// ok, me wakeup !

			}
		}


	private:
		std::mutex mutex;
		std::condition_variable condition;
		volatile int count;	//���Ӵ��������ó���2��
		volatile int status; //����״̬��0--δ�����ӣ�1--���ӳɹ���2--���ӳ�ʱ��ʧ�ܣ�
	};

}