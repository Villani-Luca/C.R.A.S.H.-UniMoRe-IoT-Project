
const env = {
    //CSV_REPO: extract('CSV_REPO'),
    DATABASE_URL: extract('DATABASE_URL'),
    PROPHET_FILE: extract('PROPHET_FILE'),
    PVENV_PATH: extract('PVENV_PATH'),
}

function extract(name: string, optional?: false): string
function extract(name: string, optional: true): string | undefined
function extract(name: string, optional?: boolean): string | undefined {
    const value = process.env[name];
    if(!value && optional === false){
        throw new Error(`ENV ${name} NOT SET`);
    }
    
    return value;
}

export default env;