import type { PageServerLoad } from './$types';
import { db } from '$lib/server/db';
import { crashreport, device, user } from '$lib/server/db/schema';
import type { User_Position } from '$lib/helpers/user_position';
import { sql, and } from 'drizzle-orm';


export const load = (async ( {url}) => {
    const deviceResult = await db.select(
    ).from(device);
    
    const deviceList = deviceResult.map((x) => ({name: x.name, lat: x.lastknownlocation?.x.valueOf()!, long: x.lastknownlocation?.y.valueOf()!}));
   
    let radius: number = url.searchParams.has('r') ? parseInt(url.searchParams.get('r')!) : 5 ;
    let lat: number = url.searchParams.has('lat') ? parseFloat(url.searchParams.get('lat')!) : 0;
    let long: number = url.searchParams.has('long') ? parseFloat(url.searchParams.get('long')!) : 0;

    let user_position: User_Position ={latitude: 0, longitude:0};

    console.log(user_position.latitude);

   const crashResult= await db
          .select()
          .from(crashreport)
          .where(and(
            sql`
              ST_Dwithin(${crashreport.location}::geography, ST_MakePoint(${lat}, ${long})::geography, ${radius*1000})
            `
          ));
    
    const crashList = crashResult.map((x)=> ({lat: x.location.x, long: x.location.y, deviceId: x.deviceid, time: x.timestamp}));

    return {
        deviceList: deviceList,
        radius: radius,
        user_position: user_position,
        crashList: crashList
     }
}) satisfies PageServerLoad;