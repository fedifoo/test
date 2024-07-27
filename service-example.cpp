#include <iomanip>
#include <iostream>
#include <sstream>

#include <condition_variable>
#include <thread>

#include <vsomeip/vsomeip.hpp>

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678

#define SAMPLE_METHOD_ID 0x0421

std::shared_ptr< vsomeip::application > app;
std::mutex mutex;
std::condition_variable condition;

void run() {

  std::unique_lock<std::mutex> its_lock(mutex);
  condition.wait(its_lock);

while(true){
  std::shared_ptr< vsomeip::message > request;
  request = vsomeip::runtime::get()->create_request(true);
  request->set_service(SAMPLE_SERVICE_ID);
  request->set_instance(SAMPLE_INSTANCE_ID);
  request->set_method(SAMPLE_METHOD_ID);
 std::string input;
        std::cout << "Type 'on' or 'off': ";
        invalid:
        std::cin >> input;
        
        std::vector<vsomeip::byte_t> payload_value;
        if (input == "on") {
            payload_value = {0x01};
        } else if (input == "off") {
            payload_value = {0x00};
        } else {
            std::cout << "Invalid input. Please type 'on' or 'off'." << std::endl;
            goto invalid;
            
        }
  std::shared_ptr< vsomeip::payload > its_payload = vsomeip::runtime::get()->create_payload();
  
  its_payload->set_data(payload_value);
  request->set_payload(its_payload);
  app->send(request);
}}

void on_message(const std::shared_ptr<vsomeip::message> &_response) {

  std::shared_ptr<vsomeip::payload> payload = _response->get_payload();
    const vsomeip::byte_t* data = payload->get_data();
    vsomeip::length_t length = payload->get_length();

    if (length > 0) {
        vsomeip::byte_t value = data[0];
          std::cout << "\nthe response is : 0x" 
              << std::hex << std::setw(2) << std::setfill('0') 
              << static_cast<int>(value) 
              << std::endl;
        
    }
}

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "CLIENT: Service ["
            << std::setw(4) << std::setfill('0') << std::hex << _service << "." << _instance
            << "] is "
            << (_is_available ? "available." : "NOT available.")
            << std::endl;
    if(_is_available) condition.notify_one();
}

int main() {

    app = vsomeip::runtime::get()->create_application("commande");
    app->init();
    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
    std::thread sender(run);
    app->start();
}
