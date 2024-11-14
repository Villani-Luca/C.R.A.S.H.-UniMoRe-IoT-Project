import { db } from "./db";
import { CrashNotification, CrashNotificationAnon, PositionUpdate } from "./messages";
import { DisconnectReason, Socket, Server } from "socket.io";
import { create_crashreport, disconnect_device, get_device_inrange, update_device_lastknownlocation } from "./db/repository";
import mqtt from 'mqtt';

export async function client_position_update(message: string) {
  const json: PositionUpdate = JSON.parse(message);

  await update_device_lastknownlocation(db, {
    id: json.device!,
    lastknownlocation: { x: json.longitude, y: json.latitude },
  });
}

export async function crash_client_notification(message: string, client: mqtt.MqttClient) {
  const json: CrashNotification = JSON.parse(message);

  const location = { x: json.longitude, y: json.latitude };
  await db.transaction(async (tx) => {
    await update_device_lastknownlocation(tx as any, {
      id: json.device!,
      lastknownlocation: location,
    });

    await create_crashreport(tx as any, {
      deviceid: json.device,
      location: location,
      timestamp: new Date(),
    })
  })

  const devices_in_range = await get_device_inrange(db, { location, range: 10000 });
  for (const device of devices_in_range.filter(x => x.id !== json.device)) {
    const crash_topic = `update/${device.id.toLowerCase()}/crash`;
    client.publish(
      crash_topic,
      JSON.stringify({
        longitude: location.x,
        latitude: location.y,
      } satisfies CrashNotificationAnon)
    )
  }
}

async function client_disconnect(reason: DisconnectReason, socket: Socket) {
  console.log('Disconnected socket', socket.id, reason);
  await disconnect_device(db, socket.id);
}

// export function client_socket_connected(socket: Socket, server: Server) {
//   console.log('Socket connected', socket.id);
//   socket.on('client-position-update', (message) => client_position_update(message, socket))
//   socket.on('client-crash-notification', (message) => crash_client_notification(message, socket, server));
//   socket.on('disconnect', (reason) => client_disconnect(reason, socket))
// }

