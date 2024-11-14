import { eq, sql } from "drizzle-orm";
import type { DrizzleDb } from "./index";
import { crashreport, CrashReport, CrashReportCreate, Device, device } from "./schema";


export function update_device_lastknownlocation(db: DrizzleDb, data: Pick<Device, 'lastknownlocation' | 'id'>) {
  return db
    .update(device)
    .set(data)
    .where(eq(device.id, data.id!))
}

export function disconnect_device(db: DrizzleDb, id: NonNullable<Device['activesocket']>) {
  return db
    .update(device)
    .set({ activesocket: null })
    .where(eq(device.activesocket, id))
}

export function create_crashreport(db: DrizzleDb, data: CrashReportCreate) {
  console.log(data);
  return db
    .insert(crashreport)
    .values(data);
}

/**
 * Range deve essere in metri 
 * TODO: da testare
 */
export function get_device_inrange(db: DrizzleDb, data: { location: CrashReport['location'], range: number }) {
  return db
    .select()
    .from(device)
    .where(sql`
      ST_Dwithin(${device.lastknownlocation}::geography, ST_MakePoint(${data.location.x}, ${data.location.y})::geography, ${data.range})
    `)
}
