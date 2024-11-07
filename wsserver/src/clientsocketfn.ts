import { db } from "./db";
import { CrashNotification, CrashNotificationAnon, PositionUpdate } from "./messages";
import { DisconnectReason, Socket, Server } from "socket.io";
import { create_crashreport, disconnect_device, get_device_inrange, update_device_lastknownlocation } from "./db/repository";

export async function client_position_update(message: string, socket: Socket) {
  const json: PositionUpdate = JSON.parse(message);

  await update_device_lastknownlocation(db, {
    id: json.device!,
    lastknownlocation: { x: json.longitude, y: json.latitude },
    activesocket: socket.id
  });
}

export async function crash_client_notification(message: string, socket: Socket, server: Server) {
  const json: CrashNotification = JSON.parse(message);

  const location = { x: json.longitude, y: json.latitude };
  await db.transaction(async (tx) => {
    await update_device_lastknownlocation(tx as any, {
      id: json.device!,
      lastknownlocation: location,
      activesocket: socket.id
    });

    await create_crashreport(tx as any, {
      deviceid: json.device,
      location: location,
      timestamp: new Date(),
    })
  })

  const devices_in_range = await get_device_inrange(db, { location, range: 10000 });
  const clients = await server.sockets.fetchSockets()

  const clients_in_range = clients.filter(x => devices_in_range.find(d => d.activesocket === x.id))
  for (const client of clients_in_range) {
    client.emit('server-crash-notification', JSON.stringify({
      longitude: location.x,
      latitude: location.y,
    } satisfies CrashNotificationAnon))
  }
}

async function client_disconnect(reason: DisconnectReason, socket: Socket) {
  console.log('Disconnected socket', socket.id, reason);
  await disconnect_device(db, socket.id);
}

export function client_socket_connected(socket: Socket, server: Server) {
  console.log('Socket connected', socket.id);
  socket.on('client-position-update', (message) => client_position_update(message, socket))
  socket.on('client-crash-notification', (message) => crash_client_notification(message, socket, server));
  socket.on('disconnect', (reason) => client_disconnect(reason, socket))
}

