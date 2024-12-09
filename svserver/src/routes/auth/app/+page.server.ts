import type { Actions, PageServerLoad } from './$types';
import { db } from '$lib/server/db';
import { crashreport, device } from '$lib/server/db/schema';
import { sql, and, eq } from 'drizzle-orm';
import { redirect } from '@sveltejs/kit';
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
  let deviceid: string | undefined = url.searchParams.get('deviceid') ?? undefined;
  let predictions_enabled: string | undefined = url.searchParams.get('predictions') ?? undefined;
  let activedevice = deviceList.filter(x => x.active).find(x => x.id === deviceid) ?? deviceList.filter(x => x.active)[0];

  const crashResult = activedevice ? await db
    .select()
    .from(crashreport)
    .where(and(
      sql`ST_Dwithin(${crashreport.location}::geography, ST_MakePoint(${activedevice.lat}, ${activedevice.long})::geography, ${radius * 1000})`
    )) : [];

  const crashList = crashResult.map((x) => ({ 
    id: x.id,
    lat: x.location.x, 
    long: x.location.y, 
    time: x.timestamp! 
  }));
  console.log("pageserver crashlist", crashList);
  
  let predictions: ReturnType<typeof prophet_predictions> | undefined;
  if(predictions_enabled === "true"){
    const crashes = [
      { ds: new Date(2024, 12, 9), y: 10, },
      { ds: new Date(2024, 12, 10), y: 20, },
      { ds: new Date(2024, 12, 11), y: 15, },
      { ds: new Date(2024, 12, 12), y: 11, },
      { ds: new Date(2024, 12, 13), y: 30, },
    ]
    
    predictions = prophet_predictions(session, crashes);
    
    console.log("predictions", await predictions);
  }

  return {
    radius: radius,
    activedevice: activedevice,
    predictions_enabled: predictions_enabled == "true",

    predictions,

    deviceList: deviceList,
    crashList: crashList
  }
}) satisfies PageServerLoad;

export const actions = {

} satisfies Actions
