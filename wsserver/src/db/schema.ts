import { geometry, pgTable, serial, text, integer, timestamp, varchar, index, boolean } from 'drizzle-orm/pg-core';

export const user = pgTable('user', {
	id: text('id').primaryKey(),
	age: integer('age'),
	username: text('username').notNull().unique(),
	passwordHash: text('password_hash').notNull()
});

export const session = pgTable('session', {
	id: text('id').primaryKey(),
	userId: text('user_id')
		.notNull()
		.references(() => user.id),
	expiresAt: timestamp('expires_at', { withTimezone: true, mode: 'date' }).notNull()
});

export const device = pgTable('device', {
	id: varchar('id', { length: 20 }).primaryKey(),
	name: varchar('name', { length: 50 }).notNull(),
	userid: text('userid').references(() => user.id).notNull(),
	lastknownlocation: geometry('last_known_location', { type: 'point', mode: 'xy', srid: 4326 }).notNull(),
	activesocket: varchar('activesocket', { length: 20 }),
})

export const crashreport = pgTable('crashreport', {
	id: serial('id').primaryKey(),
	deviceid: varchar('deviceid', { length: 20 }).references(() => device.id).notNull(),
	location: geometry('location', { type: 'point', mode: 'xy', srid: 4326 }).notNull(),
	timestamp: timestamp('timestamp'),
}, (t) => ({
	spatialIndex: index('spatial_index').using('gist', t.location),
}))

export type Session = typeof session.$inferSelect;
export type User = typeof user.$inferSelect;
export type Device = typeof device.$inferSelect;
export type CrashReport = typeof crashreport.$inferSelect;
export type CrashReportCreate = typeof crashreport.$inferInsert;
