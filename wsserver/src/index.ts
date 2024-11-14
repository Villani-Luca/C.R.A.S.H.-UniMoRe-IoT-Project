import 'dotenv/config';
import { client_position_update, crash_client_notification } from './clientsocketfn';
import mqtt from 'mqtt';


const client = mqtt.connect(process.env.MQTT_URL!, {
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD
});

client.on('connect', () => {

})

client.on('error', (err) => {
  console.error('[MQQT] Error', err);
})

client.subscribe('client-position-update', (err) => {
  console.log('Client Position Update - Sub Error', err);
})

client.subscribe('client-crash-notification', (err) => {
  console.log('Client Crash Notification - Sub Error', err);
})

client.on('message', (topic, data) => {
  const message = data.toString();

  switch (topic) {
    case "client-position-update":
      console.log(message);
      client_position_update(message);
      break;
    case "client-crash-notification":
      console.log(message);
      crash_client_notification(message, client);
      break;
  }
})

// TESTS
// get_device_inrange(db, {
//   location: {
//     x: 44.698795,
//     y: 10.656367
//   },
//   range: 10000,
// }).then((result) => {
//   console.log('test_1', result)
// })
//
// get_device_inrange(db, {
//   location: {
//     x: 44.698795,
//     y: 10.656367
//   },
//   range: 5000,
// }).then((result) => {
//   console.log('test_2', result)
// })
//
// get_device_inrange(db, {
//   location: {
//     x: 44.698795,
//     y: 10.656367
//   },
//   range: 15000,
// }).then((result) => {
//   console.log('test_3', result)
// })
//
// get_device_inrange(db, {
//   location: {
//     x: 44.698795,
//     y: 10.656367
//   },
//   range: 29000,
// }).then((result) => {
//   console.log('test_4', result)
// })
//
// get_device_inrange(db, {
//   location: {
//     x: 44.698795,
//     y: 10.656367
//   },
//   range: 30000,
// }).then((result) => {
//   console.log('test_5', result)
// })
