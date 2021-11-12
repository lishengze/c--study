
#include "../include/global_declare.h"

int main(int argc, char **argv)
{
    using namespace kafka::clients;

    std::string brokers = "127.0.0.1:9092";
    kafka::Topic topic  = "test_topic";


    try {

        // Create configuration object
        kafka::Properties props ({
            {"bootstrap.servers",  brokers},
            {"enable.idempotence", "true"},
        });

        // Create a producer instance.
        KafkaProducer producer(props);

        // Read messages from stdin and produce to the broker.
        std::cout << "% Type message value and hit enter to produce message. (empty line to quit)" << std::endl;

        for (int i = 0; i < 100; ++i)
        {
            std::string line = "Cpp_" + utrade::pandora::NanoTimeStr();

            cout << "send line: " << line << endl;
            // The ProducerRecord doesn't own `line`, it is just a thin wrapper
            auto record = producer::ProducerRecord(topic,
                                                   kafka::NullKey,
                                                   kafka::Value(line.c_str(), line.size()));
            // Send the message.
            producer.send(record,
                          // The delivery report handler
                          [](const producer::RecordMetadata& metadata, const kafka::Error& error) {
                              if (!error) {
                                  std::cout << "% Message delivered: " << metadata.toString() << "\n" << std::endl;
                              } else {
                                  std::cerr << "% Message delivery failed: " << error.message() << "\n" << std::endl;
                              }
                          },
                          // The memory block given by record.value() would be copied
                          KafkaProducer::SendOption::ToCopyRecordValue);

            std::this_thread::sleep_for(std::chrono::seconds(3));

            if (line.empty()) break;
        }

        // producer.close(); // No explicit close is needed, RAII will take care of it

    } catch (const kafka::KafkaException& e) {
        std::cerr << "% Unexpected exception caught: " << e.what() << std::endl;
    }
}

