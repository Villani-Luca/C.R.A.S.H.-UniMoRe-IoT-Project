import {env as e} from '$env/dynamic/private';

const env = {
    DATABASE_URL: e.DATABASE_URL,
    PROPHET_FILE: e.PROPHET_FILE,
    PVENV_PATH: e.PVENV_PATH,
}
export default env;