import { io, Socket } from 'socket.io-client';


const DEVICEID = 'DEV-123';
export function crash_socket_action() {
  console.info("Avvio crash socket client");

  const socket = io("ws://localhost:8080");

  // TODO: 3 eventi principali per gestione delle connessioni
  // TODO: eventi che ci interessano a noi ( principalmente l'evento di crash );
  // TODO: invio delle coordinate attuali ogni x 
  // TODO: gestione della firma del digital twin e nel caso criptazione del messaggio 

  socket.on('connection', __connection);
  socket.on('connection-evt', __connection_evt);

  setTimeout(() => {
    positionupdate(socket, {
      device: DEVICEID,
      latitude: 100 * Math.random(),
      longitude: 100 * Math.random(),
    });
  }, 1000)
}

function __connection() {
  console.log('Socket connected');
}

function __connection_evt(message: string) {
  console.log("connection-evt", message);
}

function positionupdate(socket: Socket, data: PositionUpdate) {
  socket.emit('client-position-update', JSON.stringify(data));
}

type PositionUpdate = {
  device: string,
  latitude: number,
  longitude: number,
}
