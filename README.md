# easy_blackbox_logger
create blackbox csv log file with  an easy and simple cpp class.


# usage example:

#include "BlackBoxLogger.h"

int main() {
    // Create a logger that writes to ./output by default
    BlackBoxLogger logger;

    // First row: add two fields
    logger.insert("altitude", 100.0);
    logger.insert("speed", 10.0);
    logger.newRow();  // Writes: altitude=100, speed=10

    // Second row: update only speed
    logger.insert("speed", 12.5);
    logger.newRow();  // Reuses altitude=100, writes: altitude=100, speed=12.5

    // Third row: add a new field 'temperature', update altitude
    logger.insert("temperature", 23.5);
    logger.insert("altitude", 101.2);
    logger.newRow();  // Reuses speed=12.5, adds zeros for temperature in previous rows

    // Fourth row: update only temperature
    logger.insert("temperature", 24.1);
    logger.newRow();  // Reuses other fields, writes: altitude=101.2, speed=12.5, temperature=24.1

    return 0;  // Logger destructor will automatically save everything
}
