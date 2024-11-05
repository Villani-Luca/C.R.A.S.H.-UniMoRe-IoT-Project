import express from 'express';
import { createServer } from 'http';
import { Server } from 'socket.io';
import { handler } from './build/handler.js';
import { eq } from 'drizzle-orm';
import { PositionUpdate, CrashNotification } from './src/lib/server/messages/index.js';
import { db } from './src/lib/server/db/index.js';
import { device } from './src/lib/server/db/schema.js';

const port = 3000;
const app = express();
const server = createServer(app);
const io = new Server(server);

io.on('connection', (socket) => {
  console.log('Socket connected', socket.id);
  socket.emit('connection-evt', "hello");
})

io.on('client-position-update', async (message) => {
  /** @type PositionUpdate */
  const json = JSON.parse(message);

  await db.update(device).set({
    lastknownlocation: { x: json.longitude, y: json.latitude }
  })
    .where(
      eq(device.id, json.device),
    )
})

io.on('client-crash-notification', async (message) => {
  /** @type CrashNotification */
  const json = JSON.parse(message);


  await db.update(device).set({
    lastknownlocation: { x: json.longitude, y: json.latitude }
  })
    .where(
      eq(device.id, json.device),
    )
})

app.use(handler);
server.listen(port);

