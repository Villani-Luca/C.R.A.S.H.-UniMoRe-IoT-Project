import type { PageServerLoad } from './$types';
import { db } from '$lib/server/db';
import { device } from '$lib/server/db/schema';

export const load = (async () => {
    const deviceResult = await db.select(
    ).from(device);
    
    const deviceList = deviceResult.map((x) => ({name: x.name, lat: x.lastknownlocation?.x.valueOf()!, long: x.lastknownlocation?.y.valueOf()!}))

    

    return {deviceList };
}) satisfies PageServerLoad;