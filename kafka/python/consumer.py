from kafka import KafkaProducer
from kafka import KafkaConsumer
from kafka import TopicPartition

import datetime
import time
import os
import json

from kafka.consumer import group

def get_datetime_str():
    return datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')

def test_producer():
    try:
        producer = KafkaProducer(bootstrap_servers=['127.0.0.1:9093'])
        if producer.bootstrap_connected():
            topic_name = 'test_topic'

            # msg = "python producer " + get_datetime_str()
            # msg.encode('utf-8')
            # print("send: " + msg)
            # producer.send(topic_name, value=msg, key=None, headers=None, partition=None, timestamp_ms=None)

            i = 1
            while i <= 5:
                i += 1
                # msg = "producer_%d" % i
                msg = "producer_%s" % get_datetime_str()

                print(msg)
                # producer.send(topic_name, value=b"test_again")
                producer.send(topic_name, value=bytes(msg.encode()))
                time.sleep(3)

    except Exception as e:
        print("exception: " + str(e))

def test_consumer():
    try:
        topic_name = 'test_topic'
        server_list = ['127.0.0.1:9117']

        consumer = KafkaConsumer(bootstrap_servers=server_list,
                                 auto_offset_reset='latest',
                                 group_id="test_consumer_1")


        # consumer = KafkaConsumer(topic_name,
        #                         bootstrap_servers=server_list,
        #                         auto_offset_reset='latest',# 消费kafka中最近的数据，如果设置为earliest则消费最早的数据，不管这些数据是否消费
        #                         enable_auto_commit=True, # 自动提交消费者的offset
        #                         auto_commit_interval_ms=3000, ## 自动提交消费者offset的时间间隔
        #                         group_id='MY_GROUP1',
        #                         consumer_timeout_ms= 10000, # 如果10秒内kafka中没有可供消费的数据，自动退出
        #                         client_id='consumer-python3'
        #                         )

        if consumer.bootstrap_connected():            
            print("consumer connect server: %s successfully!" % (str(server_list)))
            print(consumer.topics())
            consumer.subscribe(topics=topic_name)

            print(consumer.subscription())

            i = 0
            for msg in consumer:
                print(i)
                i+=1
                print(msg.value)
        else:
            print("consumer connect server: %s failed!" % (str(server_list)))

            # print(consumer.metrics())


    except Exception as e:
        print("exception: " + str(e))

def test_client():
    pass

def test_main():
    # test_producer()
    test_consumer()

if __name__ == "__main__":
    test_main()