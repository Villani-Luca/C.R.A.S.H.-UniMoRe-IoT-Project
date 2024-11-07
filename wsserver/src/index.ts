import 'dotenv/config';
import express from 'express';
import http from 'http';
import { Server } from 'socket.io';
import { client_socket_connected } from './clientsocketfn';


const app = express();
const server = http.createServer(app);

const io = new Server(server);
io.on('connection', (socket) => client_socket_connected(socket, io));

server.listen(process.env.PORT, () => {
  console.log(`Listening on *:${process.env.PORT}`);
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
