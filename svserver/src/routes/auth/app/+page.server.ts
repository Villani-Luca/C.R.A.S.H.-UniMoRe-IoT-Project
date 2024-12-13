import type { Actions, PageServerLoad } from './$types';
import { db } from '$lib/server/db';
import { crashreport, device } from '$lib/server/db/schema';
import { sql, and, eq, count, gt, gte, isNotNull } from 'drizzle-orm';
import { error, redirect } from '@sveltejs/kit';
import { prophet_predictions } from '$lib/server/prophet';

export const load = (async ({ url, locals }) => {
  const session = await locals.user?.id;
  if (!session) {
    throw redirect(302, "/");
  }

  const deviceResult = await db
    .select()
    .from(device)
    .where(eq(device.userid, session));

  const deviceList = deviceResult.map((x) => ({ 
    id: x.id,
    name: x.name, 
    lat: x.lastknownlocation?.x.valueOf()!, 
    long: x.lastknownlocation?.y.valueOf()!,
    active: !!x.lastknownlocation
  }));

  let radius: number = url.searchParams.has('r') ? parseInt(url.searchParams.get('r')!) : 5;
  let deviceid: string| undefined = url.searchParams.get('deviceid') ?? undefined;
  let predictions_enabled: string | undefined = url.searchParams.get('predictions') ?? undefined;
  let activedevice = deviceList.filter(x => x.active).find(x => x.id === deviceid) ?? deviceList.filter(x => x.active)[0];
  
  let timestamp_crash_threshold = new Date();
  timestamp_crash_threshold.setUTCFullYear(timestamp_crash_threshold.getUTCFullYear() - 1);

  let timestamp_crash_threshold_map = new Date();
  timestamp_crash_threshold_map.setDate(timestamp_crash_threshold_map.getDate() - 7);

  const crashResult = activedevice ? await db
    .select()
    .from(crashreport)
    .where(and(
      sql`ST_Dwithin(${crashreport.location}::geography, ST_MakePoint(${activedevice.lat}, ${activedevice.long})::geography, ${radius * 1000}, true)`,
      gte(crashreport.timestamp, timestamp_crash_threshold_map),
    )) : [];

  const crashList = crashResult.map((x) => ({ 
    id: x.id,
    lat: x.location.x, 
    long: x.location.y, 
    time: x.timestamp! 
  }));
  
  let predictions: ReturnType<typeof prophet_predictions> | undefined;
  if(predictions_enabled === "true"){
    const crashes = activedevice ? await db
      .select({
        ds: sql`${crashreport.timestamp}::date`.mapWith((x) => new Date(x)),
        y: count()
      })
      .from(crashreport)
      .where(and(
        sql`ST_Dwithin(${crashreport.location}::geography, ST_MakePoint(${activedevice.lat}, ${activedevice.long})::geography, ${radius * 1000}, true)`,
        gte(crashreport.timestamp, timestamp_crash_threshold),
      )) 
      .groupBy(sql`${crashreport.timestamp}::date`)
      : [];
      
    predictions = prophet_predictions(session, crashes, 'D');
  }

  return {
    radius: radius,
    activedevice: activedevice,
    predictions_enabled: predictions_enabled == "true",

    deviceList: deviceList,
    crashList: crashList,

    streamed: {
      predictions,
    }
  }
}) satisfies PageServerLoad;

export const actions = {
  gencrash: async ({request, locals}) => {
    const userid = locals.user?.id;
    if(!userid){
      return error(400);
    }
    
    const data = await request.formData();
    const radius = data.get('radius') ? Number(data.get('radius')) : undefined;
    const deviceid = data.has('deviceid') ? String(data.get('deviceid')) : undefined;
    const number = data.has('number') ? Number(data.get('number')) : undefined;
    
    if(!radius || !deviceid || !number){
      return error(400);
    }
    
    const deviceresult = await db
      .select()
      .from(device)
      .where(
        and(
          eq(device.userid, userid), 
          eq(device.id, deviceid),
          isNotNull(device.lastknownlocation),
        )
      );
      
    const alldevices = await db.select().from(device);
      
    if(deviceresult.length <= 0){
      return error(400);
    }
    
    const activedevice = deviceresult[0];
    const crashes: typeof crashreport.$inferInsert[] = [];
    const currenttime = new Date();
    const starttime = new Date(new Date(+currenttime).setMonth(currenttime.getMonth() - 3))
    
    
    const lat = radius / 110.574;
    const long = radius / (111.320 * Math.cos(lat));

    for(let n = 0; n < number; n++){
      let x = Math.random()*2 - 1;
      let yrange = Math.sin(Math.acos(x));
      let y = Math.random()*2*yrange - yrange;
      let ts = new Date(starttime.getTime() + Math.random() * (currenttime.getTime() - starttime.getTime()));
      
      crashes.push({
        deviceid: alldevices[Math.floor(Math.random()*alldevices.length)].id,
        timestamp: ts,
        location: {
          x: activedevice.lastknownlocation!.x + lat*x,
          y: activedevice.lastknownlocation!.y + long*y,
        },
      })
    }
    
    await db.insert(crashreport).values(crashes);
    return {
      success: true
    };
  }
} satisfies Actions
