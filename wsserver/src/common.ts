
export function error_handler<T>(fn: (...params: T[]) => void) {
  return (...params: T[]) => {
    try {
      fn(...params);
    }
    catch (error) {
      if (error instanceof Error) {
        console.error(`Error: ${error.message}`);
      }
      else {
        console.error(`CRITICAL ERROR ${error}`);
      }

      process.exit(1);
    }
  }
}
