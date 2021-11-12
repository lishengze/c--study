#include "kafka/KafkaConsumer.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    std::string brokers = "127.0.0.1:9092";
    kafka::Topic topic  = "test_topic";

    try {

        // Create configuration object
        kafka::Properties props ({
            {"bootstrap.servers",  brokers},
            {"enable.auto.commit", "true"}
        });

        // Create a consumer instance.
        kafka::clients::KafkaConsumer consumer(props);

        // Subscribe to topics
        consumer.subscribe({topic});

        consumer.seekToBeginning();
        
        // Read messages from the topic.
        std::cout << "\nReading messages from topic: " << topic << std::endl;
        kafka::Topics sub_topics = consumer.subscription();
        for (auto topic:sub_topics)
        {
            std::cout << "topic: " << topic << std::endl;
        }

        while (true) {
            
            auto records = consumer.poll(std::chrono::milliseconds(100));
            std::cout << "records: " << records.size() << std::endl;
            for (const auto& record: records) {
                // In this example, quit on empty message
                if (record.value().size() == 0) return 0;

                if (!record.error()) {
                    std::cout << "% Got a new message..." << std::endl;
                    std::cout << "    Topic    : " << record.topic() << std::endl;
                    std::cout << "    Partition: " << record.partition() << std::endl;
                    std::cout << "    Offset   : " << record.offset() << std::endl;
                    std::cout << "    Timestamp: " << record.timestamp().toString() << std::endl;
                    std::cout << "    Headers  : " << kafka::toString(record.headers()) << std::endl;
                    std::cout << "    Key   [" << record.key().toString() << "]" << std::endl;
                    std::cout << "    Value [" << record.value().toString() << "]" << std::endl;
                } else {
                    std::cerr << record.toString() << std::endl;
                }
            }
        }

        // consumer.close(); // No explicit close is needed, RAII will take care of it

    } catch (const kafka::KafkaException& e) {
        std::cerr << "% Unexpected exception caught: " << e.what() << std::endl;
    }
}

