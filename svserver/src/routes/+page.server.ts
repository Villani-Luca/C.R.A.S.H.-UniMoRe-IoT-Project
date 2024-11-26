import type { PageServerLoad } from './$types';
import { db } from '$lib/server/db';
import { device } from '$lib/server/db/schema';

export const load = (async () => {
    const result = await db.select(
    ).from(device);

    

    return {result};
}) satisfies PageServerLoad;