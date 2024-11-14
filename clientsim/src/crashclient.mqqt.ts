import { CrashNotification, CrashNotificationAnon, PositionUpdate } from './messages';
import mqtt from "mqtt"; // import namespace "mqtt"


export function crash_mqtt(device: string) {
  console.info("Avvio crash socket client DEVICE: " + device);


  const client = mqtt.connect("mqtt://localhost:1883", {
    username: 'admin',
    password: 'instar'
  });
  client.on('connect', function() {
    console.log("presence")
    client.publish('presence', 'Hello mqtt')
  })

  client.on('error', function(err) {
    console.log('error', err)
  })

  if (!client.connected) {
    client.connected
    console.log("Error during connection mqqt");
  }

  const crash_topic = `update/${device.toLowerCase()}/crash`;
  client.subscribe(crash_topic)

  client.on("message", (topic, message) => {
    console.log(topic, message.toString());

    if (topic === crash_topic) {
      server_crash_notification(message.toString());
    }
  });


  setInterval(() => {
    notify_position_update(client, {
      device: device,
      latitude: 44 + Math.random(),
      longitude: 10 + Math.random(),
    });
  }, 2000)

  setInterval(() => {
    notify_crash(client, {
      device: device,
      latitude: 44 + Math.random(),
      longitude: 10 + Math.random(),
    });
  }, 10000)


  // disconnect after 2 minutes
  setTimeout(() => {
    client.end();
  }, 1000 * 120)
}

function notify_position_update(client: mqtt.MqttClient, data: PositionUpdate) {
  console.log('client-position-update');
  client.publish('client-position-update', JSON.stringify(data));
}

function notify_crash(client: mqtt.MqttClient, data: CrashNotification) {
  console.log('client-crash-notification');
  client.publish('client-crash-notification', JSON.stringify(data));
}

function server_crash_notification(message: string) {
  const data: CrashNotificationAnon = JSON.parse(message);
  console.log(data)
}
