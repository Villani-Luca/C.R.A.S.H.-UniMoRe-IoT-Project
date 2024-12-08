import type { PageServerLoad } from './$types';
import { db } from '$lib/server/db';
import { crashreport, device, user } from '$lib/server/db/schema';
import type { User_Position } from '$lib/helpers/user_position';
import { sql, and, eq } from 'drizzle-orm';
import { redirect } from '@sveltejs/kit';


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

  return {
    radius: radius,
    activedevice: activedevice,

    deviceList: deviceList,
    crashList: crashList
  }
}) satisfies PageServerLoad;
