import { eq, sql, and, gte } from "drizzle-orm";
import type { DrizzleDb } from "./index";
import { crashreport, CrashReport, CrashReportCreate, Device, device } from "./schema";


export function update_device_lastknownlocation(db: DrizzleDb, data: Pick<Device, 'lastknownlocation' | 'id' | 'lastknownupdate'>) {
  return db
    .update(device)
    .set(data)
    .where(eq(device.id, data.id!))
}

export function create_crashreport(db: DrizzleDb, data: CrashReportCreate) {
  return db
    .insert(crashreport)
    .values(data);
}

/**
 * Range deve essere in metri,
 * timestamp corrisponde a quando é avvenuto l'evento
 * timerange serve per filtrare i device che non hanno fornito un aggiornamento di posizione entro <timerange> secondi
 */
export function get_device_inrange(db: DrizzleDb, data: { location: CrashReport['location'], range: number, timestamp: Date, timerange: number }) {
  const {
    location,
    range,
    timestamp,
    timerange
  } = data;

  timestamp.setSeconds(timestamp.getSeconds() - timerange)

  return db
    .select()
    .from(device)
    .where(and(
      sql`
        ST_Dwithin(${device.lastknownlocation}::geography, ST_MakePoint(${location.x}, ${location.y})::geography, ${range})
      `,
      gte(device.lastknownupdate, timestamp)
    ))
}
