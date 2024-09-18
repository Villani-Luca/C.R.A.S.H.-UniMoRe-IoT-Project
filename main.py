import time
import paho.mqtt.client as paho
from paho import mqtt

broker = "broker.hivemq.com"
port = 1883
topic = "VLLLCU00B28A717O" 


# Callback when the client connects to the broker
# def on_connect(client, userdata, flags, rc):
#     if rc == 0:
#         print("Connected successfully!")
#     else:
#         print(f"Failed to connect with error code {rc}")

# # Callback when a message is published
# def on_publish(client, userdata, mid):
#     print(f"Message published: {mid}")

#  # Callback when a message is received
# def on_message(client, userdata, msg):
#     message = msg.payload.decode()
#     print(f"Received message: {message} on topic {msg.topic}")
#     # Write to the CSV file
#     writer.writerow([msg.topic, message])



def main():
    # # Start the client loop to handle network traffic
    # # client.loop_start()

    # # # Publish a message
    # # for _ in range(10):
    # #     temperature = random.uniform(20.0, 30.0)
    # #     humidity = random.uniform(40.0, 60.0)
    # #     message = "Temperature: {temperature:.2f}°C, Humidity: {humidity:.2f}%"
    # #     client.publish(topic, message, qos=0)
    # #     time.sleep(2)

    # # # Stop the loop after sending the message
    # # client.loop_stop()

    

    # with open('mqtt_data.csv', mode='w', newline='') as file:
    #     writer = csv.writer(file)
    #     writer.writerow(["Topic", "Message"])

    #     client = mqtt.Client()
    #     # Assign the callbacks
    #     client.on_connect = on_connect
    #     client.on_publish = on_publish
    #     client.on_message = on_message
    #     # Connect to the HiveMQ broker
    #     client.connect(broker, port, 60)
    #     client.subscribe(topic)
    #     # Disconnect the client
    #     client.disconnect()

    def on_connect(client, userdata, flags, rc, properties=None):
        print("CONNACK received with code %s." % rc)

    # with this callback you can see if your publish was successful
    def on_publish(client, userdata, mid, properties=None):
        print("mid: " + str(mid))

    # print which topic was subscribed to
    def on_subscribe(client, userdata, mid, granted_qos, properties=None):
        print("Subscribed: " + str(mid) + " " + str(granted_qos))

    # print message, useful for checking if it was successful
    def on_message(client, userdata, msg):
        print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

    # using MQTT version 5 here, for 3.1.1: MQTTv311, 3.1: MQTTv31
    # userdata is user defined data of any type, updated by user_data_set()
    # client_id is the given name of the client
    client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
    client.on_connect = on_connect

    # enable TLS for secure connection
    client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
    # set username and password
    client.username_pw_set("luca.villani", "Ariel2365!")
    # connect to HiveMQ Cloud on port 8883 (default for MQTT)
    client.connect("83f02f5c58bd4a8bbb0d71e13ece67ee.s1.eu.hivemq.cloud", 8883)

    # setting callbacks, use separate functions like above for better visibility
    client.on_subscribe = on_subscribe
    client.on_message = on_message
    client.on_publish = on_publish

    # subscribe to all topics of encyclopedia by using the wildcard "#"
    client.subscribe("VLLLCU00B28A717O", qos=1)
    client.message_callback_add('VLLLCU00B28A717O', on_message)

    # a single publish, this can also be done in loops, etc.
    # client.publish("VLLLCU00B28A717O", payload="LUCA VILLANI", qos=1)

    # loop_forever for simplicity, here you need to stop the loop manually
    # you can also use loop_start and loop_stop
    client.loop_forever()

if __name__=="__main__":
    main()