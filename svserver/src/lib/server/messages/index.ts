

export type PositionUpdate = {
  device: string;
  latitude: number;
  longitude: number;
}

export type CrashNotification = {
  // Usato solo da client => server
  device?: string;
  latitude: number;
  longitude: number;
}
