import 'dotenv/config';
import { client_position_update, crash_client_notification } from './topichandlers';
import mqtt from 'mqtt';

const client = mqtt.connect(process.env.MQTT_URL!, {
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD,
});

console.info('Client is connected', client.connected);
client.on('connect', () => {
  console.info("[MQQT] Connected");

  client.subscribe('client-position-update', (err: any) => {
    console.info('[MQTT] Client Position Update Subscription --- Error: ', err ?? 'none');
  })

  client.subscribe('client-crash-notification', (err: any) => {
    console.info('[MQTT] Client Crash Notification Subscription --- Error: ', err ?? 'none');
  })
})

client.on('error', (err: any) => {
  console.error('[MQQT] Error', err);
})

const topic_handlers: Record<string, ((msg: string) => Promise<void>) | undefined> = {
  "client-position-update": (msg: string) => client_position_update(msg),
  "client-crash-notification": (msg: string) => crash_client_notification(msg, client),
};

client.on('message', async (topic: string, data: Buffer) => {
  const message = data.toString();
  console.debug(topic, message);

  const handler = topic_handlers[topic];
  if (!handler) {
    console.error("[MQTT] Invalid topic no handler");
    return;
  }

  await handler(message);
})

