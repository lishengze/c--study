#include "kafka/KafkaConsumer.h"
#include "kafka/KafkaConsumer.h"
#include "kafka/KafkaProducer.h"
#include "kafka/AdminClient.h"

#include <iostream>
#include <string>
using namespace std;


kafka::Topics get_created_topics(kafka::clients::AdminClient& adclient)
{
    kafka::Topics result;

    try
    {
        auto listResult = adclient.listTopics();
        
        if (listResult.error)
        {
            cout << listResult.error.message() << endl;
        }
        else
        {
            result = listResult.topics;

            for (auto topic:result)
            {
                cout << topic << "\t";
            }
            cout << endl;

            return result;
        }    

        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return result;
}

void test_create_topic()
{
    try
    {
        kafka::Properties adclient_props ({
            {"bootstrap.servers",  "127.0.0.1:9092"}
        });    

        kafka::clients::AdminClient adclient(adclient_props);

        cout << "original topics: " << endl;
        kafka::Topics original_topics = get_created_topics(adclient);

        // kafka::Properties topic_props ({
        //     {"max.message.bytes",  "1048588"}
        // });

        kafka::Properties topic_props;

        auto createResult = adclient.createTopics({"Cplus_Topic"}, 3, 3, topic_props);
        if (createResult.error)
        {
            cout << "Create Topic Error: " + createResult.error.message() << endl;
            return;
        }

        cout << "new topics: " << endl;
        kafka::Topics new_topics = get_created_topics(adclient);        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

int test_consumer()
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

int main(int argc, char **argv)
{
    test_create_topic();

    return 1;
}

