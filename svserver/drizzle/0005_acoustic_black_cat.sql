ALTER TABLE "device" ALTER COLUMN "last_known_location" SET NOT NULL;--> statement-breakpoint
ALTER TABLE "device" ADD COLUMN "last_known_update" timestamp with time zone NOT NULL;--> statement-breakpoint
ALTER TABLE "device" DROP COLUMN IF EXISTS "activesocket";