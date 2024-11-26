import { eq, sql, and, gte } from "drizzle-orm";
import type { DrizzleDb } from "./index";
import { crashreport, type CrashReport } from "./schema";


export function get_crashes_in_range(db: DrizzleDb, data: { location: {x:number, y:number}, range: number, timestamp: Date, timerange: number }) {
    const {
      location,
      range,
      timestamp,
      timerange
    } = data;
  
    timestamp.setSeconds(timestamp.getSeconds() - timerange)
  
    return db
      .select()
      .from(crashreport)
      .where(and(
        sql`
          ST_Dwithin(${crashreport.location}::geography, ST_MakePoint(${location.x}, ${location.y})::geography, ${range})
        `,
        gte(crashreport.timestamp, timestamp)
      ))
  }
  