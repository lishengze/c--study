
from kafka import KafkaProducer
import datetime
import time
import os
import json
import str

def get_datetime_str():
    return datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')

def test_producer():
    try:
        producer = KafkaProducer(bootstrap_servers=['127.0.0.1:9093'], key_serializer=str.encode)
        if producer.bootstrap_connected():
            msg = "python producer " + get_datetime_str()
            msg.encode('utf-8')

            topic_name = 'test_topic'

            print("send: " + msg)
            producer.send(topic_name, value=msg, key=None, headers=None, partition=None, timestamp_ms=None)

            # future = producer.send(topic_name, value=b'another msg',key=b'othermsg')

            # result = future.get(timeout=60)

            # print(result)


    except Exception as e:
        print("exception: " + str(e))

def test_main():
    test_producer()

if __name__ == "__main__":
    test_main()