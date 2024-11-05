import * as commandeer from 'commander';
import { crash_socket_action } from './crashclient.socket';
import { error_handler } from './common';

const program = new commandeer.Command();
program.version('1.0.0');

program
  .command('crashclient')
  .description('Avvia il simulatore client tramite websocket')
  .action(error_handler<any>(crash_socket_action))

/*
 * implementazione mqqt
program
  .command('recvclient')
  .description('Avvia il simulatore client tramite mqqt')
  .action(recv_action)
*/

program.parse(process.argv);
