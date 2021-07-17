import requests
import uuid
import datetime
import time



def test_b2c2_insert_order():

    api_token = 'eabe0596c453786c0ecee81978140fad58daf881'
    headers = {'Authorization': 'Token %s' % api_token,
               'Content-Type': 'application/json'}

    uuid = str("112")
    quantity = '30'
    side = 'buy'
    instrument = 'XRPUSD.SPOT'
    price = '0.651'
    valid_until = datetime.datetime.utcfromtimestamp(time.time() + 10).strftime("%Y-%m-%dT%H:%M:%S")
    executing_unit = 'risk-adding-strategy'


    post_data = {
        'instrument': instrument,
        'side': side,
        'quantity': quantity,
        'client_order_id': uuid,
        'price': price,
        'order_type': 'FOK',
        'valid_until': valid_until,
        'executing_unit': executing_unit,
    }

    response = requests.post('https://api.uat.b2c2.net/order/', json=post_data, headers=headers)

    print("\n\n---- response.request.headers ----")
    print(response.request.headers)

    print("\n\n----- response.request.body -----");
    print(response.request.body)

    print("\n\n---- response.headers ----")
    for item in response.headers:
        print("%s : %s" % (str(item), str(response.headers[item])))
    # print(response.headers)

    print("\n\n---- response.content ----")
    print(response.content)

if __name__ == "__main__":
    # test_websocket()
    test_b2c2_insert_order()

