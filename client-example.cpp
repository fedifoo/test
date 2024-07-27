#include <iomanip>
#include <iostream>
#include <sstream>
#include <condition_variable>
#include <thread>
#include <vsomeip/vsomeip.hpp>
#include <chrono>


#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678

#define SAMPLE_EVENTGROUP_ID 0x4465
#define SAMPLE_EVENT_ID 0x8778

std::shared_ptr<vsomeip::application> app;
std::set<vsomeip::eventgroup_t> its_groups;
std::mutex mutex;
std::condition_variable condition;

void run() {
std::unique_lock<std::mutex> its_lock(mutex);
  condition.wait(its_lock);


  

  std::set<vsomeip::eventgroup_t> its_groups;
  its_groups.insert(SAMPLE_EVENTGROUP_ID);
  app->request_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups);
  

  
  app->subscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);
 
  

}
void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "CLIENT: Service ["
            << std::setw(4) << std::setfill('0') << std::hex << _service << "." << _instance
            << "] is "
            << (_is_available ? "available." : "NOT available.")
            << std::endl;
    condition.notify_one();
}

void on_event(const std::shared_ptr<vsomeip::message>& _message) {
    std::shared_ptr<vsomeip::payload> its_payload = _message->get_payload();
    vsomeip::length_t length = its_payload->get_length();
    const vsomeip::byte_t* data = its_payload->get_data();

    if (length >= 1) {
        int received_value = static_cast<int>(data[0]);
        std::cout << "led_status: " << received_value << std::endl;
    } else {
        std::cout << "Received invalid payload." << std::endl;
    }
}

int main() {
   app = vsomeip::runtime::get()->create_application("affiche");
    app->init();
    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);

    app->register_message_handler(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE, vsomeip::ANY_METHOD, on_event);

    std::thread sender(run);
    app->start();
}

