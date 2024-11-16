import { db } from "./db";
import { CrashNotification, CrashNotificationAnon, PositionUpdate } from "./messages";
import { create_crashreport, get_device_inrange, update_device_lastknownlocation } from "./db/repository";
import mqtt from 'mqtt';

export async function client_position_update(message: string) {
  const json: PositionUpdate = JSON.parse(message);

  await update_device_lastknownlocation(db, {
    id: json.device!,
    lastknownlocation: { x: json.longitude, y: json.latitude },
    lastknownupdate: new Date(),
  });
}

export async function crash_client_notification(message: string, client: mqtt.MqttClient) {
  const json: CrashNotification = JSON.parse(message);

  const location = { x: json.longitude, y: json.latitude };
  await db.transaction(async (tx) => {
    await update_device_lastknownlocation(tx as any, {
      id: json.device!,
      lastknownlocation: location,
      lastknownupdate: new Date(),
    });

    await create_crashreport(tx as any, {
      deviceid: json.device,
      location: location,
      timestamp: new Date(),
    })
  })

  // TODO rendere parametrici range, timestamp e timerange
  const devices_in_range = await get_device_inrange(db, { location, range: 10000, timestamp: new Date(), timerange: 5 * 60 });
  for (const device of devices_in_range.filter(x => x.id !== json.device)) {
    const crash_topic = `update/${device.id}/crash`;
    client.publish(
      crash_topic,
      JSON.stringify({
        longitude: location.x,
        latitude: location.y,
      } satisfies CrashNotificationAnon)
    )
  }
}
