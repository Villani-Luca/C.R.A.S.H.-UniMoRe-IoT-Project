import { io, Socket } from 'socket.io-client';
import { CrashNotification, CrashNotificationAnon, PositionUpdate } from './messages';


export function crash_socket_action(device: string) {
  console.info("Avvio crash socket client DEVICE: " + device);

  const socket = io("ws://localhost:3000");

  socket.on('connection', __connection);
  socket.on('server-crash-notification', server_crash_notification);

  setInterval(() => {
    notify_position_update(socket, {
      device: device,
      latitude: 100 * Math.random(),
      longitude: 100 * Math.random(),
    });
  }, 2000)

  setInterval(() => {
    notify_crash(socket, {
      device: device,
      latitude: 100 * Math.random(),
      longitude: 100 * Math.random(),
    });
  }, 10000)
}

function __connection() {
  console.log('Socket connected');
}

function notify_position_update(socket: Socket, data: PositionUpdate) {
  console.log('client-position-update');
  socket.emit('client-position-update', JSON.stringify(data));
}

function notify_crash(socket: Socket, data: CrashNotification) {
  console.log('client-crash-notification');
  socket.emit('client-crash-notification', JSON.stringify(data));
}

function server_crash_notification(message: string) {
  const data: CrashNotificationAnon = JSON.parse(message);
}
